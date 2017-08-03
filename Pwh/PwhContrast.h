#ifndef PWHCONTRAST_H
#define PWHCONTRAST_H

#include <QWidget>
#include <optional>
#include "Qt-Utils/json.hpp"
#include "Model/product_structure_model.h"

namespace Ui {
class PwhContrast;
}

class QTableWidget;
using nlohmann::json;

class PwhContrast : public QWidget
{
    Q_OBJECT

public:
    explicit PwhContrast(QWidget *parent = 0);
    ~PwhContrast();

    void set_current_path (const QString& path) { current_path_ = path; }
    QString get_current_path () { return current_path_; }
private:
    void init_conn();
private:
    void setup_more_ui ();
    void set_table();
    void init_table(QTableWidget*);

    bool parse_path (const QString& path);
    std::optional<json> file_data(const QString& path);

    std::optional<QString> exec_window_tree();

    void abstract_file_data ();
    void refresh_table_data (QTableWidget* table);

    void refresh_table_data_source_info (QTableWidget* table);
    void clear_table (QTableWidget* table);
    void clear_file_data();
    QTableWidget* find_empty_table();

private slots:
    void on_add_clicked();
    void on_add_2_clicked();
    void on_add_3_clicked();
    void on_delete_2_clicked();
    void on_delete_3_clicked();
    void on_delete_4_clicked();
    void on_tree_product_clicked(const QModelIndex& index);
    void on_tree_product_doubleClicked(const QModelIndex& index);
private:
    std::unique_ptr<product_structure_model> fs_model_ { new product_structure_model};
    QString current_path_;
    std::map <std::string, float> data_;
    std::vector <json> file_data_;
private:
    Ui::PwhContrast *ui;
};

#endif // PWHCONTRAST_H
