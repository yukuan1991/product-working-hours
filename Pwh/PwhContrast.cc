#include "PwhContrast.h"
#include "ui_pwhcontrast.h"
#include <assert.h>
#include <memory>
#include "Pwh/production_management_button_tree_dlg.h"
#include "arithmetic_resource.hpp"
#include <QMessageBox>
#include "data_veryfication.h"
#include "Qt-Utils/json.hpp"

#include <QDebug>

PwhContrast::PwhContrast(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::PwhContrast)
{
    ui->setupUi(this);
    setup_more_ui();
    set_table();
    init_conn();

    ui->tree_product->setModel (fs_model_.get());
    ui->tree_product->header()->hide();
    assert(boost::filesystem::exists(PRODUCT_PATH));
    fs_model_->setRootPath (PRODUCT_PATH);
    ui->tree_product->setRootIndex (fs_model_->index (PRODUCT_PATH));
    fs_model_->set_master(PRODUCT_PATH);
    ui->tree_product->setEditTriggers(QAbstractItemView::NoEditTriggers);
}

PwhContrast::~PwhContrast()
{
    delete ui;
}

void PwhContrast::init_conn()
{
//    connect(ui->add, &QPushButton::clicked, this, &PwhContrast::on_add_clicked);
//    connect(ui->add_2, &QPushButton::clicked, this, &PwhContrast::on_add_2_clicked);
//    connect(ui->add_3, &QPushButton::clicked, this, &PwhContrast::on_add_3_clicked);

}


void PwhContrast::setup_more_ui()
{
    for (int i = 0; i < ui->listWidget->count (); i++)
    {
        auto item = ui->listWidget->item (i); assert(item);
        auto size = item->sizeHint ();

        size.setHeight (30);
        item->setSizeHint (size);
        item->setTextAlignment(Qt::AlignCenter);
    }
}

void PwhContrast::set_table()
{
    init_table (ui->table_master);
    init_table (ui->table_slave1);
    init_table (ui->table_slave2);
}

void PwhContrast::init_table(QTableWidget* table)
{
    assert (table);
    table->setRowCount (10);
    table->setColumnCount (2);
    table->verticalHeader()->setDefaultSectionSize(30);
    table->horizontalHeader()->hide();
    table->verticalHeader()->hide();

    auto header = table->horizontalHeader(); assert(header);
    header->setSectionResizeMode(QHeaderView::Stretch);
    table->setSpan(2, 0, 1, 2);
    table->setSpan(5, 0, 1, 2);


    auto row = table->rowCount();
    auto col = table->columnCount();
    for (int i = 0; i < row; i++)
    {
        for (int j = 0; j < col; j++)
        {
            auto item = std::make_unique<QTableWidgetItem> ();
            item->setFlags (item->flags() & (~Qt::ItemIsEditable));
            table->setItem (i, j, item.release());
        }
    }

    QFont bold_font;
    bold_font.setBold(true);
    auto item = table->item (0, 0); assert (item);
    item->setText ("时长");
    item->setTextAlignment (Qt::AlignHCenter | Qt::AlignVCenter);
    item->setFont (bold_font);

    item = table->item (0, 1); assert(item);
    item->setText ("占比");
    item->setTextAlignment (Qt::AlignHCenter | Qt::AlignVCenter);
    item->setFont (bold_font);
}

bool PwhContrast::parse_path(const QString &path)
{
    assert (!path.isEmpty ());

    QFileInfo info {path};
    if (info.isFile ())
    {
        if (!check_file_position_legality (path))
        {
            QMessageBox::information (this, tr("错误"), tr("文件出现在非法位置"));
            return false;
        }
        auto opt_data = file_data (path);
        if (!opt_data)
        {
            return false;
        }
        file_data_.push_back (std::move (*opt_data));
        return true;
    }
    else if (info.isDir ())
    {
        QDir dir {path};
        assert (dir.exists ());

        dir.setFilter (QDir::Dirs | QDir::Files);
        QFileInfoList list = dir.entryInfoList ();
        for (int i=0; i<list.size (); ++i)
        {
            auto item = list.at (i);
            if (item.fileName() == "." or item.fileName() == "..")
            {
                continue;
            }
            if (!parse_path (item.absoluteFilePath ()))
            {
                return false;
            }
        }
        return true;
    }
    else
    {
        return false;
    }
}

std::optional<nlohmann::json> PwhContrast::file_data(const QString &path)
{
    auto system_path = unicode_to_system (path.toStdString ());
    assert (boost::filesystem::exists (system_path));
    assert (boost::filesystem::is_regular_file (system_path));

    auto op_buffer = krys::read_all (system_path);
    assert (op_buffer);

    auto opj_data = stdtime_data_veryfication (*op_buffer);
    if (!opj_data)
    {
        QFileInfo info (path);
        auto filename = info.baseName ();
        return {};
    }

    auto iter_extra = opj_data->find ("附加信息");
    assert (iter_extra != opj_data->end () and iter_extra->is_object ());
    auto iter_method = iter_extra->find ("测量方法");
    assert (iter_method != iter_extra->end () and iter_method->is_string ());
    std::string method = *iter_method;

    auto iter_sheet = opj_data->find ("表");
    assert (iter_sheet != opj_data->end () and iter_sheet->is_object ());
    auto iter_detail = iter_sheet->find (method + "详细信息");
    assert (iter_detail != iter_sheet->end () and iter_detail->is_object ());

    if (method == "预定工时法")
    {
        auto iter_result = iter_detail->find ("总计");
        assert (iter_result != iter_detail->end () and iter_result->is_array ());
        return *iter_result;
    }
    else if (method == "视频分析法")
    {
        auto iter_result = iter_detail->find ("结果");
        assert (iter_result != iter_detail->end () and iter_result->is_array ());
        return *iter_result;
    }
    else if (method == "标准资料法")
    {
        auto iter_result = iter_detail->find ("详细信息");
        assert (iter_result != iter_detail->end () and iter_result->is_array ());
        return *iter_result;
    }
    else
    {
        assert (false);
    }

    return {};
}

std::optional<QString> PwhContrast::exec_window_tree()
{
    production_management_button_tree_dlg dlg;
    dlg.setWindowTitle("载入产品工时数据");

    if (dlg.exec () == QDialog::Accepted)
    {
        auto path = dlg.get_path();
        return path;
    }
    else
    {
        return {};
    }
}

void PwhContrast::abstract_file_data()
{
    for (size_t j=0; j<file_data_.size(); ++j)
    {
        auto total_data = file_data_ [j];
        assert (total_data.is_array ());
        for (size_t i=0; i<total_data.size (); ++i)
        {
            json data = total_data [i];
            float ftime = data ["标准时间"];
            data_ ["total"] += ftime;

            std::string str  = data ["增值/非增值"];
            if (str == "增值")
            {
                data_ ["added"] += ftime;
            }
            else
            {
                data_ ["unadded"] += ftime;
            }

            std::string operation = data ["操作分类"];
            if (operation == "加工")
            {
                data_ ["process"] += ftime;
            }
            else if (operation == "检查")
            {
                data_ ["inspect"] += ftime;
            }
            else if (operation == "搬运")
            {
                data_ ["carry"] += ftime;
            }
            else if (operation == "等待")
            {
                data_ ["wait"] += ftime;
            }
            else
            {
                data_ ["wait"] = 0;
            }
        }
    }
}

void PwhContrast::refresh_table_data(QTableWidget *table)
{
    float total_time = data_ ["total"];
    QTableWidgetItem *item = table->item(1,0); assert (item);
    QString data = QString::number (data_ ["total"], 'f', 2);

    item->setTextAlignment (Qt::AlignHCenter | Qt::AlignVCenter);
    item->setText (data);
    item = table->item(1,1);
    item->setTextAlignment (Qt::AlignHCenter | Qt::AlignVCenter);
    item->setText(total_time == 0 ? "N/A" : "100 %");

    float time = data_ ["added"];
    item = table->item(3,0); assert (item);
    data = QString::number (time, 'f', 2);
    item->setText(data);
    item->setTextAlignment (Qt::AlignHCenter | Qt::AlignVCenter);
    item = table->item (3,1); assert (item);
    item->setText(total_time == 0 ? "N/A" : QString::number (time/total_time*100, 'f', 2)+" %");
    item->setTextAlignment (Qt::AlignHCenter | Qt::AlignVCenter);

    time = data_ ["unadded"];
    item = table->item(4,0); assert (item);
    data = QString::number (time, 'f', 2);
    item->setText(data);
    item->setTextAlignment (Qt::AlignHCenter | Qt::AlignVCenter);
    item = table->item (4,1); assert (item);
    item->setText(total_time == 0 ? "N/A" : QString::number (time/total_time*100, 'f', 2)+" %");
    item->setTextAlignment (Qt::AlignHCenter | Qt::AlignVCenter);

    time = data_ ["process"];
    item = table->item(6,0); assert (item);
    data = QString::number (time, 'f', 2);
    item->setText(data);
    item->setTextAlignment (Qt::AlignHCenter | Qt::AlignVCenter);
    item = table->item (6,1); assert (item);
    item->setText(total_time == 0 ? "N/A" : QString::number (time/total_time*100, 'f', 2)+" %");
    item->setTextAlignment (Qt::AlignHCenter | Qt::AlignVCenter);

    time = data_ ["inspect"];
    item = table->item(7,0); assert (item);
    data = QString::number (time, 'f', 2);
    item->setText(data);
    item->setTextAlignment (Qt::AlignHCenter | Qt::AlignVCenter);
    item = table->item (7,1); assert (item);
    item->setText(total_time == 0 ? "N/A" : QString::number (time/total_time*100, 'f', 2)+" %");
    item->setTextAlignment (Qt::AlignHCenter | Qt::AlignVCenter);

    time = data_ ["carry"];
    item = table->item(8,0); assert (item);
    data = QString::number (time, 'f', 2);
    item->setText(data);
    item->setTextAlignment (Qt::AlignHCenter | Qt::AlignVCenter);
    item = table->item (8,1); assert (item);
    item->setText(total_time == 0 ? "N/A" : QString::number (time/total_time*100, 'f', 2)+" %");
    item->setTextAlignment (Qt::AlignHCenter | Qt::AlignVCenter);

    time = data_ ["wait"];
    item = table->item(9,0); assert (item);
    data = QString::number (time, 'f', 2);
    item->setText(data);
    item->setTextAlignment (Qt::AlignHCenter | Qt::AlignVCenter);
    item = table->item (9,1); assert (item);
    item->setText(total_time == 0 ? "N/A" : QString::number (time/total_time*100, 'f', 2)+" %");
    item->setTextAlignment (Qt::AlignHCenter | Qt::AlignVCenter);
}

void PwhContrast::refresh_table_data_source_info(QTableWidget *table)
{
    QFileInfo info { current_path_ };
    auto source = get_current_path().isEmpty() ? "N/A" : info.baseName();

    if (table == ui->table_master)
    {
        ui->label_add_master->setText (source);
    }
    else if(table == ui->table_slave1)
    {
        ui->label_add_slave1->setText (source);
    }
    else if(table == ui->table_slave2)
    {
        ui->label_add_slave2->setText (source);
    }
    else
    {
        assert (false);
    }

}

void PwhContrast::clear_table(QTableWidget *table)
{
    for (int i = 1; i <= 9; ++i)
    {
        for (int j = 0; j <= 1; ++j)
        {
            QTableWidgetItem* item = table->item(i, j);
            assert (item != nullptr);
            item->setText ("");
        }
    }
}

void PwhContrast::clear_file_data()
{
    set_current_path(nullptr);
    data_.clear();
    file_data_.clear();
}

QTableWidget *PwhContrast::find_empty_table()
{
    QTableWidget* empty_table = nullptr;
    if (ui->label_add_master->text() == "N/A")
    {
        empty_table = ui->table_master;
        return empty_table;
    }
    else if (ui->label_add_slave1->text() == "N/A")
    {
        empty_table = ui->table_slave1;
        return empty_table;
    }
    else if (ui->label_add_slave2->text() == "N/A")
    {
        empty_table = ui->table_slave2;
        return empty_table;
    }
    else
    {
        return nullptr;
    }
}

void PwhContrast::on_add_clicked()
{
    qDebug() << "on_add";
    const auto opt_path = exec_window_tree();

    if (opt_path)
    {
        set_current_path(*opt_path);
        refresh_table_data_source_info(ui->table_master);

        if (!parse_path (get_current_path ()))
        {
            clear_file_data();
            refresh_table_data_source_info(ui->table_master);
            return;
        }
        abstract_file_data();
        refresh_table_data(ui->table_master);

        clear_file_data ();
    }
}

void PwhContrast::on_add_2_clicked()
{
    const auto opt_path = exec_window_tree();

    if (opt_path)
    {
        set_current_path(*opt_path);
        refresh_table_data_source_info(ui->table_slave1);

        if (!parse_path(get_current_path()))
        {
            clear_file_data();
            refresh_table_data_source_info(ui->table_slave1);
            return;
        }
        abstract_file_data();
        refresh_table_data(ui->table_slave1);
        clear_file_data();
    }
}

void PwhContrast::on_add_3_clicked()
{
    const auto opt_path = exec_window_tree();

    if (opt_path)
    {
        set_current_path(*opt_path);
        refresh_table_data_source_info(ui->table_slave2);

        if(!parse_path(get_current_path()))
        {
            clear_file_data();
            refresh_table_data_source_info(ui->table_slave2);
            return;
        }
        abstract_file_data();
        refresh_table_data(ui->table_slave2);
        clear_file_data();
    }
}

void PwhContrast::on_delete_2_clicked()
{
    qDebug() << "on_delete_2";
    clear_table(ui->table_master);
    clear_file_data();
    refresh_table_data_source_info(ui->table_master);
}

void PwhContrast::on_delete_3_clicked()
{
    qDebug() << "on_delete_3";
    clear_table(ui->table_slave1);
    clear_file_data();
    refresh_table_data_source_info(ui->table_slave1);
}

void PwhContrast::on_delete_4_clicked()
{
    qDebug() << "on_delete_4";
    clear_table(ui->table_slave2);
    clear_file_data();
    refresh_table_data_source_info(ui->table_slave2);
}

void PwhContrast::on_tree_product_clicked(const QModelIndex &index)
{
    auto path = index.data (QFileSystemModel::FilePathRole);
    assert (path.type () == QVariant::String);
    auto qstr_path = path.toString ();

    QFileInfo info {path.toString ()};
    if (!info.isFile ())
    {
        return;
    }

    auto capp_dir = info.dir ();
    auto product_dir = capp_dir;
    bool is_ok;
    is_ok = product_dir.cdUp (); assert (is_ok);
    auto master_dir = product_dir;
    is_ok = master_dir.cdUp (); assert (is_ok);

    auto master_gbk = unicode_to_system (master_dir.absolutePath ().toStdString ());
    auto product_gbk = unicode_to_system (PRODUCT_PATH);

    if (boost::filesystem::equivalent (master_gbk, product_gbk))
    {
        current_path_ = std::move (qstr_path);
    }
    else
    {
        QMessageBox::information (this, "错误", "文件出现在了非法位置上！");
        return;
    }
}

void PwhContrast::on_tree_product_doubleClicked(const QModelIndex &index)
{
    const auto empty_table = find_empty_table();
    if (empty_table == nullptr)
    {
        return;
    }
    else
    {
        auto path_vat = index.data (QFileSystemModel::FilePathRole);
        assert (path_vat.type() == QVariant::String);
        auto path = path_vat.toString();

        QFileInfo info {path};
        if (info.isFile())
        {
            if (!parse_path(path))
            {
                return;
            }
            set_current_path(path);
            refresh_table_data_source_info(empty_table);

            abstract_file_data();
            refresh_table_data(empty_table);

            clear_file_data();
        }
    }
}
