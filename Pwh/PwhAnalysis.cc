#include "PwhAnalysis.h"
#include "ui_pwhanalysis.h"

PwhAnalysis::PwhAnalysis(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::PwhAnalysis)
{
    ui->setupUi(this);
}

PwhAnalysis::~PwhAnalysis()
{
    delete ui;
}
