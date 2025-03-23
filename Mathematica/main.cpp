#include "windowmathematica.h"

#include <QApplication>
#include <QLocale>
#include <QTranslator>
#include <QApplication>
#include <QQmlApplicationEngine>


int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    QGuiApplication::setOrganizationName("Oliwier Rogowski");
    QGuiApplication::setApplicationName("Mathematica");

    QTranslator translator;
    const QStringList uiLanguages = QLocale::system().uiLanguages();
    for (const QString &locale : uiLanguages) {
        const QString baseName = "Mathematica_" + QLocale(locale).name();
        if (translator.load(":/i18n/" + baseName)) {
            a.installTranslator(&translator);
            break;
        }
    }
    WindowMathematica w;
    w.show();
    return a.exec();
}
