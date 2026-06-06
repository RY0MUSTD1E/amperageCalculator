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


void CircuitBranch::setAmperage(std::complex<double> a) {
    amperage = a;
}

std::complex<double> CircuitBranch::getAmperage() const {
    return amperage;
}

void CircuitBranch::setEqResistance(std::complex<double> r) {
    eqResistance = r;
}

std::complex<double> CircuitBranch::getEqResistance() const {
    return eqResistance;
}

void CircuitBranch::calculateEqResistance() {
    std::complex<double> total(0.0, 0.0);
    for (const auto* node : nodes) {
        if (node->getType() != NodeType::Source) {
            total += node->getResistance();
        }
    }
    eqResistance = total;
}

void CircuitBranch::addNode(CircuitNode* node) {
    if (node) {
        nodes.push_back(node);
    }
}

const std::vector<CircuitNode*>& CircuitBranch::getNodes() const {
    return nodes;
}

void CircuitBranch::setNodes(const std::vector<CircuitNode*>& n) {
    nodes = n;
}

bool CircuitBranch::isVisited() const {
    return visited;
}

void CircuitBranch::setVisited(bool v) {
    visited = v;
}

void CircuitBranch::addPrevBranch(CircuitBranch* branch) {
    if (branch && std::find(previousBranches.begin(), previousBranches.end(), branch) == previousBranches.end()) {
        previousBranches.push_back(branch);
        if (std::find(branch->nextBranches.begin(), branch->nextBranches.end(), this) == branch->nextBranches.end()) {
            branch->nextBranches.push_back(this);
        }
    }
}

void CircuitBranch::addNextBranch(CircuitBranch* branch) {
    if (branch && std::find(nextBranches.begin(), nextBranches.end(), branch) == nextBranches.end()) {
        nextBranches.push_back(branch);
        if (std::find(branch->previousBranches.begin(), branch->previousBranches.end(), this) == branch->previousBranches.end()) {
            branch->previousBranches.push_back(this);
        }
    }
}

void CircuitBranch::removePrevBranch(CircuitBranch* branch) {
    previousBranches.erase(
        std::remove(previousBranches.begin(), previousBranches.end(), branch),
        previousBranches.end()
    );
    auto& nexts = branch->nextBranches;
    nexts.erase(
        std::remove(nexts.begin(), nexts.end(), this),
        nexts.end()
    );
}

void CircuitBranch::removeNextBranch(CircuitBranch* branch) {
    nextBranches.erase(
        std::remove(nextBranches.begin(), nextBranches.end(), branch),
        nextBranches.end()
    );
    auto& prevs = branch->previousBranches;
    prevs.erase(
        std::remove(prevs.begin(), prevs.end(), this),
        prevs.end()
    );
}

const std::vector<CircuitBranch*>& CircuitBranch::getPrevBranches() const {
    return previousBranches;
}

const std::vector<CircuitBranch*>& CircuitBranch::getNextBranches() const {
    return nextBranches;
}

void CircuitBranch::setPrevBranches(const std::vector<CircuitBranch*>& branches) {
    previousBranches = branches;
}

void CircuitBranch::setNextBranches(const std::vector<CircuitBranch*>& branches) {
    nextBranches = branches;
}