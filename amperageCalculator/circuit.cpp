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
    return "";
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
    return false; 
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


bool Circuit::isValidName(const string& name) {
    return true;
}


bool Circuit::parseLabel(const string& labelContent, ParamsOfNode& params, int lineNum) {
    return true;
}
