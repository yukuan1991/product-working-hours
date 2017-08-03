#ifndef PWHANALYSIS_H
#define PWHANALYSIS_H

#include <QWidget>

namespace Ui {
class PwhAnalysis;
}

class PwhAnalysis : public QWidget
{
    Q_OBJECT

public:
    explicit PwhAnalysis(QWidget *parent = 0);
    ~PwhAnalysis();

private:
    Ui::PwhAnalysis *ui;
};

#endif // PWHANALYSIS_H
