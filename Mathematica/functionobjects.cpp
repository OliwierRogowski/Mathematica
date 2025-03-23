#include "functionobjects.h"

// Definicja konstruktora
FunctionObject::FunctionObject(const QString &syntax, const QString &value)
    : functionSyntax(syntax), functionValue(value) {}

// Definicje getterów
QString FunctionObject::getFunctionSyntax() const {
    return functionSyntax;
}

QString FunctionObject::getFunctionValue() const {
    return functionValue;
}
