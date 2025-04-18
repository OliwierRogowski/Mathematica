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
#include <QLabel>
#include <QLineEdit>
#include <QVBoxLayout>
#include "DynamicInput.h"

class DynamicInput;

class WindowMathematica : public QMainWindow
{
    Q_OBJECT

public:
    WindowMathematica(QWidget *parent = nullptr);
    ~WindowMathematica();

private slots:
    void CheckFunction();

protected:
    //void keyPressEvent(QKeyEvent *event) override;

private:
    void Function();
    void DerivativeFunction(QString valueFunction);
    void PaintFunction(const FunctionObject& func);
    void PaintFunctionWithName(const FunctionObject& func);
    void AddLabelAndInput(const QString &labelText, bool success);
    void AddNewInputLine();
    void Solve(QString leftSide, QString rightSide);
    double solveEquation(QString equation, double start, double end, double tolerance = 1e-6, int maxIterations = 100);

    QList<QLineEdit*> dynamicInputs;


    DynamicInput* dynamicInput;

    QVBoxLayout *layout;  // Layout dla widgetów
    QWidget *centralWidget; // Centralny widget

    QList<QLineEdit*> inputs;

    QLineEdit *MainInput;
    QList<DerivativeOfAFunction> derivativeFunctions;
    QList<FunctionObject> functions;  // Lista funkcji
    QList<functionsChartsPoints> chartPoints;  // Lista punktów funkcji
    QList<QPointF> points;  // Punkty funkcji do wykresu
    double a, b, c;  // Współczynniki funkcji kwadratowej, do przykładu
    double step = 0.1;  // Krok obliczania punktów
    int liczba1;
    int liczba2;
    QString nameX;
    QString nameY;
    QString LeftSide;   // Lewa strona równania
    QString RightSide;
    QChar litera;
    QChartView *previousChartView;
    QGraphicsView *graphicsView;  // Widok grafiki dla wykresu
    QGraphicsScene *scene;
};

#endif // WINDOWMATHEMATICA_H
