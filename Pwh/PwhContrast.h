#ifndef PWHCONTRAST_H
#define PWHCONTRAST_H

#include <QWidget>

namespace Ui {
class PwhContrast;
}

class QTableWidget;
class PwhContrast : public QWidget
{
    Q_OBJECT

public:
    explicit PwhContrast(QWidget *parent = 0);
    ~PwhContrast();
private:
    void setup_more_ui ();
    void set_table();
    void init_table(QTableWidget*);
private:
    Ui::PwhContrast *ui;
};

#endif // PWHCONTRAST_H
