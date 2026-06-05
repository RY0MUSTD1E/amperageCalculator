/**
 * \file testHelpers.cpp
 * \brief Вспомогательные функции и подключения для всех тестов amperageCalculator.
 * 
 * \author Anna Bezhenar
 * \date June 2026
 */

#include "pch.h"
#include "CppUnitTest.h"
#include "TestHelpers.h"
#include "../amperageCalculator/circuit.h"
#include <string>

using namespace std;
using namespace Microsoft::VisualStudio::CppUnitTestFramework;
namespace TestHelpers
{
    Circuit* createCircuitFromString(const string& circuitStr) {
        Circuit* circuit = new Circuit();
        return circuit;
    }

    void VerifyBranchNodes(CircuitBranch* branch, const vector<string>& expectedNames) {
    }

    void VerifyConnections(CircuitBranch* branch, const vector<int>& expectedNext, const vector<int>& expectedPrev, const vector<CircuitBranch*>& allBranches) {
        
    }
}