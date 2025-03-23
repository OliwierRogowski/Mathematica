#include "windowmathematica.h"
#include "functionobjects.h"
#include "functionschartspoints.h"
#include <QVBoxLayout>
#include <QChartView>
#include <QLineSeries>
#include <QChart>
#include <QRegularExpression>
#include <QDebug>
#include <QJSEngine>
#include <cmath>
#include <QRegularExpression>

WindowMathematica::WindowMathematica(QWidget *parent)
    : QMainWindow(parent),
    previousChartView(nullptr)  // Inicjalizacja zmiennej do przechowywania poprzedniego wykresu
{
    // Tworzenie widgetów
    MainInput = new QPlainTextEdit(this);
    Wykonaj = new QPushButton("Wykonaj", this);

    // Tworzenie layoutu aplikacji
    QVBoxLayout *layout = new QVBoxLayout();
    layout->addWidget(MainInput);
    layout->addWidget(Wykonaj);

    // Dodanie layout do głównej aplikacji
    QWidget *centralWidget = new QWidget(this);
    centralWidget->setLayout(layout);
    setCentralWidget(centralWidget);

    // Podłączenie przycisku do sprawdzenia funkcji
    connect(Wykonaj, &QPushButton::clicked, this, &WindowMathematica::CheckFunction);
}

WindowMathematica::~WindowMathematica() {
    // Usuwamy poprzedni wykres (jeśli istnieje)
    delete previousChartView;
}

void WindowMathematica::CheckFunction()
{
    QString mainInput = MainInput->toPlainText().trimmed();  // Usuwamy zbędne białe znaki z początku i końca

    // 1. Sprawdzamy, czy użytkownik wpisał funkcję w formacie F[x] = <wyrażenie>
    QStringList FunctionParts = mainInput.split('=');
    if (FunctionParts.size() == 2) {  // Funkcja w formacie F[x] = <wyrażenie>
        QString FunctionSyntax = FunctionParts[0].trimmed();  // Składnia funkcji (np. F[x])
        QString FunctionValue = FunctionParts[1].trimmed();   // Wartość funkcji (np. x^2 + x + 1)

        // Sprawdzamy, czy składnia funkcji jest poprawna (czy zaczyna się od "F[" i kończy na "]")
        if (FunctionSyntax.startsWith("F[") && FunctionSyntax.endsWith("]")) {
            FunctionObject newFunction(FunctionSyntax, FunctionValue);
            functions.append(newFunction);  // Dodajemy nową funkcję do listy

            // Obliczanie punktów dla funkcji
            Function();
        }
        else {
            qDebug() << "Niepoprawny format składni funkcji! Oczekiwano F[x], a otrzymano: " << FunctionSyntax;
        }
    }
    // 2. Sprawdzamy, czy użytkownik wpisał komendę rysowania wykresu, np. Plot[F[x]]
    else if (mainInput.startsWith("Plot[F[") && mainInput.endsWith("]]")) {
        // Usuwamy "Plot[" i "]" z komendy
        QString functionName = mainInput.mid(5, mainInput.length() - 6).trimmed();

        // Przeszukujemy listę funkcji, aby znaleźć tę o nazwie 'functionName'
        FunctionObject *foundFunction = nullptr;
        for (FunctionObject &func : functions) {
            // Zmieniliśmy func.name() na func.getFunctionSyntax()
            if (func.getFunctionSyntax() == functionName) {
                foundFunction = &func;
                break;
            }
        }

        if (foundFunction) {
            // Narysuj wykres tej funkcji
            PaintFunction(*foundFunction);
        } else {
            qDebug() << "Funkcja " << functionName << " nie została znaleziona!";
        }
    }

    else if(mainInput.startsWith("Derivative[") && mainInput.endsWith("]")){
        // Usuwamy "Derivative[" i "]" z komendy
        QString functionName = mainInput.mid(11, mainInput.length() - 7).trimmed();
    }

    else {
        qDebug() << "Błędny format komendy! Upewnij się, że wpisujesz funkcję w formacie F[x] = <wyrażenie> lub Plot[F[x]].";
    }
}

double evaluateFunction(const QString& function, double x) {
    QJSEngine engine;

    // Rejestrujemy zmienną 'x' w silniku JavaScript
    QJSValue jsX = QJSValue(x);  // Zmienna 'x' przekazywana bezpośrednio
    engine.globalObject().setProperty("x", jsX);

    // Zamiana wyrażenia na format JavaScript:
    // 1. Zmieniamy wszystkie wystąpienia 'x^n' na 'Math.pow(x, n)'
    QString jsFunction = function;

    // Zamiana 'x^n' na 'Math.pow(x, n)'
    jsFunction.replace(QRegularExpression("x\\^(-?\\d+)"), "Math.pow(x, \\1)");

    // Zamiana ogólnego potęgowania na Math.pow(x, ...)
    jsFunction.replace("^", "Math.pow(x, ");

    // Próbujemy wykonać obliczenia w JavaScript
    QJSValue result = engine.evaluate(jsFunction);
    if (result.isNumber()) {
        return result.toNumber();  // Zwracamy wynik obliczeń
    }

    // W przypadku błędu
    qDebug() << "Błąd obliczeń funkcji: " << function;
    return 0.0;
}


void WindowMathematica::Function()
{
    // Wyczyszczenie poprzednich punktów
    points.clear();

    // Iteracja po funkcjach zapisanych w liście
    for (const FunctionObject &func : functions) {
        QString functionValue = func.getFunctionValue();  // Pobieramy wartość funkcji

        // Załóżmy, że 'x' ma zakres od -10 do 10, z większym krokiem
        for (double x = -10; x <= 10; x += 0.1) {  // Zwiększamy krok do 0.1
            double y = evaluateFunction(functionValue, x);  // Obliczamy wartość funkcji dla x

            // Debugowanie: Wypisanie wartości x i y
            qDebug() << "x: " << x << " y: " << y;

            points.append(QPointF(x, y));  // Dodajemy punkt do listy
        }

        // Po obliczeniu punktów, przechowujemy dane wykresu w obiekcie functionsChartsPoints
        functionsChartsPoints newChart(func.getFunctionSyntax(), points);
        chartPoints.append(newChart);  // Dodajemy do listy wykresów
    }
}


void WindowMathematica::PaintFunction(const FunctionObject &func)
{
    // Funkcja w postaci tekstowej, np. "x^3 + x + 2"
    QString expression = func.getFunctionValue();
    QList<QPointF> localPoints;  // Lista punktów lokalnych dla tej funkcji

    // Zakładając, że 'x' ma zakres od -10 do 10, obliczamy punkty z małym krokiem
    for (double x = -10; x <= 10; x += 0.1) {  // Zmniejszamy krok do 0.1
        double y = evaluateFunction(expression, x);  // Obliczamy wartość funkcji dla danego x
        localPoints.append(QPointF(x, y));  // Dodajemy punkt do listy
    }

    // Tworzymy wykres
    QChart *chart = new QChart();
    QLineSeries *series = new QLineSeries();

    // Dodajemy punkty do serii
    for (const QPointF &point : localPoints) {
        series->append(point);
    }

    // Dodajemy serię do wykresu
    chart->addSeries(series);
    chart->createDefaultAxes();

    // Tworzymy widok wykresu
    QChartView *chartView = new QChartView(chart);
    chartView->setRenderHint(QPainter::Antialiasing);
    chartView->setRenderHint(QPainter::TextAntialiasing);

    // Dodajemy widok wykresu do layoutu
    QWidget *graphWidget = new QWidget(this);
    QVBoxLayout *graphLayout = new QVBoxLayout(graphWidget);
    graphLayout->addWidget(chartView);
    graphWidget->setLayout(graphLayout);
    graphWidget->setGeometry(0, 100, 800, 400);
    graphWidget->show();
}



