#ifndef WINDOWMATHEMATICA_H
#define WINDOWMATHEMATICA_H

#include <QMainWindow>
#include <QPlainTextEdit>
#include <QPushButton>
#include <QList>
#include "functionobjects.h"
#include "functionschartspoints.h"
#include <QChartView>
#include <QOpenGLWidget>
#include <QOpenGLFunctions>

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
    void PaintFunction(const FunctionObject& func);

    QPlainTextEdit *MainInput;
    QPushButton *Wykonaj;

    QList<FunctionObject> functions;  // Lista funkcji
    QList<functionsChartsPoints> chartPoints;  // Lista punktów funkcji
    QList<QPointF> points;  // Punkty funkcji do wykresu
    double a, b, c;  // Współczynniki funkcji kwadratowej, do przykładu
    double step = 0.1;  // Krok obliczania punktów
<<<<<<< Updated upstream
=======
    int liczba1;
    int liczba2;
    QChar litera;
>>>>>>> Stashed changes
    QChartView *previousChartView;
};

#endif // WINDOWMATHEMATICA_H
