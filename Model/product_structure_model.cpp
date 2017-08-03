#include "product_structure_model.h"
#include <QMimeData>
#include <QMessageBox>
#include <QDir>
#include "arithmetic_resource.hpp"
#include <QDebug>
#include <QUrl>

product_structure_model::product_structure_model(QObject* parent):
    QFileSystemModel (parent)
{
    QFileSystemModel::setReadOnly (false);
}

void product_structure_model::set_master(const char *master_path)
{
    master_path_ = master_path;
}

QVariant product_structure_model::data(const QModelIndex &index, int role) const
{
    int roles [] =
    {
        Qt::DisplayRole,
        QFileSystemModel::FilePathRole,
        QFileSystemModel::FileNameRole
    };

    for (auto iter : roles)
    {
        if (role == iter)
        {
            return QFileSystemModel::data (index, role);
        }
    }

    return {};
}

bool product_structure_model::dropMimeData(const QMimeData *data, Qt::DropAction action, int row, int column, const QModelIndex &parent)
{
    if (master_path_ == nullptr)
    {
        return false;
    }
    auto list = data->urls (); assert (!list.isEmpty ());
    auto path = list[0].path ();

    if (*path.begin () == '/')
    {
        path.remove (0, 1);
    }

    QDir mime_dir {path};
    mime_dir.cdUp ();

    auto index_path = parent.data (QFileSystemModel::FilePathRole);
    QDir index_dir {index_path.toString ()};

    auto mime_type = get_path_type (master_path_, mime_dir.absolutePath ());
    auto index_type = get_path_type (master_path_, index_dir.absolutePath ());

    if (index_type != mime_type)
    {
        QMessageBox::information (nullptr, "移动", "不同级别的路径不能相互移动");
        return false;
    }

    auto mime_filename = QFileInfo {path}.fileName ();
    auto file_list = index_dir.entryList ();

    return QFileSystemModel::dropMimeData (data, action, row, column, parent);
}

int product_structure_model::columnCount(const QModelIndex &) const
{
    return 1;
}
