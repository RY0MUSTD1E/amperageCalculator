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
#include <queue>
#include <map>
#include <unordered_map>
#include <unordered_set>
#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/graphviz.hpp>
#include <boost/graph/breadth_first_search.hpp>

constexpr long double pi = 3.14159265358979323846;

using namespace std;

/**
 * \struct VertexProperties
 * \brief Свойства вершины графа Boost.
 *
 * Используется для хранения имени узла и указателя на соответствующий
 * объект CircuitNode при построении Boost-графа.
 */
struct VertexProperties {
    string name;          ///< Имя вершины
    CircuitNode* nodePtr; ///< Указатель на соответствующий объект CircuitNode
};

/**
 * \struct EdgeProperties
 * \brief Свойства ребра графа Boost.
 *
 * Хранит имена начальной и конечной вершин для ребра графа.
 */
struct EdgeProperties {
    string from; ///< Имя начальной вершины
    string to;   ///< Имя конечной вершины
};

/// Граф электрической цепи на основе Boost.Graph.
typedef boost::adjacency_list<
    boost::vecS,           // Контейнер для вершин
    boost::vecS,           // Контейнер для рёбер
    boost::directedS,      // Ориентированный граф
    VertexProperties,      // Свойства вершин
    EdgeProperties         // Свойства рёбер
> BoostGraph;

/// Дескриптор вершины графа Boost.
typedef boost::graph_traits<BoostGraph>::vertex_descriptor BoostVertex;

/// Дескриптор ребра графа Boost.
typedef boost::graph_traits<BoostGraph>::edge_descriptor BoostEdge;

/**
 * \brief Класс, представляющий электрическую цепь.
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

    /**
     * \brief Возвращает информацию о последней ошибке.
     * \return Объект Error, содержащий сведения о возникшей ошибке.
     */
    const Error& getError() const {
        return error;
    }
    /**
     * \brief Возвращает список ветвей электрической цепи.
     * \return Константная ссылка на вектор указателей на объекты CircuitBranch.
     */
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
     * \param[in] content Строка содержимого label (например, "R=100", "L=0.1", "C=100e-6").
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

    // Вспомогательные методы валидации

    /**
     * \brief Проверяет корректность имён всех узлов цепи.
     * \return true, если все имена корректны; false при обнаружении некорректного имени.
     */
    bool validateNames();

    /**
     * \brief Проверяет, что все имена компонентов уникальны.
     * \return true, если дубликатов нет; false при обнаружении повторяющегося имени.
     */
    bool validateUniqueness();

    /**
     * \brief Проверяет наличие и корректность параметров источника напряжения.
     * \return true, если источник корректен; false при наличии ошибок.
     */
    bool validateSource();

    /**
     * \brief Проверяет, что номинальные значения пассивных элементов положительны.
     * \return true, если все значения корректны; false при обнаружении нулевого или отрицательного значения.
     */
    bool validateValues();

    /**
     * \brief Проверяет корректность всех рёбер (связей) в цепи.
     * \return true, если все рёбра корректны; false при обнаружении ошибки.
     */
    bool validateEdges();

    /**
     * \brief Проверяет, что все узлы цепи имеют хотя бы одну связь.
     * \return true, если изолированных узлов нет; false при обнаружении изолированного компонента.
     */
    bool validateConnectivity();

    /**
     * \brief Проверяет замкнутость цепи: достижимость всех узлов из источника и обратно.
     * \return true, если цепь замкнута; false в противном случае.
     */
    bool validateClosure();

    /**
     * \brief Проверяет, что количество взаимно-параллельных элементов не превышает 10.
     * \return true, если ограничение соблюдено; false при превышении.
     */
    bool validateParallelCount();


    // Методы для работы с Boost.Graph

    /**
     * \brief Строит граф библиотеки Boost из внутреннего представления цепи.
     * \return Объект BoostGraph, содержащий вершины и рёбра цепи.
     */
    BoostGraph buildBoostGraph();

    /**
     * \class BFSReachabilityVisitor
     * \brief Вспомогательный класс-посетитель для алгоритма поиска в ширину (BFS) из библиотеки Boost.
     * Наследуется от boost::default_bfs_visitor и переопределяет метод
     * discover_vertex() для отслеживания посещённых вершин в процессе обхода графа.
     */
    class BFSReachabilityVisitor : public boost::default_bfs_visitor {
    public:
        vector<bool>& visited; ///< Вектор, содержащий информацию о посещённых вершинах.

        /**
         * \brief Конструктор посетителя BFS.
         * \param[in] v Массив отметок о посещении вершин.
         */
        BFSReachabilityVisitor(vector<bool>& v) : visited(v) {}

        /**
         * \brief Вызывается при обнаружении новой вершины в процессе обхода графа.
         * \tparam Vertex Тип дескриптора вершины графа.
         * \tparam Graph Тип графа Boost.
         * \param[in] u Обнаруженная вершина.
         * \param[in] g Граф, в котором выполняется обход.
         */
        template<typename Vertex, typename Graph>
        void discover_vertex(Vertex u, const Graph& g) {
            visited[u] = true;
        }
    };

    /**
     * \brief Проверяет, достижимы ли все вершины графа из заданной начальной вершины.
     * \param[in] graph Граф библиотеки Boost.
     * \param[in] startVertex Дескриптор начальной вершины (источник).
     * \return true, если все вершины достижимы; false, если есть недостижимые вершины.
     * \details Использует алгоритм поиска в ширину (BFS) с посетителем BFSReachabilityVisitor
     * для обхода графа и отметки посещённых вершин.
     */
    bool checkReachability(const BoostGraph& graph, BoostVertex startVertex);

    /**
     * \brief Проверяет, что из каждой вершины графа существует путь обратно к источнику.
     * \param[in] graph Граф библиотеки Boost.
     * \param[in] startVertex Дескриптор вершины-источника.
     * \return true, если из всех вершин можно достичь источника; false в противном случае.
     * \details Для каждой вершины графа (кроме источника) запускает BFS и проверяет,
     * входит ли источник в множество достижимых вершин.
     */
    bool checkReverseReachability(const BoostGraph& graph, BoostVertex startVertex);

    // Вспомогательные методы расчета токов

    /**
     * \brief Распределяет ток от текущей ветви к следующим.
     * \param[in,out] branch Указатель на текущую ветвь.
     */
    void distributeCurrentsToBranches(CircuitBranch* branch);

    /**
     * \brief Вычисляет эквивалентное сопротивление цепи от заданной ветви.
     * \param[in] branch Указатель на начальную ветвь.
     * \return Комплексное эквивалентное сопротивление всей последующей цепи.
     */
    complex<double> calcTotalResistance(CircuitBranch* branch);

    // Для тестов

    /**
     * \brief Находит узел цепи по его имени.
     * \param[in] name Имя искомого узла.
     * \return Указатель на найденный узел CircuitNode; nullptr, если узел не найден.
     */
    CircuitNode* getNodeByName(const string& name) const;

    /**
     * \brief Возвращает количество узлов цепи.
     * \return Количество узлов цепи.
     */
    size_t getNodeCount() const {
        return nodes.size();
    }

    /**
     * \brief Возвращает количество связей цепи.
     * \return Количество связей между узлами цепи.
     */
    size_t getEdgeCount() const {
        return edges.size();
    }

private:

    // Поля класса
    double frequency;                    ///< Частота источника (Гц)
    double sourceVoltage;                ///< Амплитуда напряжения источника (В)
    double sourcePhase;                  ///< Начальная фаза источника (градусы)
    vector<CircuitNode*> nodes;          ///< Список всех узлов цепи
    vector<pair<string, string>> edges;  ///< Список связей между узлами
    vector<CircuitBranch*> branches;     ///< Список ветвей цепи
    Error error;                         ///< Объект для хранения ошибок

    unordered_map<string, CircuitNode*> nameToNode; ///< Отображение имени компонента в соответствующий объект CircuitNode.
    unordered_map<string, size_t> nameToIndex;      ///< Отображение имени компонента в индекс вершины графа.
    vector<string> nodeDefinitions;                 ///< Исходные описания компонентов, считанные из входного файла.
};