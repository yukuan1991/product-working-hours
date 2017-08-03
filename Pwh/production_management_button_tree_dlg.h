#ifndef PRODUCTION_MANAGEMENT_BUTTON_TREE_DLG_H
#define PRODUCTION_MANAGEMENT_BUTTON_TREE_DLG_H

#include <QDialog>
#include <memory>
#include "Qt-Utils/stl_extension.hpp"
#include "model/product_structure_model.h"
#include "Qt-Utils/json.hpp"

using json = nlohmann::json;

namespace Ui {
class production_management_button_tree_dlg;
}

class production_management_button_tree_dlg : public QDialog
{
    Q_OBJECT

public:
    explicit production_management_button_tree_dlg(QWidget *parent = 0);
    ~production_management_button_tree_dlg();

    const QString& get_path ()
    {
        return path_;
    }

private slots:
    void on_tree_product_clicked(const QModelIndex &index);

    void on_tree_product_doubleClicked(const QModelIndex &index);

    void on_button_ok_clicked();

    void on_button_cancel_clicked();

private:
    Ui::production_management_button_tree_dlg *ui;
    unique_ptr<product_structure_model> structure_ = make_unique<product_structure_model> ();
    shared_ptr<bool> alive_ = make_shared<bool> (true);
    QString path_;
};

#endif // PRODUCTION_MANAGEMENT_BUTTON_TREE_DLG_H
