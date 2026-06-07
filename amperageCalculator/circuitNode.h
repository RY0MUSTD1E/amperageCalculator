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
 * \struct ParamsOfNode
 * \brief Вспомогательная структура для хранения распарсенных параметров элемента.
 *
 * Используется при парсинге DOT-файла для временного хранения данных
 * перед созданием объектов CircuitNode.
 */
struct ParamsOfNode {
    std::string name;           ///< Имя компонента
    NodeType type;              ///< Тип компонента
    double nominal;             ///< Номинальное значение
    double frequency;           ///< Частота источника (только для Source)
    double voltage;             ///< Напряжение источника (только для Source)
    double phase;               ///< Фаза источника (только для Source)
    std::string originalLabel;  ///< Оригинальная строка label

    /**
     * \brief Конструктор по умолчанию.
     */
    ParamsOfNode()
        : type(NodeType::Resistor)
        , nominal(0.0)
        , frequency(0.0)
        , voltage(0.0)
        , phase(0.0)
    {
    }
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

    //Сеттеры

    /**
     * \brief Устанавливает имя элемента.
     * \param[in] n Новое имя.
     */
    void setName(const std::string& n);

    /**
     * \brief Устанавливает тип элемента.
     * \param[in] t Новый тип.
     */
    void setType(NodeType t);

    /**
     * \brief Устанавливает комплексный ток.
     * \param[in] a Комплексное значение тока.
     */
    void setAmperage(std::complex<double> a);

    /**
     * \brief Устанавливает комплексное сопротивление.
     * \param[in] r Комплексное значение сопротивления.
     */
    void setResistance(std::complex<double> r);

    /**
     * \brief Устанавливает напряжение.
     * \param[in] v Значение напряжения.
     */
    void setVoltage(double v);

    /**
     * \brief Устанавливает частоту.
     * \param[in] f Значение частоты.
     */
    void setFrequency(double f);

    /**
     * \brief Устанавливает фазу.
     * \param[in] p Значение фазы.
     */
    void setPhase(double p);

    /**
     * \brief Устанавливает номинальное значение.
     * \param[in] v Номинальное значение.
     */
    void setValue(double v);

    /**
     * \brief Устанавливает оригинальную строку label.
     * \param[in] l Строка оригинального label.
     */
    void setOriginalLabel(const std::string& l);

    // Геттеры

    /**
     * \brief Возвращает имя элемента.
     * \return Имя элемента.
     */
    std::string getName() const;

    /**
     * \brief Возвращает тип элемента.
     * \return Тип элемента.
     */
    NodeType getType() const;

    /**
     * \brief Возвращает комплексный ток.
     * \return Комплексный ток.
     */
    std::complex<double> getAmperage() const;

    /**
     * \brief Возвращает комплексное сопротивление.
     * \return Комплексное сопротивление.
     */
    std::complex<double> getResistance() const;

    /**
     * \brief Возвращает напряжение.
     * \return Напряжение.
     */
    double getVoltage() const;

    /**
     * \brief Возвращает частоту.
     * \return Частота.
     */
    double getFrequency() const;

    /**
     * \brief Возвращает фазу.
     * \return Фаза.
     */
    double getPhase() const;

    /**
     * \brief Возвращает номинальное значение.
     * \return Номинальное значение.
     */
    double getValue() const;

    /**
     * \brief Возвращает оригинальную строку label.
     * \return Оригинальный label.
     */
    std::string getOriginalLabel() const;

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