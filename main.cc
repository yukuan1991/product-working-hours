#include "pwhmain.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    PwhMain w;
    w.show();

    return a.exec();
}
