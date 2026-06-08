/**
 * \file circuitNode.cpp
 * \brief Реализация класса CircuitNode.
 *
 * \author Anna Bezhenar
 * \date June 2026
 */

#include "circuitNode.h"

CircuitNode::CircuitNode()
    : type(NodeType::Resistor)
    , amperage(0.0, 0.0)
    , resistance(0.0, 0.0)
    , voltage(0.0)
    , frequency(0.0)
    , phase(0.0)
    , value(0.0)
{
}

CircuitNode::CircuitNode(const std::string& name, NodeType type, double value)
    : name(name)
    , type(type)
    , amperage(0.0, 0.0)
    , resistance(0.0, 0.0)
    , voltage(0.0)
    , frequency(0.0)
    , phase(0.0)
    , value(value)
{
    if (type == NodeType::Resistor) {
        resistance = std::complex<double>(value, 0.0);
    }
}

// Сеттеры

void CircuitNode::setName(const std::string& n) {
    name = n;
}

void CircuitNode::setType(NodeType t) {
    type = t;
}

void CircuitNode::setAmperage(std::complex<double> a) {
    amperage = a;
}

void CircuitNode::setResistance(std::complex<double> r) {
    resistance = r;
}

void CircuitNode::setVoltage(double v) {
    voltage = v;
}

void CircuitNode::setFrequency(double f) {
    frequency = f;
}

void CircuitNode::setPhase(double p) {
    phase = p;
}

void CircuitNode::setValue(double v) {
    value = v;
}

void CircuitNode::setOriginalLabel(const std::string& l) {
    originalLabel = l;
}

// Геттеры

std::string CircuitNode::getName() const {
    return name;
}

NodeType CircuitNode::getType() const {
    return type;
}

std::complex<double> CircuitNode::getAmperage() const {
    return amperage;
}

std::complex<double> CircuitNode::getResistance() const {
    return resistance;
}

double CircuitNode::getVoltage() const {
    return voltage;
}

double CircuitNode::getFrequency() const {
    return frequency;
}

double CircuitNode::getPhase() const {
    return phase;
}

double CircuitNode::getValue() const {
    return value;
}

std::string CircuitNode::getOriginalLabel() const {
    return originalLabel;
}

// Связи
void CircuitNode::addNextNode(CircuitNode* node) {
    if (node && std::find(nextNodes.begin(), nextNodes.end(), node) == nextNodes.end()) {
        nextNodes.push_back(node);
        node->addPrevNode(this);
    }
}

void CircuitNode::addPrevNode(CircuitNode* node) {
    if (node && std::find(previousNodes.begin(), previousNodes.end(), node) == previousNodes.end()) {
        previousNodes.push_back(node);
    }
}

void CircuitNode::addNextNodeOneWay(CircuitNode* node) {
    if (node && std::find(nextNodes.begin(), nextNodes.end(), node) == nextNodes.end()) {
        nextNodes.push_back(node);
    }
}

void CircuitNode::addPrevNodeOneWay(CircuitNode* node) {
    if (node && std::find(previousNodes.begin(), previousNodes.end(), node) == previousNodes.end()) {
        previousNodes.push_back(node);
    }
}

std::vector<CircuitNode*> CircuitNode::getNextNodes() const {
    return nextNodes;
}

std::vector<CircuitNode*> CircuitNode::getPrevNodes() const {
    return previousNodes;
}

void CircuitNode::clearConnections() {
    nextNodes.clear();
    previousNodes.clear();
}