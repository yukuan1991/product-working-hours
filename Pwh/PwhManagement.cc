#include "PwhManagement.h"
#include "ui_pwhmanagement.h"

PwhManagement::PwhManagement(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::PwhManagement)
{
    ui->setupUi(this);
}

PwhManagement::~PwhManagement()
{
    delete ui;
}
