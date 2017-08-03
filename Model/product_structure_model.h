#ifndef PRODUCT_STRUCTURE_MODEL_H
#define PRODUCT_STRUCTURE_MODEL_H

#include <QFileSystemModel>

class product_structure_model : public QFileSystemModel
{
public:
    product_structure_model(QObject* parent = nullptr);
    void set_master (const char* master_path);
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    bool dropMimeData(const QMimeData *data, Qt::DropAction action, int row, int column, const QModelIndex &parent) override;
    int columnCount (const QModelIndex& index) const override;
private:
    const char* master_path_ = nullptr;
};

#endif // PRODUCT_STRUCTURE_MODEL_H
