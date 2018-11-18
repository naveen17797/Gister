#include "welcomescreen.h"
#include "ui_welcomescreen.h"
#include<QTimer>
#include<memory>
#include<ui_screen.h>
welcomeScreen::welcomeScreen(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::welcomeScreen)
{
    ui->setupUi(this);
    QTimer::singleShot(1000, this, SLOT(switch_screen()));
}

welcomeScreen::~welcomeScreen()
{
    delete ui;
}
void welcomeScreen::switch_screen() {
    this->close();
    ui_screen  *ui_scr = new ui_screen();
    ui_scr->show();

}
