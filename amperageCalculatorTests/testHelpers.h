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

#include "CppUnitTest.h"
#include "../amperageCalculator/circuit.h"
#include "../amperageCalculator/circuitError.h"
#include "../amperageCalculator/circuitNode.h"
#include "../amperageCalculator/circuitBranch.h"

#include <string>
#include <vector>

namespace TestHelpers
{
    /**
     * \brief Создаёт объект Circuit из строки с описанием цепи в формате DOT.
     * \param[in] circuitStr Строка с описанием цепи.
     * \return Указатель на созданный объект Circuit.
     */
    Circuit* createCircuitFromString(const std::string& circuitStr);

    /**
     * \brief Проверяет соответствие узлов ветви ожидаемым именам.
     * \param[in] branch Указатель на проверяемую ветвь.
     * \param[in] expectedNames Ожидаемые имена узлов.
     */
    void VerifyBranchNodes(CircuitBranch* branch, const std::vector<std::string>& expectedNames);

    /**
     * \brief Проверяет связи ветви со следующими и предыдущими ветвями.
     * \param[in] branch Указатель на проверяемую ветвь.
     * \param[in] expectedNext Ожидаемые индексы следующих ветвей.
     * \param[in] expectedPrev Ожидаемые индексы предыдущих ветвей.
     * \param[in] allBranches Вектор всех ветвей цепи.
     */
    void VerifyConnections(CircuitBranch* branch, const std::vector<int>& expectedNext, const std::vector<int>& expectedPrev, const std::vector<CircuitBranch*>& allBranches);

    /**
     * \brief Проверяет рассчитанное значение тока в компоненте с заданной точностью.
     * \param[in] circuit Указатель на объект Circuit.
     * \param[in] nodeName Имя компонента.
     * \param[in] expectedReal Ожидаемая действительная часть.
     * \param[in] expectedImag Ожидаемая мнимая часть.
     * \param[in] tolerance Допустимая погрешность.
     */
    void VerifyAmperage(Circuit* circuit, const string& nodeName, double expectedReal, double expectedImag, double tolerance = 1e-4);
}
