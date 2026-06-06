/**
 * \file testHelpers.cpp
 * \brief Вспомогательные функции и подключения для всех тестов amperageCalculator.
 * 
 * \author Anna Bezhenar
 * \date June 2026
 */

#include "pch.h"
#include "TestHelpers.h"
#include "CppUnitTest.h"
#include <fstream>

using namespace Microsoft::VisualStudio::CppUnitTestFramework;
using namespace std;

namespace TestHelpers
{
    Circuit* createCircuitFromString(const string& circuitStr) {
        string tempFile = "temp_test_input.txt";
        ofstream out(tempFile);
        out << circuitStr;
        out.close();
        Circuit* circuit = new Circuit();
        circuit->parseFromFile(tempFile);
        remove(tempFile.c_str());

        return circuit;
    }

    void VerifyBranchNodes(CircuitBranch* branch, const vector<string>& expectedNames) {
        const auto& nodes = branch->getNodes();
        Assert::AreEqual(expectedNames.size(), nodes.size(), L"Node count mismatch");
        for (size_t i = 0; i < expectedNames.size(); i++) {
            string expected = expectedNames[i];
            string actual = nodes[i]->getName();
            wstring wExpected(expected.begin(), expected.end());
            wstring wActual(actual.begin(), actual.end());
            Assert::AreEqual(wExpected, wActual, L"Node name mismatch");
        }
    }

    void VerifyConnections(CircuitBranch* branch, const vector<int>& expectedNext, const vector<int>& expectedPrev, const vector<CircuitBranch*>& allBranches) {
        const auto& nextBranches = branch->getNextBranches();
        Assert::AreEqual(expectedNext.size(), nextBranches.size(), L"Next branches count mismatch");
        for (int idx : expectedNext) {
            bool found = false;
            for (auto* nb : nextBranches) {
                if (nb == allBranches[idx]) {
                    found = true;
                    break;
                }
            }
            Assert::IsTrue(found, L"Expected next branch not found");
        }

        const auto& prevBranches = branch->getPrevBranches();
        Assert::AreEqual(expectedPrev.size(), prevBranches.size(), L"Prev branches count mismatch");
        for (int idx : expectedPrev) {
            bool found = false;
            for (auto* pb : prevBranches) {
                if (pb == allBranches[idx]) {
                    found = true;
                    break;
                }
            }
            Assert::IsTrue(found, L"Expected prev branch not found");
        }
    }

    void VerifyAmperage(Circuit* circuit, const string& nodeName, double expectedReal, double expectedImag, double tolerance) {
        CircuitNode* node = circuit->getNodeByName(nodeName);
        Assert::IsNotNull(node, (wstring(L"Node ") + wstring(nodeName.begin(), nodeName.end()) + L" not found").c_str());

        complex<double> amperage = node->getAmperage();
        complex<double> expected(expectedReal, expectedImag);

        wstring message = L"Amperage mismatch for " + wstring(nodeName.begin(), nodeName.end());
        Assert::IsTrue(abs(amperage - expected) < tolerance, message.c_str());
    }

}