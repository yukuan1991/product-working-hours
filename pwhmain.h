#ifndef PWHMAIN_H
#define PWHMAIN_H

#include <QWidget>

namespace Ui {
class PwhMain;
}

class PwhMain : public QWidget
{
    Q_OBJECT

public:
    explicit PwhMain(QWidget *parent = 0);
    ~PwhMain();

private:
    Ui::PwhMain *ui;
};

#endif // PWHMAIN_H
