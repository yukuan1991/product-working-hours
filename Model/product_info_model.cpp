#include "product_info_model.h"


product_info_model::product_info_model(QObject *parent)
    :json_model (parent)
{
    headers_  << "作业内容" << "测量时间" /*<< "视频路径"*/<< "预定工时法总代码"<< "公司内部代码"
             << "数量*频次" << "评比系数" << "基本时间" << "宽放率" << "标准时间"
             << "增值/非增值" << "操作分类" ;

    edit_col_ = headers_;
}

bool product_info_model::setData(const QModelIndex &index, const QVariant &value, int role)
{
    return json_model::setData (index, value, role);
}

void product_info_model::clear()
{
    data_.clear ();
}

QVariant product_info_model::data(const QModelIndex &index, int role) const
{
    return json_model::data (index, role);
}
