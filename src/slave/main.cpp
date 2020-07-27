#include "clientuploader.h"
#include <QApplication>
#include "ini.h"
int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    ini::init();
    ClientUploader client("127.0.0.1:28080", logger);

    return a.exec();
}
