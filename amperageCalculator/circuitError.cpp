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
    invalidValue = detail;
}

string Error::getMessage() const {
    return "Not implemented yet";
}

bool Error::hasError() const {
    // Ошибка считается установленной, если её тип отличается от NoError
    return type != ErrorType::NoError;
}

void Error::clear() {
    // Возврат объекта в исходное состояние
    type = ErrorType::NoError;
    componentName.clear();
    line = 0;
    invalidValue.clear();
}