#include "PwhMain.h"
#include "ui_pwhmain.h"
#include "Pwh/PwhAnalysis.h"
#include <memory>
#include <QMdiSubWindow>

using namespace std;

PwhMain::PwhMain(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::PwhMain)
{
    ui->setupUi(this);
    initConn();
    ui->mdi->setViewMode(QMdiArea::TabbedView);
    setWindowState(Qt::WindowMaximized);
}

PwhMain::~PwhMain()
{
    delete ui;
}

void PwhMain::fileNew()
{
    auto w = createWindow();
    w->setWindowTitle("未命名");
}

void PwhMain::initConn()
{
    connect(ui->rib, &PwhRibbon::fileNew, this, &PwhMain::fileNew);
    connect(ui->rib, &PwhRibbon::fileExit, this, &PwhRibbon::close);
}

not_null<PwhAnalysis *> PwhMain::createWindow()
{
    auto pwh_win = make_unique<PwhAnalysis> ();
    pwh_win->setAttribute(Qt::WA_DeleteOnClose);
    auto w = ui->mdi->addSubWindow (pwh_win.get());

    w->setWindowState(Qt::WindowMaximized);
    return pwh_win.release();
}
