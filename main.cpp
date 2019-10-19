#include "mainwindow.h"
#include <QApplication>
#include <iostream>
#include <fatAPI.h>
#include <windows.h>
#include <QTextCodec>
#include <QTextStream>
#include <QDebug>
#include <QString>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
    w.show();
    return a.exec();
}
