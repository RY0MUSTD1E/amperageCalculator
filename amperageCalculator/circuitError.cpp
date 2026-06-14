/**
 * \file circuitError.cpp
 * \brief Реализация класса Error.
 *
 * \author Anna Bezhenar
 * \date June 2026
 */
#include "circuitError.h"

Error::Error()
    : type(ErrorType::NoError)
    , line(0)
{
}

void Error::setError(ErrorType t, const std::string& compName, int ln, const std::string& detail) {
    // Сохранение всей доступной информаци об ошибке
    type = t;
    componentName = compName;
    line = ln;
}

bool Error::hasError() const {
    // Ошибка считается установленной, если её тип отличается от NoError
    return type != ErrorType::NoError;
}

std::string Error::getMessage() const {
    std::string prefix;

    if (line > 0) {
        prefix = "Строка " + std::to_string(line) + ". ";
    }

    switch (type) {
    case ErrorType::InputFileNotExist:
        return "Неверно указан файл с входными данными. Возможно, файл не существует.";
    case ErrorType::OutputFileCreateFail:
        return "Неверно указан файл для выходных данных. Возможно, указанного расположения не существует или нет прав на запись.";
    case ErrorType::FileEmpty:
        return "Неверно указан файл с входными данными. Файл пуст.";
    case ErrorType::MissingDigraph:
        return "Неверный формат входного файла. Ожидается 'digraph' в начале файла.";
    case ErrorType::MissingOpenBrace:
        return "Неверный формат входного файла. Отсутствует открывающая фигурная скобка '{'.";
    case ErrorType::MissingCloseBrace:
        return "Неверный формат входного файла. Отсутствует закрывающая фигурная скобка '}'.";
    case ErrorType::InvalidConnectionSyntax:
        return prefix + "Не соответствует формату записи связи.";
    case ErrorType::InvalidComponentName:
        return prefix + "Некорректное имя компонента \"" + componentName + "\". Имя должно содержать только буквы и цифры и быть не более 7 символов.";
    case ErrorType::DuplicateComponent:
        return prefix + "Компонент с таким именем уже существует. Компонент \"" + componentName + "\" уже был определен ранее.";
    case ErrorType::MissingLabelAttribute:
        return prefix + "Отсутствует описание компонента. У компонента отсутствует атрибут label.";
    case ErrorType::EmptyLabel:
        return prefix + "Пустое описание компонента. Пустой label у компонента \"" + componentName + "\".";
    case ErrorType::UnknownComponentType:
        return prefix + "Неизвестный тип компонента. Ожидается SOURCE, R, L или C.";
    case ErrorType::IncompleteSource:
        return prefix + "Неполное описание источника. Ожидается: SOURCE frequency=<значение> voltage=<значение> phase=<значение>.";
    case ErrorType::MissingComponentValue:
        return prefix + "Неполное описание компонента \"" + componentName + "\".";
    case ErrorType::InvalidNumberFormat:
        return prefix + "Параметр компонента \"" + componentName + "\" не является корректным числом.";
    case ErrorType::PhaseOutOfRange:
        return "Параметр компонента \"" + componentName + "\" вне допустимого диапазона. Фаза должна находиться в диапазоне [0, 360].";
    case ErrorType::InvalidExponentialFormat:
        return "Параметр компонента \"" + componentName + "\" в экспоненциальной форме записан некорректно.";
    case ErrorType::UndefinedComponent:
        return "Неопределенный компонент. Компонент \"" + componentName + "\" используется в соединении, но не был описан.";
    case ErrorType::CircuitNotClosed:
        return "Цепь не замкнута. Необходимо, чтобы существовал путь от источника через все элементы обратно к источнику, следуя по направлению стрелок.";
    case ErrorType::IsolatedComponent:
        return "Нарушение замкнутости цепи. Обнаружен изолированный компонент \"" + componentName + "\".";
    case ErrorType::DuplicateConnection:
        return "В цепи не может содержаться дублирующееся соединение.";
    case ErrorType::SelfConnection:
        return prefix + "Некорректное соединение. Cоединение компонента \"" + componentName + "\" с самим собой недопустимо.";
    case ErrorType::ValueOutOfRange:
        return "Значение параметра компонента \"" + componentName + "\" находится вне допустимого диапазона.";
    case ErrorType::NoConnections:
        return "Цепь не содержит ни одного соединения между компонентами.";
    case ErrorType::MissingSource:
        return "Отсутствует источник тока. Цепь должна содержать ровно один элемент типа SOURCE.";
    case ErrorType::MultipleSources:
        return "В цепи не может быть более одного источника.";
    case ErrorType::TooManyParallelElements:
        return "В цепи должно быть не более 10 взаимно-параллельных элементов.";
    case ErrorType::TooManyElements:
        return "Граф задан некорректно. В цепи не может быть более 100 элементов.";
    case ErrorType::TooManyEdges:
        return "Граф задан некорректно. В цепи не может быть более 100 связей.";
    case ErrorType::InvalidCircuitTopology:
        return "Некорректная топология цепи.";
    default:
        return "Неизвестная ошибка.";
    }
}

void Error::clear() {
    // Возврат объекта в исходное состояние
    type = ErrorType::NoError;
    componentName.clear();
    line = 0;
}