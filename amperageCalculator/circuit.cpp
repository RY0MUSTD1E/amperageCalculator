/**
 * \file Circuit.cpp
 * \brief Реализация класса Circuit.
 * 
 * \author Anna Bezhenar
 * \date June 2026
 */

#include "circuit.h"
#include <string>
#include <set>
#include <fstream>
#include <regex>

using namespace std;

Circuit::Circuit()
    : frequency(0.0)
    , sourceVoltage(0.0)
    , sourcePhase(0.0)
{
}

Circuit::~Circuit() {
    for (auto* node : nodes) {
        delete node;
    }
    nodes.clear();
    for (auto* branch : branches) {
        delete branch;
    }
    branches.clear();
}

CircuitNode* Circuit::getNodeByName(const string& name) const {
    auto it = nameToNode.find(name);
    return (it != nameToNode.end()) ? it->second : nullptr;
}

size_t Circuit::getNodeCount() const {
    return nodes.size();
}

size_t Circuit::getEdgeCount() const {
    return edges.size();
}

const Error& Circuit::getError() const {
    return error;
}

const vector<CircuitBranch*>& Circuit::getBranches() const {
    return branches;
}

string Circuit::trim(const string& s) {
    // Найти первый символ, не являющийся пробельным
    size_t start = s.find_first_not_of(" \t\r\n");
    if (start == string::npos) {
        return "";
    }
    // Найти последний непробельный символ и вернуть подстроку
    size_t end = s.find_last_not_of(" \t\r\n");
    return s.substr(start, end - start + 1);
}

void Circuit::clearCircuitData() {
    // Очистить все узлы и ветви цепи
    for (auto* node : nodes) delete node;
    nodes.clear();
    for (auto* branch : branches) delete branch;
    branches.clear();

    // Очистить связи и вспомогательные структуры
    edges.clear();
    nameToNode.clear();
    nameToIndex.clear();
    nodeDefinitions.clear();

    // Сбросить параметры источника
    frequency = 0.0;
    sourceVoltage = 0.0;
    sourcePhase = 0.0;

    error.clear();
}

bool Circuit::loadFile(const string& filename, vector<string>& lines) {
    // Открыть файл для чтения
    ifstream file(filename);

    if (!file.is_open()) {
        error.setError(ErrorType::InputFileNotExist, filename);
        return false;
    }

    string line;
    bool hasContent = false;

    // Построчно прочитать файл
    while (getline(file, line)) {
        // Проверить, есть ли в строке непробельные символы
        if (!trim(line).empty()) {
            hasContent = true;
        }
        lines.push_back(line);
    }

    file.close();

    // Если файл пуст
    if (!hasContent) {
        error.setError(ErrorType::FileEmpty);
        return false;
    }

    return true;
}

bool Circuit::validateDotStructure(const vector<string>& lines) {
    // Первая строка должна содержать ключевое слово "digraph"
    if (lines.empty() || trim(lines[0]).find("digraph") != 0) {
        error.setError(ErrorType::MissingDigraph);
        return false;
    }

    bool hasOpenBrace = false;
    bool hasCloseBrace = false;

    // Поиск открывающей и закрывающей фигурных скобок
    for (const auto& l : lines) {
        if (l.find('{') != string::npos) hasOpenBrace = true;
        if (l.find('}') != string::npos) hasCloseBrace = true;
    }

    // Если отсутствует открывающая скобка
    if (!hasOpenBrace) {
        error.setError(ErrorType::MissingOpenBrace);
        return false;
    }

    // Если отсутствует закрывающая скобка
    if (!hasCloseBrace) {
        error.setError(ErrorType::MissingCloseBrace);
        return false;
    }

    return true;
}

bool Circuit::parseGraph(const vector<string>& lines, vector<ParamsOfNode>& parsedNodes) {
    set<string> parsedNames;          // Множество уже встреченных имен узлов
    smatch match;                     // Для хранения результатов regex поиска
    bool insideGraph = false;         // Флаг нахождения внутри фигурных скобок
    int lineNum = 0;                  // Текущий номер строки

    for (const auto& rawLine : lines) {
        lineNum++;
        string s = trim(rawLine);     // Удалить пробелы по краям

        // Пропустить пустые строки и комментарии
        if (s.empty() || s[0] == '#') {
            continue;
        }

        // Обработка открывающей скобки
        if (s.find('{') != string::npos) {
            insideGraph = true;
            continue;
        }

        // Обработка закрывающей скобки
        if (s == "}") {
            insideGraph = false;
            continue;
        }

        // Строки вне графа пропустить
        if (!insideGraph) {
            continue;
        }

        // Парсинг ребра (связь между узлами)
        if (s.find("->") != string::npos) {
            if (!processEdge(s, match, lineNum))
                return false;
            continue;
        }

        // Парсинг узла (компонент цепи с атрибутами)
        if (s.find('[') != string::npos) {
            if (!processNode(s, match, parsedNames, parsedNodes, lineNum, rawLine))
                return false;
            continue;
        }

        // Нераспознанная конструкция
        if (!s.empty() && s != "{") {
            error.setError(ErrorType::InvalidCircuitTopology, "", lineNum);
            return false;
        }
    }

    return true;
}

bool Circuit::processEdge(const string& s, smatch& match, int lineNum) {
    // Регулярное выражение для формата A -> B
    regex edgeRegex(R"(\s*(\w+)\s*->\s*(\w+)\s*)");

    if (!regex_match(s, match, edgeRegex)) {
        error.setError(ErrorType::InvalidConnectionSyntax, "", lineNum);
        return false;
    }

    string src = match[1];   // Имя узла-источника
    string dst = match[2];   // Имя узла-приёмника

    if (src.empty() || dst.empty()) {
        error.setError(ErrorType::InvalidConnectionSyntax, "", lineNum);
        return false;
    }

    // Запретить соединение элемента с самим собой
    if (src == dst) {
        error.setError(ErrorType::SelfConnection, src, lineNum);
        return false;
    }

    edges.emplace_back(src, dst);
    return true;
}

bool Circuit::processNode(const string& s, smatch& match, set<string>& parsedNames, vector<ParamsOfNode>& parsedNodes, int lineNum, const string& rawLine) {
    // Регулярное выражение для формата: NodeName [label="..."]
    regex nodeRegex(R"(\s*(\w+)\s*\[label\s*=\s*\"([^\"]*)\"\s*\])");

    if (!regex_match(s, match, nodeRegex)) {
        error.setError(ErrorType::InvalidCircuitTopology, "", lineNum);
        return false;
    }

    string nodeName = match[1];   // Имя узла
    string label = match[2];      // Содержимое label

    // Проверить корректность имени узла
    if (!isValidName(nodeName)) {
        error.setError(ErrorType::InvalidComponentName, nodeName, lineNum);
        return false;
    }

    // Проверить уникальность имени
    if (parsedNames.count(nodeName)) {
        error.setError(ErrorType::DuplicateComponent, nodeName, lineNum);
        return false;
    }

    // Label не должен быть пустым
    if (label.empty()) {
        error.setError(ErrorType::EmptyLabel, nodeName, lineNum);
        return false;
    }

    parsedNames.insert(nodeName);

    ParamsOfNode params;
    params.name = nodeName;

    // Парсинг содержимого label
    if (!parseLabel(label, params, lineNum)) {
        return false;
    }

    parsedNodes.push_back(params);
    nodeDefinitions.push_back(rawLine);   // Сохранить исходную строку для вывода

    return true;
}

void Circuit::buildNodes(const vector<ParamsOfNode>& parsedNodes) {
    // Пройти по всем распарсенным параметрам узлов
    for (const auto& params : parsedNodes) {
        // Создать новый узел цепи
        CircuitNode* node =
            new CircuitNode(params.name, params.type, params.nominal);

        node->setOriginalLabel(params.originalLabel);

        // Если узел является источником то сохранить его параметры
        if (params.type == NodeType::Source) {
            node->setVoltage(params.voltage);
            node->setFrequency(params.frequency);
            node->setPhase(params.phase);

            frequency = params.frequency;
            sourceVoltage = params.voltage;
            sourcePhase = params.phase;
        }

        // Добавить узел в общий список
        nodes.push_back(node);
        nameToNode[params.name] = node;
    }
}

bool Circuit::parseFromFile(const string& filename)
{
    // Очистить предыдущие данные
    clearCircuitData();

    vector<string> lines;
    vector<ParamsOfNode> parsedNodes;

    // Загрузить файл
    if (!loadFile(filename, lines))
        return false;

    // Проверить структуру DOT
    if (!validateDotStructure(lines))
        return false;

    // Парсинг графа (узлы и связи)
    if (!parseGraph(lines, parsedNodes))
        return false;

    // Создать узлы цепи из распарсенных данных
    buildNodes(parsedNodes);

    return true;
}

bool Circuit::validate() {
    // Проверить количество элементов
    if (nodes.size() > 100) {
        error.setError(ErrorType::TooManyElements);
        return false;
    }

    // Проверить количество связей
    if (edges.size() > 100) {
        error.setError(ErrorType::TooManyEdges);
        return false;
    }

    // Проверить имена элементов
    if (!validateNames()) {
        return false;
    }

    // Проверить уникальность имён
    if (!validateUniqueness()) {
        return false;
    }

    // Проверить наличие источника ЭДС и корректность значений источника
    if (!validateSource()) {
        return false;
    }

    // Проверить корректность значений R, L, C
    if (!validateValues()) {
        return false;
    }

    // Проверить, что все компоненты из связей существуют, отсутствуют соединений с самим собой и нет дублирующихся связей
    if (!validateEdges()) {
        return false;
    }

    // Проверить связность графа
    if (!validateConnectivity()) {
        return false;
    }

    // Проверить замкнутость цепи и наличие пути от источника ко всем узлам и обратно
    if (!validateClosure()) {
        return false;
    }

    // Проверить количество параллельных элементов
    if (!validateParallelCount()) {
        return false;
    }

    return true;
}

bool Circuit::calculateImpedances() {
    // Вычислить угловую частоту
    double omega = 2.0 * pi * frequency;

    // Для каждого элемента
    for (auto* node : nodes) {
        NodeType type = node->getType();

        // Источник - пропускаем
        if (type == NodeType::Source) {
            continue;
        }
        // Резистор: Z = R + j*0
        if (type == NodeType::Resistor) {
            double resistance = node->getValue();
            node->setResistance(complex<double>(resistance, 0.0));
        }
        // Катушка индуктивности: Z = 0 + j*w*L
        else if (type == NodeType::Coil) {
            double inductance = node->getValue();
            double reactance = omega * inductance;
            node->setResistance(complex<double>(0.0, reactance));
        }
        // Конденсатор: Z = 0 - j*(1/(w*C))
        else if (type == NodeType::Capacitor) {
            double capacitance = node->getValue();

            // Проверка на ноль
            if (capacitance == 0.0) {
                error.setError(ErrorType::ValueOutOfRange, node->getName());
                return false;
            }
            double reactance = 1.0 / (omega * capacitance);
            node->setResistance(complex<double>(0.0, -reactance));
        }
    }
    return true;
}

void Circuit::clearBranches() {
    // Удалить все существующие ветви
    for (auto* branch : branches)
        delete branch;

    branches.clear();

    // Очистить связи узлов перед перестроением
    for (auto* node : nodes)
        node->clearConnections();
}

CircuitNode* Circuit::findSourceNode() {
    // Найти узел с типом Source (источник ЭДС)
    for (auto* node : nodes) {
        if (node->getType() == NodeType::Source) {
            return node;
        }
    }
    return nullptr;
}

void Circuit::buildNodeConnections() {
    // Построить однонаправленные связи между узлами на основе рёбер
    for (const auto& edge : edges) {

        CircuitNode* src =
            nameToNode[edge.first];

        CircuitNode* dst =
            nameToNode[edge.second];

        if (!src || !dst)
            continue;

        src->addNextNodeOneWay(dst);   // Прямая связь
        dst->addPrevNodeOneWay(src);   // Обратная связь
    }
}

vector<CircuitNode*> Circuit::buildTopologicalOrder(CircuitNode* sourceNode) {
    vector<CircuitNode*> topoOrder;           // Результат топологической сортировки
    set<CircuitNode*> visited;                // Посещtнные узлы
    queue<CircuitNode*> q;                    // Очередь для BFS

    q.push(sourceNode);
    visited.insert(sourceNode);

    while (!q.empty()) {
        CircuitNode* node = q.front();
        q.pop();
        topoOrder.push_back(node);

        // Обработать всех потомков текущего узла
        for (auto* next : node->getNextNodes()) {
            if (visited.count(next)) {
                continue;
            }

            // Проверить, что все предки узла уже посещены
            bool allPrevVisited = true;
            for (auto* prev : next->getPrevNodes()) {
                if (!visited.count(prev)) {
                    allPrevVisited = false;
                    break;
                }
            }

            if (allPrevVisited) {
                visited.insert(next);
                q.push(next);
            }
        }

        // Если очередь пуста, но есть еще непосещенные узлы
        if (q.empty()) {
            for (auto* n : nodes) {
                if (visited.count(n)) {
                    continue;
                }

                // Найти узел, все предки которого уже посещены
                bool allPrevVisited = true;
                for (auto* prev : n->getPrevNodes()) {
                    if (!visited.count(prev)) {
                        allPrevVisited = false;
                        break;
                    }
                }

                if (allPrevVisited) {
                    visited.insert(n);
                    q.push(n);
                    break;  // Добавить один узел и продолжить
                }
            }
        }
    }

    return topoOrder;
}

bool Circuit::createBranches(const vector<CircuitNode*>& topoOrder, CircuitNode* sourceNode, map<CircuitNode*, CircuitBranch*>& nodeToBranch) {
    for (auto* node : topoOrder) {
        auto prevNodes = node->getPrevNodes();

        // Узел-источник создает первую ветвь
        if (node == sourceNode) {
            CircuitBranch* branch = new CircuitBranch();
            branch->addNode(node);
            nodeToBranch[node] = branch;
            branches.push_back(branch);
            continue;
        }

        // Определить, нужно ли создавать новую ветвь
        bool startNewBranch = false;

        // Точка слияния
        if (prevNodes.size() > 1) {
            startNewBranch = true;
        }
        // Точка разветвления
        else if (prevNodes.size() == 1) {
            CircuitNode* prev = prevNodes[0];
            if (prev->getNextNodes().size() > 1) {
                startNewBranch = true;
            }
        }

        if (startNewBranch) {
            // Создать новую ветвь
            CircuitBranch* branch = new CircuitBranch();
            branch->addNode(node);
            nodeToBranch[node] = branch;
            branches.push_back(branch);

            // Связать новую ветвь с ветвями предков
            for (auto* prev : prevNodes) {
                CircuitBranch* prevBranch = nodeToBranch[prev];
                if (prevBranch) {
                    prevBranch->addNextBranch(branch);
                }
            }
            continue;
        }

        // Продолжить существующую ветвь
        if (prevNodes.empty()) {
            error.setError(ErrorType::InvalidCircuitTopology);
            return false;
        }

        CircuitNode* prev = prevNodes[0];
        CircuitBranch* prevBranch = nodeToBranch[prev];

        if (prevBranch) {
            prevBranch->addNode(node);
            nodeToBranch[node] = prevBranch;
        }
    }

    return true;
}

bool Circuit::buildBranches() {
    // Очистить старые ветви и связи узлов
    clearBranches();

    // Найти узел-источник
    CircuitNode* sourceNode = findSourceNode();
    if (!sourceNode) {
        error.setError(ErrorType::MissingSource);
        return false;
    }

    // Построить направленные связи между узлами
    buildNodeConnections();

    // Топологическая сортировка узлов
    vector<CircuitNode*> topoOrder = buildTopologicalOrder(sourceNode);

    // Распределить узлы по ветвям
    map<CircuitNode*, CircuitBranch*> nodeToBranch;
    if (!createBranches(topoOrder, sourceNode, nodeToBranch)) {
        return false;
    }

    // Замкнуть цепь (связать конечные ветви с источником)
    CircuitBranch* sourceBranch = nodeToBranch[sourceNode];
    for (auto* node : nodes) {
        for (auto* next : node->getNextNodes()) {
            if (next != sourceNode) {
                continue;
            }

            CircuitBranch* fromBranch = nodeToBranch[node];
            if (!fromBranch || !sourceBranch || fromBranch == sourceBranch) {
                continue;
            }
            // Замкнуть ветвь, возвращающуюся к источнику
            fromBranch->addNextBranch(sourceBranch);
        }
    }

    // Вычислить эквивалентные сопротивления ветвей
    for (auto* branch : branches) {
        branch->calculateEqResistance();
    }

    return true;
}


bool Circuit::calculateCurrents() {
    if (branches.empty()) {
        error.setError(ErrorType::CircuitNotClosed);
        return false;
    }

    // Найти ветвь, содержащую источник ЭДС (главную ветвь)
    CircuitBranch* mainBranch = nullptr;
    for (auto* branch : branches) {
        for (auto* node : branch->getNodes()) {
            if (node->getType() == NodeType::Source) {
                mainBranch = branch;
                break;
            }
        }
        if (mainBranch) {
            break;
        }
    }

    if (!mainBranch) {
        error.setError(ErrorType::MissingSource);
        return false;
    }

    // Сбросить флаги visited у всех ветвей
    for (auto* branch : branches) {
        branch->setVisited(false);
    }

    // Рассчитать эквивалентное сопротивление всей цепи
    complex<double> totalResistance = calcTotalResistance(mainBranch);

    // Если Z_общ близко к нулю (|Z_общ| < 1e-12)
    if (abs(totalResistance) < 1e-12) {
        error.setError(ErrorType::ValueOutOfRange);
        return false;
    }

    // Рассчитать ток главной ветви
    double phaseRad = sourcePhase * pi / 180.0;
    complex<double> sourceVoltageComplex(sourceVoltage * cos(phaseRad), sourceVoltage * sin(phaseRad));

    complex<double> mainCurrent = sourceVoltageComplex / totalResistance;
    // Установить этот ток в главной ветви
    mainBranch->setAmperage(mainCurrent);

    // Сбросить visited снова для распределения токов
    for (auto* branch : branches) {
        branch->setVisited(false);
    }

    // Распределить токи по всем ветвям
    distributeCurrentsToBranches(mainBranch);

    // Записать рассчитанные токи в узлы
    // Для каждой ветви из списка branches
    for (auto* branch : branches) {
        complex<double> branchCurrent = branch->getAmperage();
        // Для каждого узла в ветви
        for (auto* node : branch->getNodes()) {
            // Установить ток узла равным току ветви
            node->setAmperage(branchCurrent);
        }
    }
    return true;
}

bool Circuit::calculate() {
    // Перевести в комплексные сопротивления
    if (!calculateImpedances()) {
        return false;
    }
    // Построить ветви
    if (!buildBranches()) {
        return false;
    }
    // Рассчитать токи
    if (!calculateCurrents()) {
        return false;
    }
    return true;
}

bool Circuit::writeToFile(const string& filename) {
    // Открыть файл
    ofstream outFile(filename);
    if (!outFile.is_open()) {
        error.setError(ErrorType::OutputFileCreateFail, "", 0, filename);
        return false;
    }

    // Записать граф в формате DOT
    outFile << "digraph circuit {" << endl;

    // Определения элементов с токами
    for (auto* node : nodes) {
        outFile << node->getName() << " [label=\"";

        // Тип и параметры
        if (node->getType() == NodeType::Source) {
            outFile << "SOURCE frequency=" << node->getFrequency()
                << " voltage=" << node->getVoltage()
                << " phase=" << node->getPhase();
        }
        else {
            char typeChar = 'R';
            if (node->getType() == NodeType::Coil) {
                typeChar = 'L';
            }
            else if (node->getType() == NodeType::Capacitor) {
                typeChar = 'C';
            }
            outFile << typeChar << "=" << node->getValue();
        }
        // Добавить ток
        complex<double> current = node->getAmperage();
        outFile << " I=complex<double>("
            << fixed << setprecision(6) << current.real()
            << ", " << current.imag() << ") A\"]" << endl;
    }
    outFile << endl;

    // Связи
    for (const auto& edge : edges) {
        outFile << edge.first << " -> " << edge.second << endl;
    }
    // Закрывающая скобка
    outFile << "}" << endl;
    // Закрыть файл
    outFile.close();

    return true;
}

// Вспомогательные методы парсинга

bool Circuit::parseLabel(const string& labelContent, ParamsOfNode& params, int lineNum) {
    if (labelContent.empty()) {
        error.setError(ErrorType::EmptyLabel, params.name, lineNum);
        return false;
    }

    params.originalLabel = labelContent;
    string upperContent = labelContent;
    transform(upperContent.begin(), upperContent.end(), upperContent.begin(), ::toupper);

    // Проверить, является ли элемент источником (SOURCE)
    if (upperContent.find("SOURCE") == 0) {
        if (!parseSourceLabel(labelContent, params)) {
            error.setError(ErrorType::IncompleteSource, params.name, lineNum);
            return false;
        }
    }
    else {
        // Для пассивных элементов R, L, C отдельный парсер
        if (!parseElementLabel(labelContent, params)) {
            size_t eqPos = labelContent.find('=');
            if (eqPos == string::npos) {
                error.setError(ErrorType::MissingComponentValue, params.name, lineNum);
            }
            else {
                string typeStr = trim(labelContent.substr(0, eqPos));
                // Различия неизвестного типа компонента и ошибки формата числа
                if (typeStr == "R" || typeStr == "L" || typeStr == "C" ||
                    typeStr == "r" || typeStr == "l" || typeStr == "c") {
                    error.setError(ErrorType::InvalidNumberFormat, params.name, lineNum);
                }
                else {
                    error.setError(ErrorType::UnknownComponentType, params.name, lineNum);
                }
            }
            return false;
        }
    }
    return true;
}

bool Circuit::isValidName(const string& name) {
    // Имя не должно быть пустым и не длиннее 7 символов
    if (name.empty() || name.length() > 7) {
        return false;
    }
    // Первый символ должен быть буквой
    if (!isalpha((unsigned char)name[0])) {
        return false;
    }
    // Все остальные символы только буквы или цифры
    for (char c : name) {
        if (!isalnum((unsigned char)c)) {
            return false;
        }
    }
    return true;
}

bool Circuit::parseSourceLabel(const string& content, ParamsOfNode& params) {
    // Регулярные выражения для поиска frequency, voltage и phase
    regex freqRegex(R"(frequency\s*=\s*([+-]?\d*\.?\d+(?:[eE][+-]?\d+)?))", regex::icase);
    regex voltRegex(R"(voltage\s*=\s*([+-]?\d*\.?\d+(?:[eE][+-]?\d+)?))", regex::icase);
    regex phaseRegex(R"(phase\s*=\s*([+-]?\d+))", regex::icase);
    smatch match;

    // Извлечь frequency
    if (!regex_search(content, match, freqRegex)) {
        return false;
    }
    if (!parseDouble(match[1].str(), params.frequency)) {
        return false;
    }

    // Извлечь voltage
    if (!regex_search(content, match, voltRegex)) {
        return false;
    }
    if (!parseDouble(match[1].str(), params.voltage)) {
        return false;
    }

    // Извлечь phase
    if (!regex_search(content, match, phaseRegex)) {
        return false;
    }
    double phase;
    if (!parseDouble(match[1].str(), phase)) {
        return false;
    }
    params.phase = phase;

    params.type = NodeType::Source;
    return true;
}

bool Circuit::parseElementLabel(const string& content, ParamsOfNode& params) {
    // Искать знак равенства, разделяющий тип и значение
    size_t eqPos = content.find('=');
    if (eqPos == string::npos) {
        return false;
    }

    // Разделить строку на тип и значение
    string typeStr = trim(content.substr(0, eqPos));
    string valueStr = trim(content.substr(eqPos + 1));

    if (typeStr.empty() || valueStr.empty()) {
        return false;
    }

    // Определить тип компонента по первому символу
    if (typeStr == "R" || typeStr == "r") {
        params.type = NodeType::Resistor;
    }
    else if (typeStr == "L" || typeStr == "l") {
        params.type = NodeType::Coil;
    }
    else if (typeStr == "C" || typeStr == "c") {
        params.type = NodeType::Capacitor;
    }
    else {
        return false;
    }

    // Преобразовать строковое значение в число с плавающей точкой
    return parseDouble(valueStr, params.nominal);
}

bool Circuit::parseDouble(const string& s, double& value) {
    if (s.empty()) {
        return false;
    }
    string str = s;
    // Удалить все пробельные символы
    str.erase(remove_if(str.begin(), str.end(), ::isspace), str.end());

    // Проверить экспоненциальный формат (не более 2 знаков после запятой в мантиссе)
    if (!validateExponentialFormat(str)) {
        return false;
    }

    regex numRegex(R"([+-]?(\d+\.?\d*|\.\d+)([eE][+-]?\d+)?)");
    if (regex_match(str, numRegex)) {
        try {
            istringstream iss(str);
            // Локаль "C" для гарантии, что точка это десятичный разделитель
            iss.imbue(locale("C"));
            if (!(iss >> value)) {
                return false;
            }
            // Проверить, что результат конечное число 
            return isfinite(value);
        }
        catch (...) {
            return false;
        }
    }
    return false;
}

bool Circuit::validateExponentialFormat(const string& s) {
    // Искать экспоненциальную форму
    size_t ePos = s.find_first_of("eE");
    if (ePos == string::npos) {
        return true;
    }
    // Взять мантиссу (часть до e/E)
    string mantissa = s.substr(0, ePos);

    // Искать десятичную точку
    size_t dotPos = mantissa.find('.');
    if (dotPos != string::npos) {
        // Количество знаков после запятой не более двух
        int fractionalDigits = mantissa.length() - dotPos - 1;
        if (fractionalDigits > 2) {
            return false;
        }
    }
    return true;
}

// Вспомогательные методы валидации

bool Circuit::validateNames() {
    // Пройти по всем узлам цепи
    for (auto* node : nodes) {
        // Проверить корректность имени каждого узла
        if (!isValidName(node->getName())) {
            error.setError(ErrorType::InvalidComponentName, node->getName());
            return false;
        }
    }
    return true;
}

bool Circuit::validateUniqueness() {
    // Создать множество для хранения уже встреченных имён
    unordered_set<string> usedNames;

    // Пройти по всем узлам цепи
    for (auto* node : nodes) {
        // Проверить, не встречалось ли имя ранее
        if (usedNames.find(node->getName()) != usedNames.end()) {
            error.setError(ErrorType::DuplicateComponent, node->getName());
            return false;
        }
        // Добавить имя в множество обработанных
        usedNames.insert(node->getName());
    }
    return true;
}

bool Circuit::validateSource() {
    int sourceCount = 0;
    CircuitNode* sourceNode = nullptr;

    // Пройти по всем узлам цепи
    for (auto* node : nodes) {
        // Найти узлы с типом Source
        if (node->getType() == NodeType::Source) {
            sourceCount++;
            sourceNode = node;
        }
    }

    // Проверить наличие источника
    if (sourceCount == 0) {
        error.setError(ErrorType::MissingSource);
        return false;
    }

    // Проверить, что источник только один
    if (sourceCount > 1) {
        error.setError(ErrorType::MultipleSources);
        return false;
    }

    // Проверить корректность частоты (должна быть положительной)
    if (sourceNode->getFrequency() <= 0) {
        error.setError(ErrorType::ValueOutOfRange, sourceNode->getName());
        return false;
    }

    // Проверить корректность напряжения
    if (sourceNode->getVoltage() <= 0) {
        error.setError(ErrorType::ValueOutOfRange, sourceNode->getName());
        return false;
    }

    // Проверить диапазон фазы
    if (sourceNode->getPhase() < 0 || sourceNode->getPhase() > 360) {
        error.setError(ErrorType::PhaseOutOfRange, sourceNode->getName());
        return false;
    }

    return true;
}

bool Circuit::validateValues() {
    // Пройти по всем узлам цепи
    for (auto* node : nodes) {
        // Пропустить источник (у него нет номинального значения)
        if (node->getType() == NodeType::Source) {
            continue;
        }

        // Проверить, что номинальное значение положительное
        if (node->getValue() <= 0) {
            error.setError(ErrorType::ValueOutOfRange, node->getName());
            return false;
        }
    }
    return true;
}

bool Circuit::validateEdges() {
    // Проверить, есть ли хотя бы одна связь в цепи
    if (edges.empty()) {
        error.setError(ErrorType::NoConnections);
        return false;
    }

    // Создать множество для хранения уже обработанных связей
    set<pair<string, string>> usedEdges;

    // Пройти по всем связям (рёбрам) графа
    for (const auto& edge : edges) {

        // Проверить, существует ли узел-источник
        if (nameToNode.find(edge.first) == nameToNode.end()) {
            error.setError(ErrorType::UndefinedComponent, edge.first);
            return false;
        }

        // Проверить, существует ли узел-приёмник
        if (nameToNode.find(edge.second) == nameToNode.end()) {
            error.setError(ErrorType::UndefinedComponent, edge.second);
            return false;
        }

        // Проверить, не соединяет ли элемент сам себя
        if (edge.first == edge.second) {
            error.setError(ErrorType::SelfConnection, edge.first);
            return false;
        }

        // Проверить, нет ли дублирующейся связи
        if (usedEdges.find(edge) != usedEdges.end()) {
            error.setError(ErrorType::DuplicateConnection);
            return false;
        }

        // Добавить текущую связь в множество обработанных
        usedEdges.insert(edge);
    }

    return true;
}

bool Circuit::validateConnectivity() {
    // Подсчитать количество входящих и исходящих связей для каждого узла
    unordered_map<string, int> outCount;
    unordered_map<string, int> inCount;

    // Инициализировать нулевыми значениями
    for (auto* node : nodes) {
        outCount[node->getName()] = 0;
        inCount[node->getName()] = 0;
    }

    // Подсчитать связи из списка рёбер
    for (const auto& edge : edges) {
        outCount[edge.first]++;   // Исходящая связь от источника
        inCount[edge.second]++;   // Входящая связь к приёмнику
    }

    // Проверить, что у каждого узла есть хотя бы одна связь
    for (auto* node : nodes) {
        const string& name = node->getName();
        if (outCount[name] == 0 && inCount[name] == 0) {
            error.setError(ErrorType::IsolatedComponent, name);
            return false;
        }
    }

    return true;
}

bool Circuit::validateClosure() {
    // Проверить отсутствие изолированных узлов
    unordered_map<string, int> outCount;
    unordered_map<string, int> inCount;

    // Инициализировать счетчики
    for (auto* node : nodes) {
        outCount[node->getName()] = 0;
        inCount[node->getName()] = 0;
    }

    // Подсчитать входящие и исходящие связи
    for (const auto& edge : edges) {
        outCount[edge.first]++;
        inCount[edge.second]++;
    }

    // Проверить, что каждый узел имеет хотя бы одну связь
    for (auto* node : nodes) {
        const string& name = node->getName();
        if (outCount[name] == 0 && inCount[name] == 0) {
            error.setError(ErrorType::IsolatedComponent, name);
            return false;
        }
    }

    // Построить граф Boost для BFS обхода
    BoostGraph graph = buildBoostGraph();

    // Найти вершину-источник
    BoostVertex sourceVertex = 0;
    bool found = false;

    auto vertexRange = boost::vertices(graph);
    for (auto it = vertexRange.first; it != vertexRange.second; ++it) {
        if (graph[*it].nodePtr->getType() == NodeType::Source) {
            sourceVertex = *it;
            found = true;
            break;
        }
    }

    // Если источник не найден - ошибка
    if (!found) {
        error.setError(ErrorType::MissingSource);
        return false;
    }

    // Проверить достижимость всех узлов от источника
    if (!checkReachability(graph, sourceVertex)) {
        error.setError(ErrorType::CircuitNotClosed);
        return false;
    }

    // Проверить наличие обратного пути от каждого узла к источнику
    if (!checkReverseReachability(graph, sourceVertex)) {
        error.setError(ErrorType::CircuitNotClosed);
        return false;
    }

    return true;
}

bool Circuit::validateParallelCount() {
    // Группировать рёбра
    map<string, vector<string>> sourceToDest;
    for (const auto& edge : edges) {
        sourceToDest[edge.first].push_back(edge.second);
    }

    // Для каждого источника проверить параллельные группы
    for (auto itSrc = sourceToDest.begin(); itSrc != sourceToDest.end(); ++itSrc) {
        const string& source = itSrc->first;
        const vector<string>& dests = itSrc->second;

        // Группировать dest по их предкам и потомкам
        map<vector<string>, vector<string>> parallelGroups;

        for (size_t i = 0; i < dests.size(); ++i) {
            const string& dest = dests[i];

            // Найти потомков этого dest
            vector<string> nextNodes;
            auto itNext = sourceToDest.find(dest);
            if (itNext != sourceToDest.end()) {
                nextNodes = itNext->second;
                sort(nextNodes.begin(), nextNodes.end());
            }

            vector<string> key;
            key.push_back(source);
            for (const auto& nn : nextNodes) {
                key.push_back(nn);
            }
            sort(key.begin(), key.end());

            // Добавить в группу
            parallelGroups[key].push_back(dest);
        }

        // Проверить размер каждой группы
        for (auto itGroup = parallelGroups.begin(); itGroup != parallelGroups.end(); ++itGroup) {
            if (itGroup->second.size() > 10) {
                error.setError(ErrorType::TooManyParallelElements);
                return false;
            }
        }
    }

    return true;
}

bool Circuit::checkReachability(const BoostGraph& graph, BoostVertex startVertex) {
    size_t numVertices = boost::num_vertices(graph);
    vector<bool> visited(numVertices, false);
    BFSReachabilityVisitor visitor(visited);

    // Выполнить обход графа в ширину, начиная с указанной вершины
    try {
        boost::breadth_first_search(graph, startVertex, boost::visitor(visitor));
    }
    catch (...) {
        return false;
    }

    // Пройти по всем вершинам графа
    for (size_t i = 0; i < numVertices; ++i) {
        // Если хотя бы одна вершина не была посещена — граф несвязный
        if (!visited[i]) return false;
    }
    return true;
}

bool Circuit::checkReverseReachability(const BoostGraph& graph, BoostVertex startVertex) {
    size_t numVertices = boost::num_vertices(graph);

    // Пройти по всем вершинам графа 
    for (size_t i = 0; i < numVertices; ++i) {
        // Пропустить стартовую вершину (источник)
        if (i == startVertex) {
            continue;
        }
        // Создать массив флагов посещения для текущего обхода
        vector<bool> visited(numVertices, false);
        BFSReachabilityVisitor visitor(visited);

        // Выполнить обход графа в ширину
        try {
            boost::breadth_first_search(graph, i, boost::visitor(visitor));
        }
        catch (const std::exception&) {
            return false;
        }

        // Проверить, достижима ли вершина источника от текущей вершины
        if (!visited[startVertex]) {
            // Если от какой-либо вершины нельзя добраться до источника — цепь не замкнута
            error.setError(ErrorType::CircuitNotClosed);
            return false;
        }
    }
    return true;
}

BoostGraph Circuit::buildBoostGraph() {
    BoostGraph graph;
    unordered_map<string, BoostVertex> vertexMap;

    // Пройти по всем узлам электрической цепи
    for (size_t i = 0; i < nodes.size(); ++i) {
        // Заполнить свойства вершины
        VertexProperties vp;
        vp.name = nodes[i]->getName();
        vp.nodePtr = nodes[i];

        BoostVertex v = boost::add_vertex(vp, graph);
        vertexMap[nodes[i]->getName()] = v;
        nameToIndex[nodes[i]->getName()] = i;
    }

    // Пройти по всем связям (рёбрам) электрической цепи
    for (const auto& edge : edges) {
        // Найти дескрипторы вершин для начала и конца связи
        auto srcIt = vertexMap.find(edge.first);
        auto dstIt = vertexMap.find(edge.second);

        // Проверить, что обе вершины существуют в отображении
        if (srcIt != vertexMap.end() && dstIt != vertexMap.end()) {
            // Заполнить свойства ребра
            EdgeProperties ep;
            ep.from = edge.first;
            ep.to = edge.second;

            // Добавить ребро в граф Boost
            boost::add_edge(srcIt->second, dstIt->second, ep, graph);
        }
    }
    return graph;
}

// Вспомогательные методы расчета

void Circuit::distributeCurrentsToBranches(CircuitBranch* branch) {
    // Проверить корректность ветви
    if (!branch) return;
    if (branch->isVisited()) return;

    // Отметить ветвь как посещённую
    branch->setVisited(true);

    // Получить список следующих ветвей
    auto nextBranches = branch->getNextBranches();
    if (nextBranches.empty()) return;

    // Получить входящий ток текущей ветви
    complex<double> incomingCurrent = branch->getAmperage();

    // Одна следующая ветвь — последовательное соединение
    if (nextBranches.size() == 1) {
        CircuitBranch* nextBranch = nextBranches[0];
        if (!nextBranch->isVisited()) {
            // Передать весь ток в следующую ветвь
            nextBranch->setAmperage(incomingCurrent);
            // Рекурсивно продолжить распределение
            distributeCurrentsToBranches(nextBranch);
        }
        return;
    }

    // Несколько ветвей — параллельное соединение
    // Отфильтровать только непосещённые ветви
    vector<CircuitBranch*> unvisitedBranches;
    for (auto* nb : nextBranches) {
        if (!nb->isVisited()) {
            unvisitedBranches.push_back(nb);
        }
    }

    if (unvisitedBranches.empty()) return;

    // Если после фильтрации осталась одна ветвь
    if (unvisitedBranches.size() == 1) {
        unvisitedBranches[0]->setAmperage(incomingCurrent);
        distributeCurrentsToBranches(unvisitedBranches[0]);
        return;
    }

    // Вычислить сумму проводимостей параллельных ветвей
    complex<double> sumAdmittance(0.0, 0.0);
    const double EPS = 1e-12;

    for (auto* nb : unvisitedBranches) {
        complex<double> impedance = nb->getEqResistance();
        if (abs(impedance) > EPS) {
            sumAdmittance += complex<double>(1.0, 0.0) / impedance;
        }
    }

    if (abs(sumAdmittance) < EPS) return;

    // Распределить ток пропорционально проводимостям
    for (auto* nb : unvisitedBranches) {
        complex<double> impedance = nb->getEqResistance();
        if (abs(impedance) > EPS) {
            complex<double> admittance = complex<double>(1.0, 0.0) / impedance;
            complex<double> branchCurrent = incomingCurrent * admittance / sumAdmittance;

            // Установить рассчитанный ток
            nb->setAmperage(branchCurrent);

            // Рекурсивно распределить ток дальше
            distributeCurrentsToBranches(nb);
        }
    }
}

complex<double> Circuit::calcTotalResistance(CircuitBranch* branch) {
    // Проверить корректность ветви
    if (!branch) {
        return complex<double>(0.0, 0.0);
    }

    // Проверить, не посещали ли уже эту ветвь
    if (branch->isVisited()) {
        return complex<double>(0.0, 0.0);
    }

    branch->setVisited(true);

    // Получить сопротивление текущей ветви
    complex<double> myResistance = branch->getEqResistance();

    // Получить список следующих ветвей
    auto nextBranches = branch->getNextBranches();

    // Если нет следующих ветвей — вернуть своё сопротивление
    if (nextBranches.empty()) {
        return myResistance;
    }

    // Отфильтровать только непосещённые ветви
    vector<CircuitBranch*> unvisitedBranches;
    for (auto* nb : nextBranches) {
        if (!nb->isVisited()) {
            unvisitedBranches.push_back(nb);
        }
    }

    // Если все следующие ветви уже посещены
    if (unvisitedBranches.empty()) {
        return myResistance;
    }

    // Одна следующая ветвь — последовательное соединение
    if (unvisitedBranches.size() == 1) {
        return myResistance + calcTotalResistance(unvisitedBranches[0]);
    }

    // Если несколько ветвей то параллельное соединение
    // Вычислить сумму проводимостей (1/Z) для всех параллельных ветвей
    complex<double> sumAdmittance(0.0, 0.0);
    const double EPS = 1e-12;

    for (auto* nb : unvisitedBranches) {
        complex<double> childResistance = calcTotalResistance(nb);
        if (abs(childResistance) > EPS) {
            sumAdmittance += complex<double>(1.0, 0.0) / childResistance;
        }
    }

    // Вычислить эквивалентное сопротивление параллельного участка
    complex<double> parallelResistance(0.0, 0.0);
    if (abs(sumAdmittance) > EPS) {
        parallelResistance = complex<double>(1.0, 0.0) / sumAdmittance;
    }

    // Вернуть общее сопротивление
    return myResistance + parallelResistance;
}