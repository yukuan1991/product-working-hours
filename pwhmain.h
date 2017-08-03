#ifndef PWHMAIN_H
#define PWHMAIN_H

#include <QWidget>
#include <base/lang/not_null.h>

namespace Ui {
class PwhMain;
}

class PwhAnalysis;
class PwhMain : public QWidget
{
    Q_OBJECT

public:
    explicit PwhMain(QWidget *parent = 0);
    ~PwhMain();

private:
    void fileNew();
private:
    void initConn();
private:
    not_null<PwhAnalysis*> createWindow();
private:
    Ui::PwhMain *ui;
};

#endif // PWHMAIN_H
