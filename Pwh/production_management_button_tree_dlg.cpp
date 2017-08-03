#include "production_management_button_tree_dlg.h"
#include "ui_production_management_button_tree_dlg.h"
#include <boost/scope_exit.hpp>
#include "data_veryfication.h"
#include "Qt-Utils/krys_application.hpp"
#include "Qt-Utils/qt.hpp"
#include <boost/filesystem.hpp>
#include <QDebug>

production_management_button_tree_dlg::production_management_button_tree_dlg(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::production_management_button_tree_dlg)
{
    ui->setupUi(this);

    assert (boost::filesystem::exists (PRODUCT_PATH));
    structure_->setRootPath (PRODUCT_PATH);
    ui->tree_product->setModel (structure_.get ());
    ui->tree_product->setRootIndex (structure_->index (PRODUCT_PATH));
}

production_management_button_tree_dlg::~production_management_button_tree_dlg()
{
    delete ui;
}

void production_management_button_tree_dlg::on_tree_product_clicked(const QModelIndex &index)
{
    ui->button_ok->setEnabled (true);
    auto variant = structure_->data (index, QFileSystemModel::FilePathRole);
    path_ = variant.toString ();
}

void production_management_button_tree_dlg::on_tree_product_doubleClicked(const QModelIndex &)
{
    if (ui->button_ok->isEnabled ())
    {
        accept ();
    }
}

void production_management_button_tree_dlg::on_button_ok_clicked()
{
    accept ();
}

void production_management_button_tree_dlg::on_button_cancel_clicked()
{
    reject ();
}
