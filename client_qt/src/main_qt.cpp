/**
 * @file main_qt.cpp
 * @brief Point d'entrée de l'application client de tchat.
 *
 * Ce fichier initialise QApplication, charge la feuille de style,
 * crée et affiche la fenêtre principale (MainWindow).
 */

#include <QApplication>
#include <QFile>
#include <QDebug>
#include "MainWindow.hpp"

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);

    // Application du thème global via une feuille de style QSS
    QFile styleFile(":/styles/dark.qss");
    if (styleFile.open(QFile::ReadOnly | QFile::Text)) {
        QString styleSheet = QLatin1String(styleFile.readAll());
        app.setStyleSheet(styleSheet);
        styleFile.close();
    } else {
        qWarning("Impossible d'ouvrir la feuille de style ':/styles/dark.qss'.");
    }
    
    MainWindow window;
    window.resize(950, 700);
    window.show();
    
    return app.exec();
}