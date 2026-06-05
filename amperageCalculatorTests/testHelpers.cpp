/**
 * \file testHelpers.cpp
 * \brief Вспомогательные функции и подключения для всех тестов amperageCalculator.
 * 
 * \author Anna Bezhenar
 * \date May 2026
 */
#include "pch.h"

#include "TestHelpers.h"
#include "../amperageCalculator/circuit.h"
#include <string>

using namespace std;

namespace TestHelpers
{
    Circuit* createCircuitFromString(const string& circuitStr) {
        Circuit* circuit = new Circuit();
        return circuit;
    }

}