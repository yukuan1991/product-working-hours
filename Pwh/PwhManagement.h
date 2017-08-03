#ifndef PWHMANAGEMENT_H
#define PWHMANAGEMENT_H

#include <QWidget>

namespace Ui {
class PwhManagement;
}

class PwhManagement : public QWidget
{
    Q_OBJECT

public:
    explicit PwhManagement(QWidget *parent = 0);
    ~PwhManagement();

private:
    Ui::PwhManagement *ui;
};

#endif // PWHMANAGEMENT_H
