 /**
  * \file circuit.h
  * \brief Класс Circuit: парсинг, валидация и расчёт электрической цепи.
  *
  * \author Anna Bezhenar
  * \date June 2026
  */

#pragma once

#include "circuitError.h"
#include "circuitNode.h"
#include "circuitBranch.h"
#include <string>
#include <vector>
#include <map>
#include <unordered_map>

using namespace std;

  /**
   * \brief Класс, представляющий электрическую цепь
   */
class Circuit {
public:

    /**
     * \brief Конструктор по умолчанию.
     */
    Circuit();

    /**
     * \brief Деструктор.
     */
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
    const vector<CircuitBranch*>& getBranches() const {
        return branches;
    }

    // Вспомогательные методы парсинга
   /**
    * \brief Удаляет пробельные символы в начале и конце строки.
    * \param[in] s Исходная строка.
    * \return Строка без начальных и конечных пробелов, табуляций, перевода строки.
    */
    string trim(const string& s);

    /**
     * \brief Проверяет корректность имени компонента.
     * \param[in] name Имя компонента для проверки.
     * \return true, если имя корректно; false в противном случае.
     */
    bool isValidName(const string& name);

    /**
     * \brief Определяет тип элемента по содержимому label и вызывает соответствующий парсер.
     * \param[in] labelContent Строка содержимого label.
     * \param[out] params Структура ParamsOfNode для записи распарсенных параметров.
     * \param[in] lineNum Номер строки в исходном файле (для сообщений об ошибках).
     * \return true, если парсинг успешен; false в противном случае.
     */
    bool parseLabel(const string& labelContent, ParamsOfNode& params, int lineNum);

    /**
     * \brief Парсит содержимое label для элемента типа SOURCE.
     * \param[in] content Строка содержимого label.
     * \param[out] params Структура для записи распарсенных параметров.
     * \return true, если парсинг успешен; false в противном случае.
     */
    bool parseSourceLabel(const string& content, ParamsOfNode& params);

    /**
     * \brief Парсит содержимое label для пассивного элемента (R, L, C).
     * \param content Строка содержимого label (например, "R=100", "L=0.1", "C=100e-6").
     * \param[out] params Структура ParamsOfNode для записи распарсенных параметров.
     * \return true, если парсинг успешен; false в противном случае.
     */
    bool parseElementLabel(const string& content, ParamsOfNode& params);

    /**
     * \brief Преобразует строку в число с плавающей точкой.
     * \param[in] s Исходная строка.
     * \param[out] value Переменная для записи результата.
     * \return true, если строка содержит корректное число; false в противном случае.
     */
    bool parseDouble(const string& s, double& value);

    /**
     * \brief Проверяет корректность экспоненциального формата числа.
     * \param[in] s Строка, предположительно содержащая число в экспоненциальной форме.
     * \return true, если формат корректен; false в противном случае.
     */
    bool validateExponentialFormat(const string& s);

    // Для тестов
    /**
     * \brief Находит узел цепи по его имени.
     * \param[in] name Имя искомого узла.
     * \return Указатель на найденный узел CircuitNode; nullptr, если узел не найден.
     */
    CircuitNode* getNodeByName(const string& name) const;
    size_t getNodeCount() const {
        return nodes.size();
    }
    size_t getEdgeCount() const {
        return edges.size();
    }

private:

    // Поля класса
    double frequency;
    double sourceVoltage;
    double sourcePhase;
    vector<CircuitNode*> nodes;
    vector<pair<string, string>> edges;
    vector<CircuitBranch*> branches;
    Error error;

    unordered_map<string, CircuitNode*> nameToNode;
    unordered_map<string, size_t> nameToIndex;
    vector<string> nodeDefinitions;
};