/**
 * \file circuitError.h
 * \brief Заголовочный файл с классами для обработки ошибок и вспомогательными структурами.
 *
 * Содержит перечисления типов ошибок и элементов цепи, класс Error для обработки ошибок
 *
 * \author Anna Bezhenar
 * \date June 2026
 */

#pragma once
#include <string>

using namespace std;

 /**
  * \enum ErrorType
  * \brief Типы ошибок, возникающих при парсинге и валидации цепи.
  */
enum class ErrorType {
    NoError,                    ///< Ошибок нет
    InputFileNotExist,          ///< Входной файл не существует
    OutputFileCreateFail,       ///< Ошибка создания выходного файла
    FileEmpty,                  ///< Входной файл пуст
    MissingDigraph,             ///< Отсутствует ключевое слово 'digraph'
    MissingOpenBrace,           ///< Отсутствует открывающая фигурная скобка '{'
    MissingCloseBrace,          ///< Отсутствует закрывающая фигурная скобка '}'
    InvalidConnectionSyntax,    ///< Некорректный синтаксис соединения
    InvalidComponentName,       ///< Некорректное имя компонента
    DuplicateComponent,         ///< Дублирование компонента
    MissingLabelAttribute,      ///< Отсутствует атрибут label
    EmptyLabel,                 ///< Пустой label
    UnknownComponentType,       ///< Неизвестный тип компонента
    IncompleteSource,           ///< Неполное описание источника
    MissingComponentValue,      ///< Отсутствует значение компонента
    InvalidNumberFormat,        ///< Некорректный числовой формат
    ValueOutOfRange,            ///< Значение вне допустимого диапазона
    PhaseOutOfRange,            ///< Фаза вне допустимого диапазона [0, 360]
    InvalidExponentialFormat,   ///< Некорректный экспоненциальный формат
    UndefinedComponent,         ///< Неопределённый компонент
    CircuitNotClosed,           ///< Цепь не замкнута
    IsolatedComponent,          ///< Изолированный компонент
    SelfConnection,             ///< Соединение компонента с самим собой
    DuplicateConnection,        ///< Дублирующееся соединение
    NoConnections,              ///< Отсутствуют соединения
    NoComponents,               ///< Отсутствуют компоненты
    MissingSource,              ///< Отсутствует источник
    MultipleSources,            ///< Несколько источников в цепи
    TooManyParallelElements,    ///< Слишком много параллельных элементов
    TooManyElements,            ///< Слишком много элементов
    TooManyEdges,               ///< Слишком много связей
    InvalidCircuitTopology      ///< Некорректная топология цепи
};

/**
 * \class Error
 * \brief Класс для хранения и обработки ошибок.
 *
 * Содержит информацию о типе ошибки, имени связанного компонента,
 * номере строки и дополнительных деталях ошибки.
 * Предоставляет методы для установки, сброса и получения информации об ошибке.
 */
class Error {
public:
    /**
     * \brief Конструктор по умолчанию.
     */
    Error();

    /**
     * \brief Устанавливает параметры ошибки.
     * \param[in] t Тип ошибки.
     * \param[in] compName Имя связанного компонента.
     * \param[in] ln Номер строки с ошибкой.
     * \param[in] detail Детали некорректного значения.
     */
    void setError(ErrorType t, const std::string& compName = "", int ln = 0, const std::string& detail = "");

    /**
     * \brief Проверяет, была ли зафиксирована ошибка.
     * \return true, если ошибка есть.
     */
    bool hasError() const;

    /**
     * \brief Возвращает текстовое сообщение об ошибке.
     * \return Строка с описанием ошибки.
     */
    std::string getMessage() const;

    /**
     * \brief Очищает информацию об ошибке.
     */
    void clear();

    ErrorType type;                 ///< Тип ошибки
    std::string componentName;      ///< Имя связанного компонента
    int line;                       ///< Номер строки
};