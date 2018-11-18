#include "welcomescreen.h"
#include <QApplication>
#include<QDesktopWidget>
int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    welcomeScreen w;
    w.setWindowFlags(Qt::FramelessWindowHint);
    w.move(QApplication::desktop()->rect().center() - w.rect().center());
    w.show();

    return a.exec();
}
