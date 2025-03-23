#ifndef FUNCTIONOBJECT_H
#define FUNCTIONOBJECT_H

#include <QString>

class FunctionObject {
public:
    FunctionObject(const QString &syntax, const QString &value);  // Deklaracja konstruktora

    // Gettery
    QString getFunctionSyntax() const;
    QString getFunctionValue() const;

private:
    QString functionSyntax;  // Składnia funkcji np. "F[x]"
    QString functionValue;   // Wartość funkcji np. "x^3 + x + 2"
};

#endif // FUNCTIONOBJECT_H
