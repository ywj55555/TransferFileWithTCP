#include "mainwindow.h"

#include <QApplication>
#pragma execution_character_set("utf-8")
int main(int argc, char *argv[])
{
    if (QT_VERSION >= QT_VERSION_CHECK(5, 6, 0))
    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
    QApplication a(argc, argv);
    MainWindow w;
    w.show();
    return a.exec();
}
