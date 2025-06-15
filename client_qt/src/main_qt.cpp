#include <QApplication>
#include <QFile>
#include "MainWindow.hpp"

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);

    // --- Application du thème Dark ---
    QFile styleFile(":/styles/dark.qss");
    if (styleFile.open(QFile::ReadOnly | QFile::Text)) {
        QString styleSheet = QLatin1String(styleFile.readAll());
        app.setStyleSheet(styleSheet);
    } else {
        // En cas d'erreur, pour le débogage
        qWarning("Impossible d'ouvrir la feuille de style.");
    }
    // ---------------------------------
    
    MainWindow window;
    window.resize(800, 600); // Agrandir un peu la fenêtre par défaut
    window.show();
    
    return app.exec();
}