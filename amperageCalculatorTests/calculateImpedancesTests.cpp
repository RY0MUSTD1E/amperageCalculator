/**
 * \file calculateImpedancesTests.cpp
 * \brief Модульные тесты для проверки расчёта импедансов компонентов цепи.
 *
 * Данный файл содержит набор тестов, реализованных с использованием Microsoft CppUnitTestFramework.
 * Тестируется корректность вычисления комплексных сопротивлений резисторов,
 * катушек индуктивности и конденсаторов при различных частотах и номиналах.
 *
 * \author Anna Bezhenar
 * \date June 2026
 */

#include "pch.h"
#include "CppUnitTest.h"
#include "testHelpers.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;
using namespace std;
using namespace TestHelpers;

namespace CircuitTests
{
    TEST_CLASS(CalculateImpedancesTests)
    {
    public:
        const double PI = 3.14159265358979323846;

        TEST_METHOD(CapacitorZeroCapacitance)
        {
            string circuitStr =
                "digraph circuit {\n"
                "source [label=\"SOURCE frequency=50 voltage=100 phase=0\"]\n"
                "C1 [label=\"C=0\"]\n"
                "source -> C1\n"
                "C1 -> source\n"
                "}\n";

            Circuit* circuit = createCircuitFromString(circuitStr);
            circuit->validate();

            Assert::IsFalse(circuit->calculateImpedances());


            delete circuit;
        }
        TEST_METHOD(Resistor100Ohm)
        {
            string circuitStr =
                "digraph circuit {\n"
                "source [label=\"SOURCE frequency=50 voltage=100 phase=0\"]\n"
                "R1 [label=\"R=100\"]\n"
                "source -> R1\n"
                "R1 -> source\n"
                "}\n";
            Circuit* circuit = createCircuitFromString(circuitStr);
            circuit->validate();
            circuit->calculateImpedances();
            CircuitNode* r1 = circuit->getNodeByName("R1");
            complex<double> expected(100.0, 0.0);
            Assert::IsTrue(abs(r1->getResistance() - expected) < 1e-6);
            delete circuit;
        }

        TEST_METHOD(Resistor25_5Ohm)
        {
            string circuitStr =
                "digraph circuit {\n"
                "source [label=\"SOURCE frequency=50 voltage=100 phase=0\"]\n"
                "R1 [label=\"R=25.5\"]\n"
                "source -> R1\n"
                "R1 -> source\n"
                "}\n";
            Circuit* circuit = createCircuitFromString(circuitStr);
            circuit->validate();
            circuit->calculateImpedances();
            CircuitNode* r1 = circuit->getNodeByName("R1");
            complex<double> expected(25.5, 0.0);
            Assert::IsTrue(abs(r1->getResistance() - expected) < 1e-6);
            delete circuit;
        }

        TEST_METHOD(Resistor10000Ohm)
        {
            string circuitStr =
                "digraph circuit {\n"
                "source [label=\"SOURCE frequency=50 voltage=100 phase=0\"]\n"
                "R1 [label=\"R=10000\"]\n"
                "source -> R1\n"
                "R1 -> source\n"
                "}\n";
            Circuit* circuit = createCircuitFromString(circuitStr);
            circuit->validate();
            circuit->calculateImpedances();
            CircuitNode* r1 = circuit->getNodeByName("R1");
            complex<double> expected(10000.0, 0.0);
            Assert::IsTrue(abs(r1->getResistance() - expected) < 1e-6);
            delete circuit;
        }

        TEST_METHOD(Coil01H_50Hz)
        {
            string circuitStr =
                "digraph circuit {\n"
                "source [label=\"SOURCE frequency=50 voltage=100 phase=0\"]\n"
                "L1 [label=\"L=0.1\"]\n"
                "source -> L1\n"
                "L1 -> source\n"
                "}\n";
            Circuit* circuit = createCircuitFromString(circuitStr);
            circuit->validate();
            circuit->calculateImpedances();
            CircuitNode* l1 = circuit->getNodeByName("L1");
            double omega = 2.0 * PI * 50.0;
            complex<double> expected(0.0, omega * 0.1);
            Assert::IsTrue(abs(l1->getResistance() - expected) < 1e-2);
            delete circuit;
        }

        TEST_METHOD(Coil1H_50Hz)
        {
            string circuitStr =
                "digraph circuit {\n"
                "source [label=\"SOURCE frequency=50 voltage=100 phase=0\"]\n"
                "L1 [label=\"L=1\"]\n"
                "source -> L1\n"
                "L1 -> source\n"
                "}\n";
            Circuit* circuit = createCircuitFromString(circuitStr);
            circuit->validate();
            circuit->calculateImpedances();
            CircuitNode* l1 = circuit->getNodeByName("L1");
            double omega = 2.0 * PI * 50.0;
            complex<double> expected(0.0, omega * 1.0);
            Assert::IsTrue(abs(l1->getResistance() - expected) < 1e-2);
            delete circuit;
        }

        TEST_METHOD(Coil0_001H_50Hz)
        {
            string circuitStr =
                "digraph circuit {\n"
                "source [label=\"SOURCE frequency=50 voltage=100 phase=0\"]\n"
                "L1 [label=\"L=0.001\"]\n"
                "source -> L1\n"
                "L1 -> source\n"
                "}\n";
            Circuit* circuit = createCircuitFromString(circuitStr);
            circuit->validate();
            circuit->calculateImpedances();
            CircuitNode* l1 = circuit->getNodeByName("L1");
            double omega = 2.0 * PI * 50.0;
            complex<double> expected(0.0, omega * 0.001);
            Assert::IsTrue(abs(l1->getResistance() - expected) < 1e-2);
            delete circuit;
        }

        TEST_METHOD(Capacitor100uF_50Hz)
        {
            string circuitStr =
                "digraph circuit {\n"
                "source [label=\"SOURCE frequency=50 voltage=100 phase=0\"]\n"
                "C1 [label=\"C=100e-6\"]\n"
                "source -> C1\n"
                "C1 -> source\n"
                "}\n";
            Circuit* circuit = createCircuitFromString(circuitStr);
            circuit->validate();
            circuit->calculateImpedances();
            CircuitNode* c1 = circuit->getNodeByName("C1");
            double omega = 2.0 * PI * 50.0;
            complex<double> expected(0.0, -1.0 / (omega * 100e-6));
            Assert::IsTrue(abs(c1->getResistance() - expected) < 1e-2);
            delete circuit;
        }

        TEST_METHOD(Capacitor10uF_50Hz)
        {
            string circuitStr =
                "digraph circuit {\n"
                "source [label=\"SOURCE frequency=50 voltage=100 phase=0\"]\n"
                "C1 [label=\"C=10e-6\"]\n"
                "source -> C1\n"
                "C1 -> source\n"
                "}\n";
            Circuit* circuit = createCircuitFromString(circuitStr);
            circuit->validate();
            circuit->calculateImpedances();
            CircuitNode* c1 = circuit->getNodeByName("C1");
            double omega = 2.0 * PI * 50.0;
            complex<double> expected(0.0, -1.0 / (omega * 10e-6));
            Assert::IsTrue(abs(c1->getResistance() - expected) < 1e-2);
            delete circuit;
        }

        TEST_METHOD(Capacitor1000uF_50Hz)
        {
            string circuitStr =
                "digraph circuit {\n"
                "source [label=\"SOURCE frequency=50 voltage=100 phase=0\"]\n"
                "C1 [label=\"C=1000e-6\"]\n"
                "source -> C1\n"
                "C1 -> source\n"
                "}\n";
            Circuit* circuit = createCircuitFromString(circuitStr);
            circuit->validate();
            circuit->calculateImpedances();
            CircuitNode* c1 = circuit->getNodeByName("C1");
            double omega = 2.0 * PI * 50.0;
            complex<double> expected(0.0, -1.0 / (omega * 1000e-6));
            Assert::IsTrue(abs(c1->getResistance() - expected) < 1e-2);
            delete circuit;
        }

        TEST_METHOD(Coil100Hz)
        {
            string circuitStr =
                "digraph circuit {\n"
                "source [label=\"SOURCE frequency=100 voltage=100 phase=0\"]\n"
                "L1 [label=\"L=0.1\"]\n"
                "source -> L1\n"
                "L1 -> source\n"
                "}\n";
            Circuit* circuit = createCircuitFromString(circuitStr);
            circuit->validate();
            circuit->calculateImpedances();
            CircuitNode* l1 = circuit->getNodeByName("L1");
            double omega = 2.0 * PI * 100.0;
            complex<double> expected(0.0, omega * 0.1);
            Assert::IsTrue(abs(l1->getResistance() - expected) < 1e-2);
            delete circuit;
        }

        TEST_METHOD(Capacitor100Hz)
        {
            string circuitStr =
                "digraph circuit {\n"
                "source [label=\"SOURCE frequency=100 voltage=100 phase=0\"]\n"
                "C1 [label=\"C=100e-6\"]\n"
                "source -> C1\n"
                "C1 -> source\n"
                "}\n";
            Circuit* circuit = createCircuitFromString(circuitStr);
            circuit->validate();
            circuit->calculateImpedances();
            CircuitNode* c1 = circuit->getNodeByName("C1");
            double omega = 2.0 * PI * 100.0;
            complex<double> expected(0.0, -1.0 / (omega * 100e-6));
            Assert::IsTrue(abs(c1->getResistance() - expected) < 1e-2);
            delete circuit;
        }
    };
}