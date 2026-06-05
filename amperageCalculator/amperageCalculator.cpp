/**
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