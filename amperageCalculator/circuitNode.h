/**
 * \file circuitNode.h
 * \brief Заголовочный файл класса CircuitNode (элемента электрической цепи).
 *
 * \author Anna Bezhenar
 * \date June 2026
 */

#pragma once

#include <complex>
#include <vector>
#include <string>

/**
* \enum NodeType
* \brief Типы элементов электрической цепи.
*/
enum class NodeType {
    Resistor,   ///< Резистор
    Capacitor,  ///< Конденсатор
    Coil,       ///< Катушка индуктивности
    Source      ///< Источник напряжения
};

/**
* \class CircuitNode
* \brief Класс, представляющий элемент (узел) электрической цепи переменного тока.
*
* Содержит имя, тип, номинальное значение, комплексное сопротивление,
* комплексный ток, списки предыдущих и следующих узлов для построения топологии цепи.
*/
class CircuitNode {
public:
    /**
     * \brief Конструктор по умолчанию.
     */
    CircuitNode();

    /**
     * \brief Конструктор с параметрами.
     * \param[in] name Имя элемента.
     * \param[in] type Тип элемента.
     * \param[in] value Номинальное значение.
     */
    CircuitNode(const std::string& name, NodeType type, double value);

    /**
     * \brief Возвращает имя элемента.
     * \return Имя элемента.
     */
    std::string getName() const;

    /**
     * \brief Возвращает комплексное сопротивление.
     * \return Комплексное сопротивление.
     */
    std::complex<double> getResistance() const;

private:
    std::string name;                       ///< Имя элемента
    NodeType type;                          ///< Тип элемента
    std::complex<double> amperage;          ///< Комплексный ток
    std::complex<double> resistance;        ///< Комплексное сопротивление
    double voltage;                         ///< Напряжение
    double frequency;                       ///< Частота
    double phase;                           ///< Фаза
    double value;                           ///< Номинальное значение
    std::string originalLabel;              ///< Оригинальная строка label
    std::vector<CircuitNode*> previousNodes; ///< Предыдущие узлы
    std::vector<CircuitNode*> nextNodes;     ///< Следующие узлы
};