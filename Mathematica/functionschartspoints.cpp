#ifndef FUNCTIONCHARTSPOINTS_H
#define FUNCTIONCHARTSPOINTS_H

#include <QString>
#include <QPointF>
#include <QList>
#include "functionschartspoints.h"

class functionsChartsPoints {
public:
    functionsChartsPoints(const QString &syntax, const QList<QPointF> &points)
        : functionSyntax(syntax), functionPoints(points) {}

    // Getters
    QString getFunctionSyntax() const { return functionSyntax; }
    QList<QPointF> getFunctionPoints() const { return functionPoints; }

private:
    QString functionSyntax;      // Składnia funkcji (np. F[x])
    QList<QPointF> functionPoints; // Punkty na wykresie funkcji (x, y)
};

#endif // FUNCTIONCHARTSPOINTS_H
