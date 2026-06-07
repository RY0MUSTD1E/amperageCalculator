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

bool Circuit::parseFromFile(const string& filename) {

    // Очистить все предыдущие данные
    for (auto* node : nodes) delete node;
    nodes.clear();
    for (auto* branch : branches) delete branch;
    branches.clear();
    edges.clear();
    nameToNode.clear();
    nameToIndex.clear();
    nodeDefinitions.clear();
    frequency = 0.0;
    sourceVoltage = 0.0;
    sourcePhase = 0.0;
    error.clear();

    // Открыть файл filename
    ifstream file(filename);

    // Если файл не существует или не удалось открыть
    if (!file.is_open()) {
        error.setError(ErrorType::InputFileNotExist, filename);
        return false;
    }

    vector<string> lines;
    string line;
    bool hasContent = false;

    while (getline(file, line)) {
        string trimmed = trim(line);
        if (!trimmed.empty()) {
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

    // Проверить наличие ключевого слова "digraph" в начале файла
    if (lines.empty() || trim(lines[0]).find("digraph") != 0) {
        error.setError(ErrorType::MissingDigraph);
        return false;
    }

    // Проверить наличие открывающей и закрывающей фигурной скобки
    bool hasOpenBrace = false, hasCloseBrace = false;
    for (const auto& l : lines) {
        if (l.find('{') != string::npos) {
            hasOpenBrace = true;
        }
        if (l.find('}') != string::npos) {
            hasCloseBrace = true;
        }
    }

    // Если отсутствует открывающая скобка
    if (!hasOpenBrace) {
        error.setError(ErrorType::MissingOpenBrace);
        return false;
    }
    // Если отсутствует закрырывающая скобка
    if (!hasCloseBrace) {
        error.setError(ErrorType::MissingCloseBrace);
        return false;
    }

    // Выполнить парсинг DOT-формата
    vector<ParamsOfNode> parsedNodes;

    set<string> parsedNames;
    regex nodeRegex(R"(\s*(\w+)\s*\[label\s*=\s*\"([^\"]*)\"\s*\])");
    regex edgeRegex(R"(\s*(\w+)\s*->\s*(\w+)\s*)");

    int lineNum = 0;
    bool insideGraph = false;

    for (const auto& rawLine : lines) {
        lineNum++;
        string s = trim(rawLine);

        if (s.empty() || s[0] == '#') {
            continue;
        }

        if (s.find('{') != string::npos) {
            insideGraph = true;
            if (s == "{" || s.find("digraph") != string::npos) {
                continue;
            }
        }
        if (s == "}") {
            insideGraph = false;
            continue;
        }

        if (!insideGraph) {
            continue;
        }

        smatch match;

        // Обойти все рёбра графа
        if (s.find("->") != string::npos) {
            if (regex_match(s, match, edgeRegex)) {
                // Извлечь имена начальной и конечной вершин
                string src = match[1].str();
                string dst = match[2].str();
                if (src.empty() || dst.empty()) {
                    error.setError(ErrorType::InvalidConnectionSyntax, "", lineNum);
                    return false;
                }
                if (src == dst) {
                    error.setError(ErrorType::SelfConnection, src, lineNum);
                    return false;
                }
                // Добавить пару (src, dst) в список edges
                edges.emplace_back(src, dst);
            }
            else {
                error.setError(ErrorType::InvalidConnectionSyntax, "", lineNum);
                return false;
            }
            continue;
        }

        // Обойти все вершины графа
        if (regex_match(s, match, nodeRegex)) {
            // Извлечь имя вершины и атрибут label
            string nodeName = match[1].str();
            string labelContent = match[2].str();

            if (!isValidName(nodeName)) {
                error.setError(ErrorType::InvalidComponentName, nodeName, lineNum);
                return false;
            }

            if (parsedNames.find(nodeName) != parsedNames.end()) {
                error.setError(ErrorType::DuplicateComponent, nodeName, lineNum);
                return false;
            }

            parsedNames.insert(nodeName);

            if (labelContent.empty()) {
                error.setError(ErrorType::EmptyLabel, nodeName, lineNum);
                return false;
            }

            ParamsOfNode params;
            params.name = nodeName;

            // На основе label определить тип элемента (SOURCE, R, L, C) и извлечь числовые параметры
            if (!parseLabel(labelContent, params, lineNum)) {
                return false;
            }

            // Сохранить полученные данные во временные структуры ParamsOfNode
            parsedNodes.push_back(params);
            nodeDefinitions.push_back(rawLine);
            continue;
        }

        if (!s.empty() && s != "{") {
            error.setError(ErrorType::InvalidCircuitTopology, "", lineNum);
            return false;
        }
    }

    // На основе собранных данных заполнить поля объекта Circuit
    for (const auto& params : parsedNodes) {
        // Создать узлы CircuitNode из ParamsOfNode
        CircuitNode* node = new CircuitNode(params.name, params.type, params.nominal);
        node->setOriginalLabel(params.originalLabel);

        if (params.type == NodeType::Source) {
            node->setVoltage(params.voltage);
            node->setFrequency(params.frequency);
            node->setPhase(params.phase);

            // Сохранить частоту, напряжение и фазу источника в соответствующие поля
            frequency = params.frequency;
            sourceVoltage = params.voltage;
            sourcePhase = params.phase;
        }

        // Сохранить узлы в список nodes
        nodes.push_back(node);
        nameToNode[params.name] = node;
    }

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
    return false;
}

bool Circuit::buildBranches() {
    return false;
}

bool Circuit::calculateCurrents() {
    return false;
}

bool Circuit::calculate() {
    return false;
}

bool Circuit::writeToFile(const string& filename) {
    return false;
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
    return true;
}

bool Circuit::validateConnectivity() {
    return true;
}

bool Circuit::validateClosure() {
    return true;
}

bool Circuit::validateParallelCount() {
    return true;
}