#include "derivativeofafunction.h"

DerivativeOfAFunction::DerivativeOfAFunction(const QString &dirativeSyntax, const QString &dirativeValue)
    : functionDerativeSyntax(dirativeSyntax), functionDerativeValue(dirativeValue) {}


QString DerivativeOfAFunction::getFunctionDerativeSyntax() const {
    return functionDerativeSyntax;
}
QString DerivativeOfAFunction::getFunctionDerativeValue() const {
    return functionDerativeValue;
}

