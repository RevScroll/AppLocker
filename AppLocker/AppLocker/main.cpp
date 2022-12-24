#include "applocker.h"
#include <QtWidgets/QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    AppLocker w;
    w.show();
    return a.exec();
}
