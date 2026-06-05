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

std::complex<double> CircuitNode::getResistance() const {
    return resistance;
}