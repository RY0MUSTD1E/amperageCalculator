/**
 * \file circuitBranch.cpp
 * \brief Реализация класса CircuitBranch.
 *
 * \author Anna Bezhenar
 * \date June 2026
 */

#include "circuitBranch.h"

CircuitBranch::CircuitBranch()
    : amperage(0.0, 0.0)
    , eqResistance(0.0, 0.0)
    , visited(false)
{
}

CircuitBranch::~CircuitBranch()
{
    nodes.clear();
    previousBranches.clear();
    nextBranches.clear();
}

const std::vector<CircuitNode*>& CircuitBranch::getNodes() const
{
    return nodes;
}

const std::vector<CircuitBranch*>& CircuitBranch::getPrevBranches() const
{
    return previousBranches;
}

const std::vector<CircuitBranch*>& CircuitBranch::getNextBranches() const
{
    return nextBranches;
}