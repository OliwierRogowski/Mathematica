#ifndef DERIVATIVEOFAFUNCTION_H
#define DERIVATIVEOFAFUNCTION_H
#include <QString>

class DerivativeOfAFunction
{
public:
    DerivativeOfAFunction(const QString &dirativeSyntax, const QString &dirativeValue);

    QString getFunctionDerativeSyntax() const;
    QString getFunctionDerativeValue() const;

private:
    QString functionDerativeSyntax;  // Składnia funkcji np. "F[x]"
    QString functionDerativeValue;
};

#endif
