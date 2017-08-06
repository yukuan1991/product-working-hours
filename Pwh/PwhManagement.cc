#include "PwhManagement.h"
#include "ui_pwhmanagement.h"
#include <boost/filesystem.hpp>

#include <QDebug>

PwhManagement::PwhManagement(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::PwhManagement)
{
    ui->setupUi(this);

    ui->tree_product->setModel (fs_model_.get());
    assert (boost::filesystem::exists (PRODUCT_PATH));
    fs_model_->setRootPath(PRODUCT_PATH);
    ui->tree_product->setRootIndex(fs_model_->index(PRODUCT_PATH));
    fs_model_->set_master(PRODUCT_PATH);
    ui->tree_product->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->tree_product->header()->hide();
}

PwhManagement::~PwhManagement()
{
    delete ui;
}

void PwhManagement::on_tree_product_clicked(const QModelIndex &index)
{
    qDebug() << "on_tree_product_clicked";
    const auto path = index.data(QFileSystemModel::FilePathRole);
    assert(path.type() == QVariant::String);

    QFileInfo finfo { path.toString() };
    if (!finfo.isFile())
    {
        return;
    }

    ui->widget_video_player->setEnabled(false);

    const auto qstr_path = path.toString();
    QFileInfo info(qstr_path);
    const auto capp_dir = info.dir();
    const auto product_dir = capp_dir;





}
