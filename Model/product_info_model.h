#ifndef GLOBAL_INFO_MODEL_H
#define GLOBAL_INFO_MODEL_H

#include "Model/json_model.h"

class product_info_model : public json_model
{
    Q_OBJECT
public:
    product_info_model (QObject* parent = nullptr);

    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    bool setData (const QModelIndex& index, const QVariant& value, int role) override;
    void clear ();
};

#endif // GLOBAL_INFO_MODEL_H
