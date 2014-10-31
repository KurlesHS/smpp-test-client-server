#include "testsmppclientmainwindow.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    TestSmppClientMainWindow w;
    w.show();

    return a.exec();
}
