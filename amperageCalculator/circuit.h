 /**
  * \file circuit.h
  * \brief Класс Circuit: парсинг, валидация и расчёт электрической цепи.
  *
  * \author Anna Bezhenar
  * \date June 2026
  */

#pragma once

#include "circuitError.h"
#include <string>

using namespace std;

  /**
   * \brief Класс, представляющий электрическую цепь
   */
class Circuit {
public:
    Circuit();
    ~Circuit();

    // Основные методы
   /**
    * \brief Парсит входной файл в формате DOT и заполняет внутренние структуры цепи.
    * \param[in] filename Путь к входному файлу с описанием электрической цепи.
    * \return true, если парсинг успешен; false в случае ошибки.
    */
    bool parseFromFile(const string& filename);

    /**
     * \brief Выполняет полную валидацию электрической цепи.
     * \return true, если цепь прошла все проверки; false при обнаружении ошибки.
     */
    bool validate();

    /**
     * \brief Вычисляет комплексные сопротивления всех элементов цепи.
     * \return true, если расчёт успешен; false при обнаружении нулевой ёмкости.
     */
    bool calculateImpedances();

    /**
     * \brief Строит ветви электрической цепи на основе топологии графа.
     * \return true, если построение успешно; false, если не найден источник.
     */
    bool buildBranches();

    /**
     * \brief Рассчитывает комплексные токи во всех ветвях и узлах цепи.
     * \return true, если расчёт успешен; false при ошибке.
     */
    bool calculateCurrents();

    /**
     * \brief Выполняет полный цикл расчёта электрической цепи.
     * \return true, если расчёт успешен; false при ошибке на любом этапе.
     */
    bool calculate();

    /**
     * \brief Записывает результаты расчёта в выходной файл в формате DOT.
     * \param[in] filename Путь к выходному файлу.
     * \return true, если запись успешна; false при ошибке создания файла.
     */
    bool writeToFile(const string& filename);

    const Error& getError() const {
        return error;
    }
    
private:

    Error error;

};