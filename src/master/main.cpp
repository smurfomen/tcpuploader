#include "mainwindow.h"
#include <QApplication>
#include "ini.h"
int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    ini::init();
    MainWindow w;
    w.show();

    return a.exec();
}
