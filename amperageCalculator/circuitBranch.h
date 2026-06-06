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
 * Ветвь содержит последовательность узлов, по которым течёт одинаковый ток.
 * Используется для построения дерева ветвей и расчёта токов в цепи переменного тока.
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

    /**
     * \brief Устанавливает комплексный ток ветви.
     * \param[in] a Комплексное значение тока.
     */
    void setAmperage(std::complex<double> a);

    /**
     * \brief Возвращает комплексный ток ветви.
     * \return Комплексный ток.
     */
    std::complex<double> getAmperage() const;

    /**
     * \brief Устанавливает эквивалентное сопротивление ветви.
     * \param[in] r Комплексное сопротивление.
     */
    void setEqResistance(std::complex<double> r);

    /**
     * \brief Возвращает эквивалентное сопротивление ветви.
     * \return Комплексное сопротивление.
     */
    std::complex<double> getEqResistance() const;

    /**
     * \brief Добавляет узел в ветвь.
     * \param[in] node Указатель на добавляемый узел.
     */
    void addNode(CircuitNode* node);

    /**
     * \brief Возвращает константную ссылку на вектор узлов ветви.
     * \return Вектор указателей на узлы.
     */
    const std::vector<CircuitNode*>& getNodes() const;

    /**
     * \brief Устанавливает узлы ветви.
     * \param[in] n Вектор указателей на узлы.
     */
    void setNodes(const std::vector<CircuitNode*>& n);

    /**
     * \brief Проверяет, была ли ветвь посещена при обходе графа.
     * \return true, если ветвь посещена.
     */
    bool isVisited() const;

    /**
     * \brief Устанавливает флаг посещения ветви.
     * \param[in] v Значение флага.
     */
    void setVisited(bool v);

    /**
     * \brief Добавляет предыдущую ветвь с автоматическим обратным связыванием.
     * \param[in] branch Указатель на предыдущую ветвь.
     */
    void addPrevBranch(CircuitBranch* branch);

    /**
     * \brief Добавляет следующую ветвь с автоматическим обратным связыванием.
     * \param[in] branch Указатель на следующую ветвь.
     */
    void addNextBranch(CircuitBranch* branch);

    /**
     * \brief Удаляет связь с предыдущей ветвью (двусторонне).
     * \param[in] branch Указатель на удаляемую предыдущую ветвь.
     */
    void removePrevBranch(CircuitBranch* branch);

    /**
     * \brief Удаляет связь со следующей ветвью (двусторонне).
     * \param[in] branch Указатель на удаляемую следующую ветвь.
     */
    void removeNextBranch(CircuitBranch* branch);

    /**
     * \brief Возвращает список предыдущих ветвей.
     * \return Константная ссылка на вектор предыдущих ветвей.
     */
    const std::vector<CircuitBranch*>& getPrevBranches() const;

    /**
     * \brief Возвращает список следующих ветвей.
     * \return Константная ссылка на вектор следующих ветвей.
     */
    const std::vector<CircuitBranch*>& getNextBranches() const;

    /**
     * \brief Устанавливает список предыдущих ветвей.
     * \param[in] branches Вектор указателей на предыдущие ветви.
     */
    void setPrevBranches(const std::vector<CircuitBranch*>& branches);

    /**
     * \brief Устанавливает список следующих ветвей.
     * \param[in] branches Вектор указателей на следующие ветви.
     */
    void setNextBranches(const std::vector<CircuitBranch*>& branches);

    /**
     * \brief Вычисляет эквивалентное сопротивление как сумму сопротивлений узлов.
     */
    void calculateEqResistance();

private:
    std::complex<double> amperage;               ///< Комплексный ток ветви
    std::complex<double> eqResistance;           ///< Эквивалентное сопротивление ветви
    bool visited;                                ///< Флаг посещения при обходе
    std::vector<CircuitNode*> nodes;             ///< Узлы, входящие в ветвь
    std::vector<CircuitBranch*> previousBranches; ///< Предыдущие ветви
    std::vector<CircuitBranch*> nextBranches;     ///< Следующие ветви
};