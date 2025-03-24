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
#include <QApplication>
#include <QQmlApplicationEngine>
#include "derivativeofafunction.h"


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

bool checkSequence(const QString& str, QChar &letter, int& number1, int& number2) {
    // Usuwanie zbędnych spacji
    QString trimmedStr = str.simplified();

    // Wyrażenie regularne: przechwytywanie litery i liczb
    QRegularExpression pattern(R"(^Plot\[F\[([a-zA-Z])\],\{(-?\d+),(-?\d+)\}\]$)");
    QRegularExpressionMatch match = pattern.match(trimmedStr);

    if (match.hasMatch()) {
        letter = match.captured(1)[0];  // Pobranie znaku między F[ ]
        number1 = match.captured(2).toInt();
        number2 = match.captured(3).toInt();
        return true;
    }

    return false;
}


bool checkSequenceForDerivativeFunction(const QString& str, QString& input, QString& contentInsideBrackets) {
    QString trimmedStr = str.simplified();
    // Regex to capture both F[x] and F'[x] (keeping the apostrophe if it exists)
    QRegularExpression pattern("^Derivative\\[F'?\\[([A-Za-z]+)\\]\\]$");  // Capture content inside []

    QRegularExpressionMatch match = pattern.match(trimmedStr);

    if (match.hasMatch()) {
        // Remove the "Derivative[" part (first 10 characters) and "]" (last character)
        input = match.captured(0).mid(11, match.captured(0).length() - 12);  // Skip "Derivative[" and "]"
        contentInsideBrackets = match.captured(1);  // This will capture content between []
        return true;
    }

    return false;
}




QString cleanString(const QString& str) {
    QString cleaned;
    for (QChar c : str) {
        if (c.isLetterOrNumber() || c == '[' || c == ']') {
            cleaned += c;
        }
    }
    return cleaned;
}


void WindowMathematica::CheckFunction()
{
    QString mainInput = MainInput->toPlainText().trimmed();
    QString userInput;
    QString contentInsideBrackets;

    // 1. Checking the user input for function format F[x] = <expression>
    QStringList FunctionParts = mainInput.split('=');
    if (FunctionParts.size() == 2) {
        QString FunctionSyntax = FunctionParts[0].trimmed();
        QString FunctionValue = FunctionParts[1].trimmed();

        if (FunctionSyntax.startsWith("F[") && FunctionSyntax.endsWith("]")) {
            FunctionObject newFunction(FunctionSyntax, FunctionValue);
            functions.append(newFunction);
            Function();  // Evaluate function and prepare graph
        } else {
            qDebug() << "Niepoprawny format składni funkcji! Oczekiwano F[x], a otrzymano: " << FunctionSyntax;
        }
    }
    // 2. Checking for Plot input sequence like Plot[F[x], {...}]
    else if (checkSequence(mainInput, litera, liczba1, liczba2)) {
        FunctionObject *foundFunction = nullptr;
        QString functionName = "F[" + QString(litera) + "]";

        for (FunctionObject &func : functions) {
            QString cleanFuncSyntax = cleanString(func.getFunctionSyntax());
            if (cleanFuncSyntax == functionName) {
                foundFunction = &func;
                break;
            }
        }

        if (foundFunction) {
            PaintFunction(*foundFunction);
        } else {
            qDebug() << "Funkcja " << functionName << " nie została znaleziona!";
        }
    }
    // 3. Check for Derivative sequence like Derivative[F[x]] or Derivative[F'[x]]
    else if (checkSequenceForDerivativeFunction(mainInput, userInput, contentInsideBrackets)) {
        qDebug() << "Próbujemy obliczyć pochodną funkcji " << userInput;

        // Now userInput will be "F[x]" or "F'[x]" without the "Derivative" part
        QString input = userInput;
        qDebug() << "Input for derivative: " << input;
        qDebug() << "Content inside brackets: " << contentInsideBrackets;

        if(input.startsWith("F'")){
            //nadpisz funkcje pochodną
        }
        else if(input.startsWith("F")){
            //stwórz nową funkcję pochodną i dodaj ją do listy
            QString SyntaxFunction = "F[" + QString(contentInsideBrackets) + "]";  // F[x]
            QString derivativeSyntaxFunction = "F'[" + QString(contentInsideBrackets) + "]";  // F'[x]

            FunctionObject *derivativeFunctionValue = nullptr;
            for (FunctionObject &func : functions) {
                QString cleanFuncSyntax = cleanString(func.getFunctionSyntax());
                if (cleanFuncSyntax == SyntaxFunction) {
                    derivativeFunctionValue = &func;
                    break;
                }
            }

            if (derivativeFunctionValue) {
                // Calculate the derivative of the function
                DerivativeFunction(derivativeFunctionValue->getFunctionValue());
                qDebug() << "Obliczono pochodną funkcji " << SyntaxFunction;
            }
        }
    }
}


void WindowMathematica::DerivativeFunction(QString value) {
    // Zmienna przechowująca pochodną funkcji
    QString derivativeValue = value;

    // Sprawdzenie, czy funkcja zawiera 'x' (na przykład 'x^2', 'x', itp.)
    if (derivativeValue.contains("x")) {
        // Prosta analiza składniowa: różniczkowanie podstawowych funkcji
        // Zamiana x^n na n*x^(n-1) dla x^n
        QRegularExpression powerPattern("x\\^(-?\\d+)");  // Wyszukiwanie wzoru 'x^n'
        QRegularExpressionMatchIterator it = powerPattern.globalMatch(derivativeValue);

        // Przeanalizuj każdy przypadek 'x^n'
        while (it.hasNext()) {
            QRegularExpressionMatch match = it.next();
            QString powerStr = match.captured(1);  // Wartość potęgi (np. '2' z 'x^2')
            bool ok;
            int power = powerStr.toInt(&ok);
            if (ok && power != 0) {
                // Obliczamy pochodną: n*x^(n-1)
                int newPower = power - 1;
                QString newTerm = QString::number(power) + "*x^" + QString::number(newPower);
                derivativeValue.replace(match.captured(0), newTerm);  // Zamieniamy 'x^n' na 'n*x^(n-1)'
            } else if (power == 0) {
                // Jeśli potęga to 0 (stała), pochodna jest 0
                derivativeValue.replace(match.captured(0), "0");
            }
        }

        // Usuń zbędne elementy i uprość wyrażenie
        derivativeValue = derivativeValue.replace(QRegularExpression("([\\+\\-])0*x\\^0"), "");  // Usuń "0*x^0"

        // Obsługa funkcji, które zawierają zmienną 'x'
        qDebug() << "Pochodna funkcji: " << derivativeValue;
    } else {
        qDebug() << "Brak zmiennej 'x' w funkcji do obliczenia pochodnej!";
    }

    // Tworzymy nowy obiekt DerivativeOfAFunction
    DerivativeOfAFunction newDerivativeFunction("F'[" + value + "]", derivativeValue);

    // Dodanie pochodnej do listy funkcji pochodnych
    derivativeFunctions.append(newDerivativeFunction);  // Dodanie do listy funkcji pochodnych

    qDebug() << "Pochodna funkcji: " << derivativeValue;
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
    for (double x = liczba1; x <= liczba2; x += 0.1) {  // Zmniejszamy krok do 0.1
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
    graphWidget->setStyleSheet("QVBoxLayout {"
                             "background-color: #f0f0f0;"
                             "padding: 5px;"
                             "width: 500px;"
                             "height: 500px;"
                             "}");
    graphWidget->show();
}





