/**
 * \file circuitBranch.h
 * \brief Заголовочный файл класса CircuitBranch (ветви электрической цепи).
 *
 * \author Anna Bezhenar
 * \date June 2026
 */
#pragma once

#include "circuitNode.h"
#include <complex>
#include <vector>

  /**
   * \class CircuitBranch
   * \brief Класс, представляющий ветвь электрической цепи.
   *
   * Ветвь содержит последовательность узлов, по которым течёт одинаковый ток.
   * Используется для построения дерева ветвей и расчёта токов в цепи
   * переменного тока.
   */
class CircuitBranch {
public:
    /**
     * \brief Конструктор по умолчанию.
     */
    CircuitBranch();

    /**
     * \brief Деструктор.
     */
    ~CircuitBranch();


private:
    std::complex<double> amperage;               ///< Комплексный ток ветви
    std::complex<double> eqResistance;           ///< Эквивалентное сопротивление ветви
    bool visited;                                ///< Флаг посещения при обходе
    std::vector<CircuitNode*> nodes;             ///< Узлы, входящие в ветвь
    std::vector<CircuitBranch*> previousBranches; ///< Предыдущие ветви
    std::vector<CircuitBranch*> nextBranches;     ///< Следующие ветви
};