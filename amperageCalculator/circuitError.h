/**
 * \file circuitError.h
 * \brief Заголовочный файл с классами для обработки ошибок и вспомогательными структурами.
 *
 * Содержит перечисления типов ошибок и элементов цепи, класс Error для
 * обработки ошибок, а также структуру ParamsOfNode для временного хранения распарсенных параметров компонента.
 *
 * \author Anna Bezhenar
 * \date June 2026
 */

#pragma once

#include <string>

using namespace std;

/**
 * \class Error
 * \brief Класс для хранения и обработки ошибок.
 */
class Error {
public:
    /**
     * \brief Конструктор по умолчанию.
     */
    Error();

    /**
     * \brief Возвращает сообщение об ошибке.
     * \return Строка с сообщением об ошибке.
     */
    string getMessage() const;
};