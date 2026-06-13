/**
 * \file buildBranchesTests.cpp
 * \brief Модульные тесты для проверки построения ветвей электрической цепи.
 *
 * Данный файл содержит набор тестов, реализованных с использованием Microsoft CppUnitTestFramework.
 * Тестируется корректность разбиения цепи на ветви, связи между ветвями и распределение узлов по ветвям.
 *
 * \author Anna Bezhenar
 * \date June 2026
 */

#include "pch.h"
#include "CppUnitTest.h"
#include "testHelpers.h"
#include "../amperageCalculator/amperageCalculator.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;
using namespace std;
using namespace TestHelpers;

namespace CircuitTests
{
    TEST_CLASS(BuildBranchesTests)
    {
    public:

        TEST_METHOD(NoPrevNodes)
        {
            string circuitStr =
                "digraph circuit {\n"
                "SOURCE [label=\"SOURCE frequency=50 voltage=100 phase=0\"]\n"
                "R1 [label=\"R=10\"]\n"
                "R2 [label=\"R=20\"]\n"
                "SOURCE -> R1\n"
                "R2 -> SOURCE\n"
                "}\n";
            Circuit* circuit = createCircuitFromString(circuitStr);
            circuit->calculateImpedances();
            bool result = circuit->buildBranches();
            Assert::IsFalse(result);

            delete circuit;
        }
        TEST_METHOD(MissingSource)
        {
            string circuitStr =
                "digraph circuit {\n"
                "R1 [label=\"R=10\"]\n"
                "R2 [label=\"R=20\"]\n"
                "R1 -> R2\n"
                "R2 -> R1\n"
                "}\n";
            Circuit* circuit = createCircuitFromString(circuitStr);
            bool result = circuit->buildBranches();
            Assert::IsFalse(result);

            delete circuit;
        }
        TEST_METHOD(SeriesConnection)
        {
            string circuitStr =
                "digraph circuit {\n"
                "SOURCE [label=\"SOURCE frequency=50 voltage=100 phase=0\"]\n"
                "R1 [label=\"R=10\"]\n"
                "R2 [label=\"R=20\"]\n"
                "R3 [label=\"R=30\"]\n"
                "SOURCE -> R1\n"
                "R1 -> R2\n"
                "R2 -> R3\n"
                "R3 -> SOURCE\n"
                "}\n";
            Circuit* circuit = createCircuitFromString(circuitStr);
            circuit->validate();
            circuit->calculateImpedances();
            bool result = circuit->buildBranches();

            Assert::IsTrue(result);
            const auto& branches = circuit->getBranches();
            Assert::AreEqual(size_t(1), branches.size());
            VerifyBranchNodes(branches[0], { "SOURCE", "R1", "R2", "R3" });
            Assert::AreEqual(size_t(0), branches[0]->getNextBranches().size());
            Assert::AreEqual(size_t(0), branches[0]->getPrevBranches().size());
            delete circuit;
        }

        TEST_METHOD(ParallelFromSource)
        {
            string circuitStr =
                "digraph circuit {\n"
                "SOURCE [label=\"SOURCE frequency=50 voltage=100 phase=0\"]\n"
                "R1 [label=\"R=10\"]\n"
                "R2 [label=\"R=20\"]\n"
                "R3 [label=\"R=30\"]\n"
                "SOURCE -> R1\n"
                "SOURCE -> R2\n"
                "SOURCE -> R3\n"
                "R1 -> SOURCE\n"
                "R2 -> SOURCE\n"
                "R3 -> SOURCE\n"
                "}\n";
            Circuit* circuit = createCircuitFromString(circuitStr);
            circuit->validate();
            circuit->calculateImpedances();
            bool result = circuit->buildBranches();

            Assert::IsTrue(result);
            const auto& branches = circuit->getBranches();
            Assert::AreEqual(size_t(4), branches.size());

            int srcIdx = -1, r1Idx = -1, r2Idx = -1, r3Idx = -1;
            for (size_t i = 0; i < branches.size(); i++) {
                const auto& nodes = branches[i]->getNodes();
                if (nodes.size() == 1 && nodes[0]->getName() == "SOURCE") srcIdx = (int)i;
                else if (nodes.size() == 1 && nodes[0]->getName() == "R1") r1Idx = (int)i;
                else if (nodes.size() == 1 && nodes[0]->getName() == "R2") r2Idx = (int)i;
                else if (nodes.size() == 1 && nodes[0]->getName() == "R3") r3Idx = (int)i;
            }
            Assert::IsTrue(srcIdx >= 0 && r1Idx >= 0 && r2Idx >= 0 && r3Idx >= 0);

            VerifyBranchNodes(branches[srcIdx], { "SOURCE" });
            VerifyBranchNodes(branches[r1Idx], { "R1" });
            VerifyBranchNodes(branches[r2Idx], { "R2" });
            VerifyBranchNodes(branches[r3Idx], { "R3" });

            VerifyConnections(branches[srcIdx], { r1Idx, r2Idx, r3Idx }, { r1Idx, r2Idx, r3Idx }, branches);
            VerifyConnections(branches[r1Idx], { srcIdx }, { srcIdx }, branches);
            VerifyConnections(branches[r2Idx], { srcIdx }, { srcIdx }, branches);
            VerifyConnections(branches[r3Idx], { srcIdx }, { srcIdx }, branches);
            delete circuit;
        }

        TEST_METHOD(BranchAndMerge)
        {
            string circuitStr =
                "digraph circuit {\n"
                "SOURCE [label=\"SOURCE frequency=50 voltage=100 phase=0\"]\n"
                "R1 [label=\"R=10\"]\n"
                "R2 [label=\"R=20\"]\n"
                "R3 [label=\"R=30\"]\n"
                "R4 [label=\"R=40\"]\n"
                "SOURCE -> R1\n"
                "R1 -> R2\n"
                "R1 -> R3\n"
                "R2 -> R4\n"
                "R3 -> R4\n"
                "R4 -> SOURCE\n"
                "}\n";
            Circuit* circuit = createCircuitFromString(circuitStr);
            circuit->validate();
            circuit->calculateImpedances();
            bool result = circuit->buildBranches();

            Assert::IsTrue(result);
            const auto& branches = circuit->getBranches();
            Assert::AreEqual(size_t(4), branches.size());

            int br0 = -1, br1 = -1, br2 = -1, br3 = -1;
            for (size_t i = 0; i < branches.size(); i++) {
                const auto& nodes = branches[i]->getNodes();
                if (nodes.size() == 2 && nodes[0]->getName() == "SOURCE" && nodes[1]->getName() == "R1") br0 = (int)i;
                else if (nodes.size() == 1 && nodes[0]->getName() == "R2") br1 = (int)i;
                else if (nodes.size() == 1 && nodes[0]->getName() == "R3") br2 = (int)i;
                else if (nodes.size() == 1 && nodes[0]->getName() == "R4") br3 = (int)i;
            }
            Assert::IsTrue(br0 >= 0 && br1 >= 0 && br2 >= 0 && br3 >= 0);

            VerifyBranchNodes(branches[br0], { "SOURCE", "R1" });
            VerifyBranchNodes(branches[br1], { "R2" });
            VerifyBranchNodes(branches[br2], { "R3" });
            VerifyBranchNodes(branches[br3], { "R4" });

            VerifyConnections(branches[br0], { br1, br2 }, { br3 }, branches);
            VerifyConnections(branches[br1], { br3 }, { br0 }, branches);
            VerifyConnections(branches[br2], { br3 }, { br0 }, branches);
            VerifyConnections(branches[br3], { br0 }, { br1, br2 }, branches);
            delete circuit;
        }

        TEST_METHOD(LongSeriesChain)
        {
            stringstream ss;
            ss << "digraph circuit {\n";
            ss << "SOURCE [label=\"SOURCE frequency=50 voltage=100 phase=0\"]\n";
            vector<int> values = { 10, 15, 22, 33, 47, 56, 68, 82 };
            for (size_t i = 0; i < values.size(); i++) {
                ss << "R" << (i + 1) << " [label=\"R" << "=" << values[i] << "\"]\n";
            }
            ss << "SOURCE -> R1\n";
            for (size_t i = 1; i < values.size(); i++) {
                ss << "R" << i << " -> R" << (i + 1) << "\n";
            }
            ss << "R8 -> SOURCE\n";
            ss << "}\n";

            Circuit* circuit = createCircuitFromString(ss.str());
            circuit->validate();
            circuit->calculateImpedances();
            bool result = circuit->buildBranches();

            Assert::IsTrue(result);
            const auto& branches = circuit->getBranches();
            Assert::AreEqual(size_t(1), branches.size());
            Assert::AreEqual(size_t(9), branches[0]->getNodes().size());
            delete circuit;
        }

        TEST_METHOD(TenParallelBranches)
        {
            stringstream ss;
            ss << "digraph circuit {\n";
            ss << "SOURCE [label=\"SOURCE frequency=50 voltage=100 phase=0\"]\n";
            for (int i = 1; i <= 10; i++) {
                ss << "R" << i << " [label=\"R=" << (10 * i) << "\"]\n";
            }
            for (int i = 1; i <= 10; i++) {
                ss << "SOURCE -> R" << i << "\n";
            }
            for (int i = 1; i <= 10; i++) {
                ss << "R" << i << " -> SOURCE\n";
            }
            ss << "}\n";

            Circuit* circuit = createCircuitFromString(ss.str());
            circuit->validate();
            circuit->calculateImpedances();
            bool result = circuit->buildBranches();

            Assert::IsTrue(result);
            const auto& branches = circuit->getBranches();
            Assert::AreEqual(size_t(11), branches.size());
            delete circuit;
        }

        TEST_METHOD(LongLinkInParallelBranch)
        {
            string circuitStr =
                "digraph circuit {\n"
                "SOURCE [label=\"SOURCE frequency=50 voltage=100 phase=0\"]\n"
                "R1 [label=\"R=10\"]\n"
                "R2 [label=\"R=20\"]\n"
                "R3 [label=\"R=30\"]\n"
                "R4 [label=\"R=40\"]\n"
                "R5 [label=\"R=50\"]\n"
                "SOURCE -> R1\n"
                "SOURCE -> R2\n"
                "R1 -> R3\n"
                "R3 -> R4\n"
                "R4 -> SOURCE\n"
                "R2 -> R5\n"
                "R5 -> SOURCE\n"
                "}\n";
            Circuit* circuit = createCircuitFromString(circuitStr);
            circuit->validate();
            circuit->calculateImpedances();
            bool result = circuit->buildBranches();

            Assert::IsTrue(result);
            const auto& branches = circuit->getBranches();
            Assert::AreEqual(size_t(3), branches.size());

            int br0 = -1, br1 = -1, br2 = -1;
            for (size_t i = 0; i < branches.size(); i++) {
                const auto& nodes = branches[i]->getNodes();
                if (nodes.size() == 1 && nodes[0]->getName() == "SOURCE") br0 = (int)i;
                else if (nodes.size() == 3 && nodes[0]->getName() == "R1" && nodes[1]->getName() == "R3" && nodes[2]->getName() == "R4") br1 = (int)i;
                else if (nodes.size() == 2 && nodes[0]->getName() == "R2" && nodes[1]->getName() == "R5") br2 = (int)i;
            }
            Assert::IsTrue(br0 >= 0 && br1 >= 0 && br2 >= 0);

            VerifyBranchNodes(branches[br0], { "SOURCE" });
            VerifyBranchNodes(branches[br1], { "R1", "R3", "R4" });
            VerifyBranchNodes(branches[br2], { "R2", "R5" });

            VerifyConnections(branches[br0], { br1, br2 }, { br1, br2 }, branches);
            VerifyConnections(branches[br1], { br0 }, { br0 }, branches);
            VerifyConnections(branches[br2], { br0 }, { br0 }, branches);
            delete circuit;
        }

        TEST_METHOD(ComplexNested)
        {
            string circuitStr =
                "digraph circuit {\n"
                "SOURCE [label=\"SOURCE frequency=50 voltage=100 phase=0\"]\n"
                "R1 [label=\"R=10\"]\n"
                "R2 [label=\"R=20\"]\n"
                "R3 [label=\"R=30\"]\n"
                "R4 [label=\"R=40\"]\n"
                "R5 [label=\"R=50\"]\n"
                "R6 [label=\"R=60\"]\n"
                "R7 [label=\"R=70\"]\n"
                "R8 [label=\"R=80\"]\n"
                "SOURCE -> R1\n"
                "R1 -> R2\n"
                "R1 -> R3\n"
                "R2 -> R4\n"
                "R2 -> R5\n"
                "R3 -> R6\n"
                "R3 -> R7\n"
                "R4 -> R8\n"
                "R5 -> R8\n"
                "R6 -> R8\n"
                "R7 -> R8\n"
                "R8 -> SOURCE\n"
                "}\n";
            Circuit* circuit = createCircuitFromString(circuitStr);
            circuit->validate();
            circuit->calculateImpedances();
            bool result = circuit->buildBranches();

            Assert::IsTrue(result);
            const auto& branches = circuit->getBranches();
            Assert::AreEqual(size_t(8), branches.size());

            int br0 = -1, br1 = -1, br2 = -1, br3 = -1, br4 = -1, br5 = -1, br6 = -1, br7 = -1;
            for (size_t i = 0; i < branches.size(); i++) {
                const auto& nodes = branches[i]->getNodes();
                if (nodes.size() == 2 && nodes[0]->getName() == "SOURCE" && nodes[1]->getName() == "R1") br0 = (int)i;
                else if (nodes.size() == 1 && nodes[0]->getName() == "R2") br1 = (int)i;
                else if (nodes.size() == 1 && nodes[0]->getName() == "R3") br2 = (int)i;
                else if (nodes.size() == 1 && nodes[0]->getName() == "R4") br3 = (int)i;
                else if (nodes.size() == 1 && nodes[0]->getName() == "R5") br4 = (int)i;
                else if (nodes.size() == 1 && nodes[0]->getName() == "R6") br5 = (int)i;
                else if (nodes.size() == 1 && nodes[0]->getName() == "R7") br6 = (int)i;
                else if (nodes.size() == 1 && nodes[0]->getName() == "R8") br7 = (int)i;
            }
            Assert::IsTrue(br0 >= 0 && br1 >= 0 && br2 >= 0 && br3 >= 0 && br4 >= 0 && br5 >= 0 && br6 >= 0 && br7 >= 0);

            VerifyBranchNodes(branches[br0], { "SOURCE", "R1" });
            VerifyBranchNodes(branches[br1], { "R2" });
            VerifyBranchNodes(branches[br2], { "R3" });
            VerifyBranchNodes(branches[br3], { "R4" });
            VerifyBranchNodes(branches[br4], { "R5" });
            VerifyBranchNodes(branches[br5], { "R6" });
            VerifyBranchNodes(branches[br6], { "R7" });
            VerifyBranchNodes(branches[br7], { "R8" });

            VerifyConnections(branches[br0], { br1, br2 }, { br7 }, branches);
            VerifyConnections(branches[br1], { br3, br4 }, { br0 }, branches);
            VerifyConnections(branches[br2], { br5, br6 }, { br0 }, branches);
            VerifyConnections(branches[br3], { br7 }, { br1 }, branches);
            VerifyConnections(branches[br4], { br7 }, { br1 }, branches);
            VerifyConnections(branches[br5], { br7 }, { br2 }, branches);
            VerifyConnections(branches[br6], { br7 }, { br2 }, branches);
            VerifyConnections(branches[br7], { br0 }, { br3, br4, br5, br6 }, branches);
            delete circuit;
        }

        TEST_METHOD(ComplexLarge)
        {
            string circuitStr =
                "digraph circuit {\n"
                "SOURCE [label=\"SOURCE frequency=50 voltage=100 phase=0\"]\n"
                "R1 [label=\"R=10\"]\n"
                "R2 [label=\"R=20\"]\n"
                "R3 [label=\"R=30\"]\n"
                "R4 [label=\"R=40\"]\n"
                "R5 [label=\"R=50\"]\n"
                "R6 [label=\"R=60\"]\n"
                "R7 [label=\"R=70\"]\n"
                "R8 [label=\"R=80\"]\n"
                "R9 [label=\"R=90\"]\n"
                "R10 [label=\"R=100\"]\n"
                "R11 [label=\"R=110\"]\n"
                "SOURCE -> R1\n"
                "R1 -> R2\n"
                "R1 -> R3\n"
                "R2 -> R4\n"
                "R2 -> R5\n"
                "R3 -> R6\n"
                "R3 -> R7\n"
                "R4 -> R8\n"
                "R5 -> R8\n"
                "R6 -> R9\n"
                "R7 -> R9\n"
                "R8 -> R10\n"
                "R9 -> R11\n"
                "R10 -> SOURCE\n"
                "R11 -> SOURCE\n"
                "}\n";
            Circuit* circuit = createCircuitFromString(circuitStr);
            circuit->validate();
            circuit->calculateImpedances();
            bool result = circuit->buildBranches();

            Assert::IsTrue(result);
            const auto& branches = circuit->getBranches();
            Assert::AreEqual(size_t(9), branches.size());

            int br0 = -1, br1 = -1, br2 = -1, br3 = -1, br4 = -1, br5 = -1, br6 = -1, br7 = -1, br8 = -1;
            for (size_t i = 0; i < branches.size(); i++) {
                const auto& nodes = branches[i]->getNodes();
                if (nodes.size() == 2 && nodes[0]->getName() == "SOURCE" && nodes[1]->getName() == "R1") br0 = (int)i;
                else if (nodes.size() == 1 && nodes[0]->getName() == "R2") br1 = (int)i;
                else if (nodes.size() == 1 && nodes[0]->getName() == "R3") br2 = (int)i;
                else if (nodes.size() == 1 && nodes[0]->getName() == "R4") br3 = (int)i;
                else if (nodes.size() == 1 && nodes[0]->getName() == "R5") br4 = (int)i;
                else if (nodes.size() == 1 && nodes[0]->getName() == "R6") br5 = (int)i;
                else if (nodes.size() == 1 && nodes[0]->getName() == "R7") br6 = (int)i;
                else if (nodes.size() == 2 && nodes[0]->getName() == "R8" && nodes[1]->getName() == "R10") br7 = (int)i;
                else if (nodes.size() == 2 && nodes[0]->getName() == "R9" && nodes[1]->getName() == "R11") br8 = (int)i;
            }
            Assert::IsTrue(br0 >= 0 && br1 >= 0 && br2 >= 0 && br3 >= 0 && br4 >= 0 && br5 >= 0 && br6 >= 0 && br7 >= 0 && br8 >= 0);

            VerifyBranchNodes(branches[br0], { "SOURCE", "R1" });
            VerifyBranchNodes(branches[br1], { "R2" });
            VerifyBranchNodes(branches[br2], { "R3" });
            VerifyBranchNodes(branches[br3], { "R4" });
            VerifyBranchNodes(branches[br4], { "R5" });
            VerifyBranchNodes(branches[br5], { "R6" });
            VerifyBranchNodes(branches[br6], { "R7" });
            VerifyBranchNodes(branches[br7], { "R8", "R10" });
            VerifyBranchNodes(branches[br8], { "R9", "R11" });

            VerifyConnections(branches[br0], { br1, br2 }, { br7, br8 }, branches);
            VerifyConnections(branches[br1], { br3, br4 }, { br0 }, branches);
            VerifyConnections(branches[br2], { br5, br6 }, { br0 }, branches);
            VerifyConnections(branches[br3], { br7 }, { br1 }, branches);
            VerifyConnections(branches[br4], { br7 }, { br1 }, branches);
            VerifyConnections(branches[br5], { br8 }, { br2 }, branches);
            VerifyConnections(branches[br6], { br8 }, { br2 }, branches);
            VerifyConnections(branches[br7], { br0 }, { br3, br4 }, branches);
            VerifyConnections(branches[br8], { br0 }, { br5, br6 }, branches);
            delete circuit;
        }

        TEST_METHOD(ComplexRLC)
        {
            string circuitStr =
                "digraph circuit {\n"
                "SOURCE [label=\"SOURCE frequency=50 voltage=100 phase=0\"]\n"
                "R1 [label=\"R=10\"]\n"
                "R2 [label=\"R=20\"]\n"
                "R3 [label=\"R=30\"]\n"
                "R4 [label=\"R=40\"]\n"
                "L1 [label=\"L=0.1\"]\n"
                "L2 [label=\"L=0.2\"]\n"
                "L3 [label=\"L=0.3\"]\n"
                "L4 [label=\"L=0.4\"]\n"
                "C1 [label=\"C=1.13e-4\"]\n"
                "C2 [label=\"C=2.26e-4\"]\n"
                "C3 [label=\"C=3.39e-4\"]\n"
                "C4 [label=\"C=4.52e-4\"]\n"
                "SOURCE -> R1\n"
                "R1 -> R2\n"
                "R1 -> R3\n"
                "R2 -> L1\n"
                "R2 -> C1\n"
                "R3 -> L2\n"
                "R3 -> C2\n"
                "L1 -> R4\n"
                "C1 -> R4\n"
                "L2 -> L3\n"
                "C2 -> L3\n"
                "R4 -> L4\n"
                "L3 -> C3\n"
                "L4 -> C4\n"
                "C3 -> SOURCE\n"
                "C4 -> SOURCE\n"
                "}\n";
            Circuit* circuit = createCircuitFromString(circuitStr);
            circuit->validate();
            circuit->calculateImpedances();
            bool result = circuit->buildBranches();

            Assert::IsTrue(result);
            const auto& branches = circuit->getBranches();
            Assert::AreEqual(size_t(9), branches.size());

            int br0 = -1, br1 = -1, br2 = -1, br3 = -1, br4 = -1, br5 = -1, br6 = -1, br7 = -1, br8 = -1;
            for (size_t i = 0; i < branches.size(); i++) {
                const auto& nodes = branches[i]->getNodes();
                if (nodes.size() == 2 && nodes[0]->getName() == "SOURCE" && nodes[1]->getName() == "R1") br0 = (int)i;
                else if (nodes.size() == 1 && nodes[0]->getName() == "R2") br1 = (int)i;
                else if (nodes.size() == 1 && nodes[0]->getName() == "R3") br2 = (int)i;
                else if (nodes.size() == 1 && nodes[0]->getName() == "L1") br3 = (int)i;
                else if (nodes.size() == 1 && nodes[0]->getName() == "C1") br4 = (int)i;
                else if (nodes.size() == 1 && nodes[0]->getName() == "L2") br5 = (int)i;
                else if (nodes.size() == 1 && nodes[0]->getName() == "C2") br6 = (int)i;
                else if (nodes.size() == 3 && nodes[0]->getName() == "R4" && nodes[1]->getName() == "L4" && nodes[2]->getName() == "C4") br7 = (int)i;
                else if (nodes.size() == 2 && nodes[0]->getName() == "L3" && nodes[1]->getName() == "C3") br8 = (int)i;
            }
            Assert::IsTrue(br0 >= 0 && br1 >= 0 && br2 >= 0 && br3 >= 0 && br4 >= 0 && br5 >= 0 && br6 >= 0 && br7 >= 0 && br8 >= 0);

            VerifyBranchNodes(branches[br0], { "SOURCE", "R1" });
            VerifyBranchNodes(branches[br1], { "R2" });
            VerifyBranchNodes(branches[br2], { "R3" });
            VerifyBranchNodes(branches[br3], { "L1" });
            VerifyBranchNodes(branches[br4], { "C1" });
            VerifyBranchNodes(branches[br5], { "L2" });
            VerifyBranchNodes(branches[br6], { "C2" });
            VerifyBranchNodes(branches[br7], { "R4", "L4", "C4" });
            VerifyBranchNodes(branches[br8], { "L3", "C3" });

            VerifyConnections(branches[br0], { br1, br2 }, { br7, br8 }, branches);
            VerifyConnections(branches[br1], { br3, br4 }, { br0 }, branches);
            VerifyConnections(branches[br2], { br5, br6 }, { br0 }, branches);
            VerifyConnections(branches[br3], { br7 }, { br1 }, branches);
            VerifyConnections(branches[br4], { br7 }, { br1 }, branches);
            VerifyConnections(branches[br5], { br8 }, { br2 }, branches);
            VerifyConnections(branches[br6], { br8 }, { br2 }, branches);
            VerifyConnections(branches[br7], { br0 }, { br3, br4 }, branches);
            VerifyConnections(branches[br8], { br0 }, { br5, br6 }, branches);
            delete circuit;
        }
    };
}