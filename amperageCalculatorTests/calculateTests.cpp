/**
 * \file CalculateTests.cpp
 * \brief Модульные тесты для проверки полного расчёта электрической цепи.
 *
 * Данный файл содержит набор тестов, реализованных с использованием Microsoft CppUnitTestFramework.
 * Тестируется вычисление импедансов, эквивалентного сопротивления цепи,
 * токов в ветвях и корректность распределения токов по узлам.
 *
 * \author Anna Bezhenar
 * \date June 2026
 */

#include "pch.h"
#include "testHelpers.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;
using namespace std;
using namespace TestHelpers;

namespace CircuitTests
{
    TEST_CLASS(CalculateTests)
    {
    private:


    public:
        TEST_METHOD(SeriesThreeResistors)
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
            bool result = circuit->calculate();

            Assert::IsTrue(result);
            VerifyAmperage(circuit, "R1", 1.66667, 0.0);
            VerifyAmperage(circuit, "R2", 1.66667, 0.0);
            VerifyAmperage(circuit, "R3", 1.66667, 0.0);
            VerifyAmperage(circuit, "SOURCE", 1.66667, 0.0);

            delete circuit;
        }

        TEST_METHOD(ParallelThreeResistors)
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
            bool result = circuit->calculate();

            Assert::IsTrue(result);
            VerifyAmperage(circuit, "R1", 10.0, 0.0, 1e-5);
            VerifyAmperage(circuit, "R2", 5.0, 0.0, 1e-5);
            VerifyAmperage(circuit, "R3", 3.333333, 0.0, 1e-5);
            VerifyAmperage(circuit, "SOURCE", 18.333333, 0.0, 1e-5);

            delete circuit;
        }

        TEST_METHOD(MixedSeriesParallel)
        {
            string circuitStr =
                "digraph circuit {\n"
                "SOURCE [label=\"SOURCE frequency=50 voltage=100 phase=0\"]\n"
                "R1 [label=\"R=10\"]\n"
                "R2 [label=\"R=20\"]\n"
                "R3 [label=\"R=20\"]\n"
                "SOURCE -> R1\n"
                "R1 -> R2\n"
                "R1 -> R3\n"
                "R2 -> SOURCE\n"
                "R3 -> SOURCE\n"
                "}\n";

            Circuit* circuit = createCircuitFromString(circuitStr);
            bool result = circuit->calculate();

            Assert::IsTrue(result);
            VerifyAmperage(circuit, "R1", 5.0, 0.0, 1e-5);
            VerifyAmperage(circuit, "R2", 2.5, 0.0, 1e-5);
            VerifyAmperage(circuit, "R3", 2.5, 0.0, 1e-5);
            VerifyAmperage(circuit, "SOURCE", 5.0, 0.0, 1e-5);

            delete circuit;
        }

        TEST_METHOD(RLCircuit)
        {
            string circuitStr =
                "digraph circuit {\n"
                "SOURCE [label=\"SOURCE frequency=50 voltage=100 phase=0\"]\n"
                "R1 [label=\"R=10\"]\n"
                "L1 [label=\"L=1\"]\n"
                "SOURCE -> R1\n"
                "R1 -> L1\n"
                "L1 -> SOURCE\n"
                "}\n";

            Circuit* circuit = createCircuitFromString(circuitStr);
            bool result = circuit->calculate();

            Assert::IsTrue(result);
            VerifyAmperage(circuit, "R1", 0.010122, -0.317988, 1e-4);
            VerifyAmperage(circuit, "L1", 0.010122, -0.317988, 1e-4);
            VerifyAmperage(circuit, "SOURCE", 0.010122, -0.317988, 1e-4);

            delete circuit;
        }

        TEST_METHOD(RCCircuit)
        {
            string circuitStr =
                "digraph circuit {\n"
                "SOURCE [label=\"SOURCE frequency=50 voltage=100 phase=0\"]\n"
                "R1 [label=\"R=10\"]\n"
                "C1 [label=\"C=100e-6\"]\n"
                "SOURCE -> R1\n"
                "R1 -> C1\n"
                "C1 -> SOURCE\n"
                "}\n";

            Circuit* circuit = createCircuitFromString(circuitStr);
            bool result = circuit->calculate();

            Assert::IsTrue(result);
            VerifyAmperage(circuit, "R1", 0.898302, 2.859383, 1e-4);
            VerifyAmperage(circuit, "C1", 0.898302, 2.859383, 1e-4);
            VerifyAmperage(circuit, "SOURCE", 0.898302, 2.859383, 1e-4);

            delete circuit;
        }

        TEST_METHOD(RLCSeriesCircuit)
        {
            string circuitStr =
                "digraph circuit {\n"
                "SOURCE [label=\"SOURCE frequency=50 voltage=100 phase=0\"]\n"
                "R1 [label=\"R=10\"]\n"
                "L1 [label=\"L=1\"]\n"
                "C1 [label=\"C=100e-6\"]\n"
                "SOURCE -> R1\n"
                "R1 -> L1\n"
                "L1 -> C1\n"
                "C1 -> SOURCE\n"
                "}\n";

            Circuit* circuit = createCircuitFromString(circuitStr);
            bool result = circuit->calculate();

            Assert::IsTrue(result);
            VerifyAmperage(circuit, "R1", 0.012530, -0.353754, 1e-4);
            VerifyAmperage(circuit, "L1", 0.012530, -0.353754, 1e-4);
            VerifyAmperage(circuit, "C1", 0.012530, -0.353754, 1e-4);
            VerifyAmperage(circuit, "SOURCE", 0.012530, -0.353754, 1e-4);

            delete circuit;
        }

        TEST_METHOD(TenParallelResistors)
        {
            stringstream ss;
            ss << "digraph circuit {\n";
            ss << "SOURCE [label=\"SOURCE frequency=50 voltage=100 phase=0\"]\n";
            vector<int> values = { 10, 20, 30, 40, 50, 60, 70, 80, 90, 100 };
            for (size_t i = 0; i < values.size(); i++) {
                ss << "R" << (i + 1) << " [label=\"R=" << values[i] << "\"]\n";
            }
            for (size_t i = 0; i < values.size(); i++) {
                ss << "SOURCE -> R" << (i + 1) << "\n";
            }
            for (size_t i = 0; i < values.size(); i++) {
                ss << "R" << (i + 1) << " -> SOURCE\n";
            }
            ss << "}\n";

            Circuit* circuit = createCircuitFromString(ss.str());
            bool result = circuit->calculate();

            Assert::IsTrue(result);

            double totalAmperage = 0.0;
            for (size_t i = 0; i < values.size(); i++) {
                double expected = 100.0 / values[i];
                string nodeName = "R" + to_string(i + 1);
                VerifyAmperage(circuit, nodeName, expected, 0.0, 1e-4);
                totalAmperage += expected;
            }

            VerifyAmperage(circuit, "SOURCE", totalAmperage, 0.0, 1e-4);

            delete circuit;
        }

        TEST_METHOD(SeriesResonanceLowResistance)
        {
            string circuitStr =
                "digraph circuit {\n"
                "SOURCE [label=\"SOURCE frequency=50 voltage=100 phase=0\"]\n"
                "R1 [label=\"R=1e-11\"]\n"
                "L1 [label=\"L=0.1\"]\n"
                "C1 [label=\"C=101.32e-6\"]\n"
                "SOURCE -> R1\n"
                "R1 -> L1\n"
                "L1 -> C1\n"
                "C1 -> SOURCE\n"
                "}\n";

            Circuit* circuit = createCircuitFromString(circuitStr);
            bool result = circuit->calculate();

            Assert::IsTrue(result);

            VerifyAmperage(circuit, "R1", 0.007424, 272473.843146, 1.0);
            VerifyAmperage(circuit, "L1", 0.007424, 272473.843146, 1.0);
            VerifyAmperage(circuit, "C1", 0.007424, 272473.843146, 1.0);
            VerifyAmperage(circuit, "SOURCE", 0.007424, 272473.843146, 1.0);

            delete circuit;
        }

        TEST_METHOD(OnlyInductor)
        {
            string circuitStr =
                "digraph circuit {\n"
                "SOURCE [label=\"SOURCE frequency=50 voltage=100 phase=0\"]\n"
                "L1 [label=\"L=0.1\"]\n"
                "SOURCE -> L1\n"
                "L1 -> SOURCE\n"
                "}\n";

            Circuit* circuit = createCircuitFromString(circuitStr);
            bool result = circuit->calculate();

            Assert::IsTrue(result);

            VerifyAmperage(circuit, "L1", 0.0, -3.183099, 1e-4);
            VerifyAmperage(circuit, "SOURCE", 0.0, -3.183099, 1e-4);

            delete circuit;
        }

        TEST_METHOD(OnlyCapacitor)
        {
            string circuitStr =
                "digraph circuit {\n"
                "SOURCE [label=\"SOURCE frequency=50 voltage=100 phase=0\"]\n"
                "C1 [label=\"C=100e-6\"]\n"
                "SOURCE -> C1\n"
                "C1 -> SOURCE\n"
                "}\n";

            Circuit* circuit = createCircuitFromString(circuitStr);
            bool result = circuit->calculate();

            Assert::IsTrue(result);

            VerifyAmperage(circuit, "C1", 0.0, 3.141593, 1e-4);
            VerifyAmperage(circuit, "SOURCE", 0.0, 3.141593, 1e-4);

            delete circuit;
        }

        TEST_METHOD(ParallelResonance)
        {
            string circuitStr =
                "digraph circuit {\n"
                "SOURCE [label=\"SOURCE frequency=50 voltage=100 phase=0\"]\n"
                "L1 [label=\"L=0.1\"]\n"
                "C1 [label=\"C=101.32e-6\"]\n"
                "SOURCE -> L1\n"
                "SOURCE -> C1\n"
                "L1 -> SOURCE\n"
                "C1 -> SOURCE\n"
                "}\n";

            Circuit* circuit = createCircuitFromString(circuitStr);
            bool result = circuit->calculate();

            Assert::IsTrue(result);

            VerifyAmperage(circuit, "L1", 0.0, -3.183099, 1e-4);
            VerifyAmperage(circuit, "C1", 0.0, 3.183062, 1e-4);
            VerifyAmperage(circuit, "SOURCE", 0.0, -0.000037, 1e-4);

            delete circuit;
        }

        TEST_METHOD(TwoLevelParallelization)
        {
            string circuitStr =
                "digraph circuit {\n"
                "SOURCE [label=\"SOURCE frequency=50 voltage=100 phase=0\"]\n"
                "R1 [label=\"R=10\"]\n"
                "R2 [label=\"R=20\"]\n"
                "R3 [label=\"R=30\"]\n"
                "R4 [label=\"R=40\"]\n"
                "R5 [label=\"R=40\"]\n"
                "SOURCE -> R1\n"
                "R1 -> R2\n"
                "R1 -> R3\n"
                "R2 -> R4\n"
                "R2 -> R5\n"
                "R3 -> SOURCE\n"
                "R4 -> SOURCE\n"
                "R5 -> SOURCE\n"
                "}\n";

            Circuit* circuit = createCircuitFromString(circuitStr);
            bool result = circuit->calculate();

            Assert::IsTrue(result);

            VerifyAmperage(circuit, "R1", 3.684211, 0.0, 1e-4);
            VerifyAmperage(circuit, "R2", 2.210526, 0.0, 1e-4);
            VerifyAmperage(circuit, "R3", 1.473684, 0.0, 1e-4);
            VerifyAmperage(circuit, "R4", 1.105263, 0.0, 1e-4);
            VerifyAmperage(circuit, "R5", 1.105263, 0.0, 1e-4);
            VerifyAmperage(circuit, "SOURCE", 3.684211, 0.0, 1e-4);

            delete circuit;
        }

        TEST_METHOD(ComplexRLCNetwork)
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
                "C1 [label=\"C=100e-6\"]\n"
                "C2 [label=\"C=200e-6\"]\n"
                "C3 [label=\"C=300e-6\"]\n"
                "C4 [label=\"C=400e-6\"]\n"
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
            bool result = circuit->calculate();

            Assert::IsTrue(result);

            VerifyAmperage(circuit, "R1", 3.049905, 1.727921, 1e-3);
            VerifyAmperage(circuit, "R2", 1.829943, 1.036753, 1e-3);
            VerifyAmperage(circuit, "R3", 1.219962, 0.691169, 1e-3);
            VerifyAmperage(circuit, "R4", 140.337775, 79.508269, 0.5);
            VerifyAmperage(circuit, "L1", 140.337775, 79.508269, 0.5);
            VerifyAmperage(circuit, "L2", -0.413849, -0.234466, 0.01);
            VerifyAmperage(circuit, "L3", -0.413849, -0.234466, 0.01);
            VerifyAmperage(circuit, "L4", 140.337775, 79.508269, 0.5);
            VerifyAmperage(circuit, "C1", -138.507832, -78.471516, 0.5);
            VerifyAmperage(circuit, "C2", 1.633811, 0.925635, 0.01);
            VerifyAmperage(circuit, "C3", -0.413849, -0.234466, 0.01);
            VerifyAmperage(circuit, "C4", 140.337775, 79.508269, 0.5);
            VerifyAmperage(circuit, "SOURCE", 3.049905, 1.727921, 1e-3);

            delete circuit;
        }
    };
}