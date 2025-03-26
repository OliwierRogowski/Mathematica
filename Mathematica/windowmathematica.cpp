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
#include "qcustomplot.h"
#include <QLabel>
#include <QFile>
#include <QKeyEvent>

WindowMathematica::WindowMathematica(QWidget *parent)
    : QMainWindow(parent),
    previousChartView(nullptr)  // Inicjalizacja zmiennej do przechowywania poprzedniego wykresu
{
    // Tworzenie layoutu aplikacji
    layout = new QVBoxLayout();

    // Tworzenie początkowego QLineEdit
    QLineEdit *firstInput = new QLineEdit(this);
    layout->addWidget(firstInput);

    // Dodanie layoutu do głównej aplikacji
    centralWidget = new QWidget(this);
    centralWidget->setLayout(layout);
    setCentralWidget(centralWidget);

    // Dodaj początkowy input do listy
    inputs.append(firstInput);

    // Połącz sygnał returnPressed z metodą, która będzie wywoływana przy naciśnięciu Enter
    connect(firstInput, &QLineEdit::returnPressed, [this, firstInput](){
        CheckFunction();  // wykonanie funkcji przy naciśnięciu Enter
        AddNewInputLine();  // Dodanie nowego inputu
    });
}

WindowMathematica::~WindowMathematica() {
    // Usuwamy poprzedni wykres (jeśli istnieje)
    delete previousChartView;
    qDeleteAll(inputs);  // Usuwamy wszystkie dynamicznie dodane QLineEdit
}

void WindowMathematica::keyPressEvent(QKeyEvent *event) {
    if (event->key() == Qt::Key_Return && event->modifiers() == Qt::ShiftModifier) {
        // Jeśli naciśnięto Shift + Enter, dodaj nowy input
        AddNewInputLine();
    } else {
        // Domyślna obsługa innych klawiszy
        QMainWindow::keyPressEvent(event);
    }
}

void WindowMathematica::AddNewInputLine() {
    // Tworzymy nowy QLineEdit
    QLineEdit *newInput = new QLineEdit(this);

    // Połącz nowy QLineEdit z funkcją, która będzie wykonywana po naciśnięciu Enter
    connect(newInput, &QLineEdit::returnPressed, [this, newInput](){
        CheckFunction();  // Wykonanie funkcji przy naciśnięciu Enter
        AddNewInputLine();  // Dodanie kolejnego inputu
    });

    // Dodaj nowy QLineEdit do layoutu
    layout->addWidget(newInput);

    // Dodaj do listy wejściowych pól tekstowych
    inputs.append(newInput);
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

bool checkSequenceWithName(const QString& str, QChar &letter, int& number1, int& number2, QString& nameX, QString& nameY) {
    // Usuwanie zbędnych spacji
    QString trimmedStr = str.simplified();

    // Wyrażenie regularne: przechwytywanie litery, liczb oraz nazw X i Y
    QRegularExpression pattern(R"(^Plot\[F\[([a-zA-Z])\],\{(-?\d+),(-?\d+)\}\,\{([^\}]+),([^\}]+)\}\]$)");

    QRegularExpressionMatch match = pattern.match(trimmedStr);

    if (match.hasMatch()) {
        letter = match.captured(1)[0];  // Pobranie litery między F[ ]
        number1 = match.captured(2).toInt();  // Pierwsza liczba
        number2 = match.captured(3).toInt();  // Druga liczba
        nameX = match.captured(4);  // Nazwa dla X
        nameY = match.captured(5);  // Nazwa dla Y
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
    QString mainInput = MainInput->text().trimmed();
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

    else if(checkSequenceWithName(mainInput, litera, liczba1, liczba2, nameX, nameY)){
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

        // Now userInput will be "F[] = x^2+x+1x]" or "F'[x]" without the "Derivative" part
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
    QString derivativeValue = value;

    if (derivativeValue.contains("x")) {
        QRegularExpression powerPattern("x\\^(-?\\d+)");
        QRegularExpressionMatchIterator it = powerPattern.globalMatch(derivativeValue);

        while (it.hasNext()) {
            QRegularExpressionMatch match = it.next();
            QString powerStr = match.captured(1);
            bool ok;
            int power = powerStr.toInt(&ok);
            if (ok && power != 0) {
                int newPower = power - 1;
                QString newTerm = QString::number(power) + "*x^" + QString::number(newPower);
                derivativeValue.replace(match.captured(0), newTerm);
            } else if (power == 0) {
                derivativeValue.replace(match.captured(0), "0");
            }
        }

        derivativeValue = derivativeValue.replace(QRegularExpression("([\\+\\-])0*x\\^0"), "");

        qDebug() << "Pochodna funkcji: " << derivativeValue;
    } else {
        qDebug() << "Brak zmiennej 'x' w funkcji do obliczenia pochodnej!";
    }

    DerivativeOfAFunction newDerivativeFunction("F'[" + value + "]", derivativeValue);
    derivativeFunctions.append(newDerivativeFunction);
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
    // Create the function chart using QCustomPlot
    QVector<QPointF> localPoints;
    QString expression = func.getFunctionValue();

    for (double x = liczba1; x <= liczba2; x += 0.1) {
        double y = evaluateFunction(expression, x);
        localPoints.append(QPointF(x, y));
    }

    // Create a new custom plot chart
    QCustomPlot *customPlot = new QCustomPlot(this);

    QVector<double> xData, yData;
    for (const QPointF &point : localPoints) {
        xData.append(point.x());
        yData.append(point.y());
    }

    customPlot->addGraph();
    customPlot->graph(0)->setData(xData, yData);
    customPlot->graph(0)->setPen(QPen(Qt::blue));


    if(nameX == "" && nameY== ""){
        nameX= "x";
        nameX= "y";
    }
    else if(nameX == "" && nameY !=""){
        nameX= "x";
    }
    else if(nameX != "" && nameY ==""){
        nameY= "y";
    }


    customPlot->xAxis->setLabel(nameX);
    customPlot->yAxis->setLabel(nameY);
    customPlot->xAxis->setRange(liczba1, liczba2);
    customPlot->yAxis->setRange(*std::min_element(yData.begin(), yData.end()) - 1,
                                *std::max_element(yData.begin(), yData.end()) + 1);

    // Add the chart to the layout
    layout->addWidget(customPlot);
}
/*void WindowMathematica::PaintFunctionWithName(const FunctionObject &func)
{
    // Funkcja w postaci tekstowej, np. "x^3 + x + 2"
    QString expression = func.getFunctionValue();
    QVector<QPointF> localPoints;  // Lista punktów lokalnych dla tej funkcji

    // Zakładając, że 'x' ma zakres od liczba1 do liczba2, obliczamy punkty z małym krokiem
    for (double x = liczba1; x <= liczba2; x += 0.1) {  // Zmniejszamy krok do 0.1
        double y = evaluateFunction(expression, x);  // Obliczamy wartość funkcji dla danego x
        localPoints.append(QPointF(x, y));  // Dodajemy punkt do listy
    }

    // Tworzymy wykres z QCustomPlot
    QCustomPlot *customPlot = new QCustomPlot(this);  // Tworzymy instancję QCustomPlot

    // Ustawienie danych do wykresu
    QVector<double> xData, yData;  // Wektor danych dla QCustomPlot
    for (const QPointF &point : localPoints) {
        xData.append(point.x());
        yData.append(point.y());
    }

    // Dodanie wykresu do obiektu QCustomPlot
    customPlot->addGraph();  // Dodajemy nowy graf
    customPlot->graph(0)->setData(xData, yData);  // Przypisujemy dane do grafu
    customPlot->graph(0)->setPen(QPen(Qt::blue));  // Kolor wykresu (np. niebieski)

    // Ustawienia osi
    customPlot->xAxis->setLabel(nameX);
    customPlot->yAxis->setLabel(nameY);
    customPlot->xAxis->setRange(liczba1, liczba2);  // Zakres osi X
    customPlot->yAxis->setRange(*std::min_element(yData.begin(), yData.end()) - 1,
                                *std::max_element(yData.begin(), yData.end()) + 1);  // Zakres osi Y

    // Dodajemy wykres do layoutu
    layout->addWidget(customPlot);
}*/




