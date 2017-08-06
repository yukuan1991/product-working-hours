#ifndef PWHMANAGEMENT_H
#define PWHMANAGEMENT_H

#include <QWidget>
#include <memory>
#include "Model/product_structure_model.h"

namespace Ui {
class PwhManagement;
}

class PwhManagement : public QWidget
{
    Q_OBJECT

public:
    explicit PwhManagement(QWidget *parent = 0);
    ~PwhManagement();
private slots:
    void on_tree_product_clicked(const QModelIndex& index);
private:
    std::unique_ptr<product_structure_model> fs_model_ { new product_structure_model };
private:
    Ui::PwhManagement *ui;
};

#endif // PWHMANAGEMENT_H
