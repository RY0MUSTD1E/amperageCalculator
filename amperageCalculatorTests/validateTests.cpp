/**
 * \file validateTests.cpp
 * \brief Модульные тесты для проверки валидации входных данных электрической цепи.
 *
 * Данный файл содержит набор тестов, реализованных с использованием Microsoft CppUnitTestFramework.
 * Тестируются проверки: имён компонентов, уникальности, наличия источника,
 * корректности значений, связности, замкнутости цепи и количества параллельных элементов.
 *
 * \author Anna Bezhenar
 * \date June 2026
 */

#include "pch.h"
#include "CppUnitTest.h"
#include "TestHelpers.h"
#include <sstream>

using namespace Microsoft::VisualStudio::CppUnitTestFramework;
using namespace std;
using namespace TestHelpers;

namespace CircuitTests
{
    TEST_CLASS(ValidateTests)
    {
    public:
        TEST_METHOD(TooManyElements)
        {
            stringstream ss;
            ss << "digraph circuit {\n";
            ss << "SOURCE [label=\"SOURCE frequency=50 voltage=100 phase=0\"]\n";
            for (int i = 1; i <= 100; i++) {
                ss << "R" << i << " [label=\"R=10\"]\n";
            }
            ss << "SOURCE -> R1\n";
            for (int i = 1; i < 100; i++) {
                ss << "R" << i << " -> R" << (i + 1) << "\n";
            }
            ss << "R100 -> SOURCE\n";
            ss << "}\n";

            Circuit* circuit = createCircuitFromString(ss.str());
            bool result = circuit->validate();
            Assert::IsFalse(result);
            Assert::AreEqual(int(ErrorType::TooManyElements), int(circuit->getError().type));
            delete circuit;
        }

        TEST_METHOD(TooManyEdges)
        {
            stringstream ss;
            ss << "digraph circuit {\n";
            ss << "SOURCE [label=\"SOURCE frequency=50 voltage=100 phase=0\"]\n";
            ss << "R1 [label=\"R=10\"]\n";
            for (int i = 0; i < 101; i++) {
                ss << "SOURCE -> R1\n";
            }
            ss << "}\n";

            Circuit* circuit = createCircuitFromString(ss.str());
            bool result = circuit->validate();
            Assert::IsFalse(result);
            Assert::AreEqual(int(ErrorType::TooManyEdges), int(circuit->getError().type));
            delete circuit;
        }

        TEST_METHOD(InvalidName_SpecialChars)
        {
            string circuitStr =
                "digraph circuit {\n"
                "SOURCE [label=\"SOURCE frequency=50 voltage=100 phase=0\"]\n"
                "R1_2 [label=\"R=10\"]\n"
                "R2 [label=\"R=20\"]\n"
                "SOURCE -> R1_2\n"
                "R1_2 -> R2\n"
                "R2 -> SOURCE\n"
                "}\n";

            Circuit* circuit = createCircuitFromString(circuitStr);
            Assert::IsTrue(circuit->getError().hasError());
            Assert::AreEqual(int(ErrorType::InvalidComponentName), int(circuit->getError().type));
            delete circuit;
        }

        TEST_METHOD(InvalidName_TooLong)
        {
            string circuitStr =
                "digraph circuit {\n"
                "SOURCE [label=\"SOURCE frequency=50 voltage=100 phase=0\"]\n"
                "Resistor1 [label=\"R=10\"]\n"
                "R2 [label=\"R=20\"]\n"
                "SOURCE -> Resistor1\n"
                "Resistor1 -> R2\n"
                "R2 -> SOURCE\n"
                "}\n";
            Circuit* circuit = createCircuitFromString(circuitStr);

            Assert::IsTrue(circuit->getError().hasError());
            Assert::AreEqual(int(ErrorType::InvalidComponentName), int(circuit->getError().type));
            delete circuit;
        }

        TEST_METHOD(InvalidName_StartsWithDigit)
        {
            string circuitStr =
                "digraph circuit {\n"
                "SOURCE [label=\"SOURCE frequency=50 voltage=100 phase=0\"]\n"
                "1R [label=\"R=10\"]\n"
                "R2 [label=\"R=20\"]\n"
                "SOURCE -> 1R\n"
                "1R -> R2\n"
                "R2 -> SOURCE\n"
                "}\n";
            Circuit* circuit = createCircuitFromString(circuitStr);

            Assert::IsTrue(circuit->getError().hasError());
            Assert::AreEqual(int(ErrorType::InvalidComponentName), int(circuit->getError().type));
            delete circuit;
        }

        TEST_METHOD(DuplicateComponent)
        {
            string circuitStr =
                "digraph circuit {\n"
                "SOURCE [label=\"SOURCE frequency=50 voltage=100 phase=0\"]\n"
                "R1 [label=\"R=10\"]\n"
                "R1 [label=\"R=20\"]\n"
                "R2 [label=\"R=20\"]\n"
                "SOURCE -> R1\n"
                "R1 -> R2\n"
                "R2 -> SOURCE\n"
                "}\n";
            Circuit* circuit = createCircuitFromString(circuitStr);
            Assert::AreEqual(int(ErrorType::DuplicateComponent), int(circuit->getError().type));
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
            bool result = circuit->validate();
            Assert::IsFalse(result);
            Assert::AreEqual(int(ErrorType::MissingSource), int(circuit->getError().type));
            delete circuit;
        }

        TEST_METHOD(MultipleSources)
        {
            string circuitStr =
                "digraph circuit {\n"
                "SOURCE1 [label=\"SOURCE frequency=50 voltage=100 phase=0\"]\n"
                "SOURCE2 [label=\"SOURCE frequency=50 voltage=100 phase=0\"]\n"
                "R1 [label=\"R=10\"]\n"
                "SOURCE1 -> R1\n"
                "R1 -> SOURCE2\n"
                "SOURCE2 -> SOURCE1\n"
                "}\n";
            Circuit* circuit = createCircuitFromString(circuitStr);
            bool result = circuit->validate();
            Assert::IsFalse(result);
            Assert::AreEqual(int(ErrorType::MultipleSources), int(circuit->getError().type));
            delete circuit;
        }

        TEST_METHOD(SourceFrequencyZero)
        {
            string circuitStr =
                "digraph circuit {\n"
                "SOURCE [label=\"SOURCE frequency=0 voltage=100 phase=0\"]\n"
                "R1 [label=\"R=10\"]\n"
                "SOURCE -> R1\n"
                "R1 -> SOURCE\n"
                "}\n";
            Circuit* circuit = createCircuitFromString(circuitStr);
            bool result = circuit->validate();
            Assert::IsFalse(result);
            Assert::AreEqual(int(ErrorType::ValueOutOfRange), int(circuit->getError().type));
            delete circuit;
        }

        TEST_METHOD(SourceFrequencyNegative)
        {
            string circuitStr =
                "digraph circuit {\n"
                "SOURCE [label=\"SOURCE frequency=-50 voltage=100 phase=0\"]\n"
                "R1 [label=\"R=10\"]\n"
                "SOURCE -> R1\n"
                "R1 -> SOURCE\n"
                "}\n";
            Circuit* circuit = createCircuitFromString(circuitStr);
            bool result = circuit->validate();
            Assert::IsFalse(result);
            Assert::AreEqual(int(ErrorType::ValueOutOfRange), int(circuit->getError().type));
            delete circuit;
        }

        TEST_METHOD(SourceVoltageNegative)
        {
            string circuitStr =
                "digraph circuit {\n"
                "SOURCE [label=\"SOURCE frequency=50 voltage=-100 phase=0\"]\n"
                "R1 [label=\"R=10\"]\n"
                "SOURCE -> R1\n"
                "R1 -> SOURCE\n"
                "}\n";
            Circuit* circuit = createCircuitFromString(circuitStr);
            bool result = circuit->validate();
            Assert::IsFalse(result);
            Assert::AreEqual(int(ErrorType::ValueOutOfRange), int(circuit->getError().type));
            delete circuit;
        }

        TEST_METHOD(SourcePhaseNegative)
        {
            string circuitStr =
                "digraph circuit {\n"
                "SOURCE [label=\"SOURCE frequency=50 voltage=100 phase=-10\"]\n"
                "R1 [label=\"R=10\"]\n"
                "SOURCE -> R1\n"
                "R1 -> SOURCE\n"
                "}\n";
            Circuit* circuit = createCircuitFromString(circuitStr);
            bool result = circuit->validate();
            Assert::IsFalse(result);
            Assert::AreEqual(int(ErrorType::PhaseOutOfRange), int(circuit->getError().type));
            delete circuit;
        }

        TEST_METHOD(SourcePhaseTooLarge)
        {
            string circuitStr =
                "digraph circuit {\n"
                "SOURCE [label=\"SOURCE frequency=50 voltage=100 phase=400\"]\n"
                "R1 [label=\"R=10\"]\n"
                "SOURCE -> R1\n"
                "R1 -> SOURCE\n"
                "}\n";
            Circuit* circuit = createCircuitFromString(circuitStr);
            bool result = circuit->validate();
            Assert::IsFalse(result);
            Assert::AreEqual(int(ErrorType::PhaseOutOfRange), int(circuit->getError().type));
            delete circuit;
        }

        TEST_METHOD(ResistorValueZero)
        {
            string circuitStr =
                "digraph circuit {\n"
                "SOURCE [label=\"SOURCE frequency=50 voltage=100 phase=0\"]\n"
                "R1 [label=\"R=0\"]\n"
                "SOURCE -> R1\n"
                "R1 -> SOURCE\n"
                "}\n";
            Circuit* circuit = createCircuitFromString(circuitStr);
            bool result = circuit->validate();
            Assert::IsFalse(result);
            Assert::AreEqual(int(ErrorType::ValueOutOfRange), int(circuit->getError().type));
            delete circuit;
        }

        TEST_METHOD(CoilValueZero)
        {
            string circuitStr =
                "digraph circuit {\n"
                "SOURCE [label=\"SOURCE frequency=50 voltage=100 phase=0\"]\n"
                "L1 [label=\"L=0\"]\n"
                "SOURCE -> L1\n"
                "L1 -> SOURCE\n"
                "}\n";
            Circuit* circuit = createCircuitFromString(circuitStr);
            bool result = circuit->validate();
            Assert::IsFalse(result);
            Assert::AreEqual(int(ErrorType::ValueOutOfRange), int(circuit->getError().type));
            delete circuit;
        }

        TEST_METHOD(CapacitorValueZero)
        {
            string circuitStr =
                "digraph circuit {\n"
                "SOURCE [label=\"SOURCE frequency=50 voltage=100 phase=0\"]\n"
                "C1 [label=\"C=0\"]\n"
                "SOURCE -> C1\n"
                "C1 -> SOURCE\n"
                "}\n";
            Circuit* circuit = createCircuitFromString(circuitStr);
            bool result = circuit->validate();
            Assert::IsFalse(result);
            Assert::AreEqual(int(ErrorType::ValueOutOfRange), int(circuit->getError().type));
            delete circuit;
        }

        TEST_METHOD(UndefinedSourceInEdge)
        {
            string circuitStr =
                "digraph circuit {\n"
                "SOURCE [label=\"SOURCE frequency=50 voltage=100 phase=0\"]\n"
                "R1 [label=\"R=10\"]\n"
                "R2 -> R1\n"
                "R1 -> SOURCE\n"
                "}\n";
            Circuit* circuit = createCircuitFromString(circuitStr);
            Assert::IsTrue(circuit->getError().hasError() || !circuit->validate());
            delete circuit;
        }

        TEST_METHOD(UndefinedDestInEdge)
        {
            string circuitStr =
                "digraph circuit {\n"
                "SOURCE [label=\"SOURCE frequency=50 voltage=100 phase=0\"]\n"
                "R1 [label=\"R=10\"]\n"
                "SOURCE -> R2\n"
                "R1 -> SOURCE\n"
                "}\n";
            Circuit* circuit = createCircuitFromString(circuitStr);
            Assert::IsTrue(circuit->getError().hasError() || !circuit->validate());
            delete circuit;
        }

        TEST_METHOD(SelfConnection)
        {
            string circuitStr =
                "digraph circuit {\n"
                "SOURCE [label=\"SOURCE frequency=50 voltage=100 phase=0\"]\n"
                "SOURCE -> SOURCE\n"
                "}\n";
            Circuit* circuit = createCircuitFromString(circuitStr);
            Assert::AreEqual(int(ErrorType::SelfConnection), int(circuit->getError().type));
            delete circuit;
        }

        TEST_METHOD(DuplicateConnection)
        {
            string circuitStr =
                "digraph circuit {\n"
                "SOURCE [label=\"SOURCE frequency=50 voltage=100 phase=0\"]\n"
                "R1 [label=\"R=10\"]\n"
                "SOURCE -> R1\n"
                "SOURCE -> R1\n"
                "R1 -> SOURCE\n"
                "}\n";
            Circuit* circuit = createCircuitFromString(circuitStr);
            bool result = circuit->validate();
            Assert::IsFalse(result);
            Assert::AreEqual(int(ErrorType::DuplicateConnection), int(circuit->getError().type));
            delete circuit;
        }

        TEST_METHOD(IsolatedComponent)
        {
            string circuitStr =
                "digraph circuit {\n"
                "SOURCE [label=\"SOURCE frequency=50 voltage=100 phase=0\"]\n"
                "R1 [label=\"R=10\"]\n"
                "R2 [label=\"R=20\"]\n"
                "SOURCE -> R1\n"
                "R1 -> SOURCE\n"
                "}\n";
            Circuit* circuit = createCircuitFromString(circuitStr);
            bool result = circuit->validate();
            Assert::IsFalse(result);
            Assert::AreEqual(int(ErrorType::IsolatedComponent), int(circuit->getError().type));
            delete circuit;
        }

        TEST_METHOD(UnreachableNode)
        {
            string circuitStr =
                "digraph circuit {\n"
                "SOURCE [label=\"SOURCE frequency=50 voltage=100 phase=0\"]\n"
                "R1 [label=\"R=10\"]\n"
                "R2 [label=\"R=20\"]\n"
                "R3 [label=\"R=30\"]\n"
                "SOURCE -> R1\n"
                "R1 -> SOURCE\n"
                "R2 -> R3\n"
                "R3 -> R2\n"
                "}\n";
            Circuit* circuit = createCircuitFromString(circuitStr);
            bool result = circuit->validate();
            Assert::IsFalse(result);
            Assert::AreEqual(int(ErrorType::CircuitNotClosed), int(circuit->getError().type));
            delete circuit;
        }

        TEST_METHOD(NoReturnPath)
        {
            string circuitStr =
                "digraph circuit {\n"
                "SOURCE [label=\"SOURCE frequency=50 voltage=100 phase=0\"]\n"
                "R1 [label=\"R=10\"]\n"
                "SOURCE -> R1\n"
                "}\n";
            Circuit* circuit = createCircuitFromString(circuitStr);
            bool result = circuit->validate();
            Assert::IsFalse(result);
            Assert::AreEqual(int(ErrorType::CircuitNotClosed), int(circuit->getError().type));
            delete circuit;
        }

        TEST_METHOD(TooManyParallelElements)
        {
            stringstream ss;
            ss << "digraph circuit {\n";
            ss << "SOURCE [label=\"SOURCE frequency=50 voltage=100 phase=0\"]\n";
            for (int i = 1; i <= 11; i++) {
                ss << "R" << i << " [label=\"R=10\"]\n";
            }
            for (int i = 1; i <= 11; i++) {
                ss << "SOURCE -> R" << i << "\n";
            }
            for (int i = 1; i <= 11; i++) {
                ss << "R" << i << " -> SOURCE\n";
            }
            ss << "}\n";
            Circuit* circuit = createCircuitFromString(ss.str());
            bool result = circuit->validate();
            Assert::IsFalse(result);
            Assert::AreEqual(int(ErrorType::TooManyParallelElements), int(circuit->getError().type));
            delete circuit;
        }

        TEST_METHOD(ValidCircuit)
        {
            string circuitStr =
                "digraph circuit {\n"
                "SOURCE [label=\"SOURCE frequency=50 voltage=100 phase=0\"]\n"
                "R1 [label=\"R=10\"]\n"
                "SOURCE -> R1\n"
                "R1 -> SOURCE\n"
                "}\n";
            Circuit* circuit = createCircuitFromString(circuitStr);
            bool result = circuit->validate();
            Assert::IsTrue(result);
            Assert::AreEqual(int(ErrorType::NoError), int(circuit->getError().type));
            delete circuit;
        }
    };
}