#include "pwhmain.h"
#include "ui_pwhmain.h"

PwhMain::PwhMain(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::PwhMain)
{
    ui->setupUi(this);
}

PwhMain::~PwhMain()
{
    delete ui;
}
