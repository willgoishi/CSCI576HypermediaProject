#include "mainwindow.h"
#include <QApplication>

#include <QApplication>
#include <QCommandLineParser>
#include <QCommandLineOption>
#include <QDir>

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    MainWindow mainWindow;
    mainWindow.show();

    return app.exec();
}
