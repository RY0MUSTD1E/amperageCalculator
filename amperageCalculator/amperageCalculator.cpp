/**
 * \mainpage Главная страница
 * \section description Описание
 * Программа предназначена для расчёта распределения токов в электрической цепи переменного тока, 
 * содержащей один источник напряжения и некоторое количество пссивных элементов (резисторы, конденсаторы и катушки индуктивности). 
 *
 * \section input Формат входных данных
 * Входной файл с электрической цепью должен иметь расширение .txt
 * Электрическая цепь должна быть записана в формате DOT (ориентированный граф):
 * \verbatim
digraph circuit {
    SOURCE [label="SOURCE frequency=50 voltage=100 phase=0"]
    R1 [label="R=10"]
    R2 [label="R=20"]
    R3 [label="R=30"]
    SOURCE -> R1
    R1 -> R2
    R2 -> R3
    R3 -> SOURCE
}
 \endverbatim
 * 
 * \section output Формат выходных данных
 * Выходной файл содержит ту же цепь, но с рассчитанными значениями токов:
 * \verbatim
digraph circuit {
    SOURCE [label="SOURCE frequency=50 voltage=100 phase=0 I=complex<double>(1.666667, 0.000000) A"]
    R1 [label="R=10.000000 I=complex<double>(1.666667, 0.000000) A"]
    R2 [label="R=20.000000 I=complex<double>(1.666667, 0.000000) A"]
    R3 [label="R=30.000000 I=complex<double>(1.666667, 0.000000) A"]
    SOURCE -> R1
    R1 -> R2
    R2 -> R3
    R3 -> SOURCE
}
 \endverbatim
 *
 * \section example Пример запуска
 * \code
 * ../amperageCalculator input.txt output.txt
 * \endcode
 *
 * \section requirements Основные требования
 * - Частота источника должна быть положительной
 * - Фаза источника в диапазоне [0, 360] градусов
 * - Номиналы R, L, C > 0
 * - Электрическая цепь должна быть замкнута
 *
 * \author Anna Bezhenar
 * \date June 2026
 * 
 * \file amperageCalculator.cpp
 * \brief Главный файл программы для расчёта токов в электрической цепи переменного тока.
 *
 * Программа принимает два аргумента командной строки:
 * 1. Путь к входному файлу с описанием цепи в формате DOT.
 * 2. Путь к выходному файлу для записи результатов.
 *
 * \author Anna Bezhenar
 * \date June 2026
 */

#include <iostream>
#include <string>
#include "circuit.h"

using namespace std;

int main(int argc, char* argv[]) {
    setlocale(LC_ALL, "Russian");

    string inputFile;
    string outputFile;

    // Если передано корректное количество аргументов
    if (argc >= 3) {
        inputFile = argv[1];
        outputFile = argv[2];
    }
    else {
        cout << "Введите имя входного файла (input.txt): ";
        getline(cin, inputFile);
        cout << "Введите имя выходного файла (output.txt): ";
        getline(cin, outputFile);
    }

    // Создать объект Circuit
    Circuit circuit;

    // Выполнить парсинг входного файла
    if (!circuit.parseFromFile(inputFile)) {
        cout << "Ошибка: " << circuit.getError().getMessage() << endl;
        return 1;
    }
    // Выполнить расчет цепи
    if (!circuit.calculate()) {
        cout << "Ошибка: " << circuit.getError().getMessage() << endl;
        return 1;
    }
    // Записать результат в выходной файл
    if (!circuit.writeToFile(outputFile)) {
        cout << "Ошибка: " << circuit.getError().getMessage() << endl;
        return 1;
    }
    cout << "Результат записан в файл: " << outputFile << endl;

    return 0;
}