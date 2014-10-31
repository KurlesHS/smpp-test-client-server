#include "testsmppservermainwindow.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    TestSmppServerMainWindow w;
    w.show();

    return a.exec();
}
