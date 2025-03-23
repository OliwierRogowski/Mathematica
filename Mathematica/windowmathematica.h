#ifndef WINDOWMATHEMATICA_H
#define WINDOWMATHEMATICA_H

#include <QMainWindow>
#include <QPlainTextEdit>
#include <QPushButton>
#include <QList>
#include "functionobjects.h"
#include "functionschartspoints.h"
#include <QChartView>
#include "derivativeofafunction.h"

class WindowMathematica : public QMainWindow
{
    Q_OBJECT

public:
    WindowMathematica(QWidget *parent = nullptr);
    ~WindowMathematica();

private slots:
    void CheckFunction();

private:
    void Function();
    void DerivativeFunction(QString valueFunction);
    void PaintFunction(const FunctionObject& func);

    QPlainTextEdit *MainInput;
    QPushButton *Wykonaj;

    QList<DerivativeOfAFunction> derivativeFunctions;
    QList<FunctionObject> functions;  // Lista funkcji
    QList<functionsChartsPoints> chartPoints;  // Lista punktów funkcji
    QList<QPointF> points;  // Punkty funkcji do wykresu
    double a, b, c;  // Współczynniki funkcji kwadratowej, do przykładu
    double step = 0.1;  // Krok obliczania punktów
    int liczba1 = 0;
    int liczba2 = 0;
    QChartView *previousChartView;
};

#endif // WINDOWMATHEMATICA_H
