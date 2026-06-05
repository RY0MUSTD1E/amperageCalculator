/**
 * \file testHelpers.h
 * \brief Вспомогательные функции для модульных тестов amperageCalculator.
 *
 * Содержит объявления функций для создания цепи из строки и проверки структуры ветвей.
 *
 * \author Anna Bezhenar
 * \date June 2026
 */
#pragma once

#include "../amperageCalculator/circuit.h"

#include <string>
#include <vector>

namespace TestHelpers
{
    /**
     * \brief Создаёт объект Circuit из строки с описанием цепи в формате DOT.
     * \param circuitStr Строка с описанием цепи.
     * \return Указатель на созданный объект Circuit.
     */
    Circuit* createCircuitFromString(const std::string& circuitStr);
}
