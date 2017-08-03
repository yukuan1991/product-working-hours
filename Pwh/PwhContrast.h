#ifndef PWHCONTRAST_H
#define PWHCONTRAST_H

#include <QWidget>
#include <optional>

namespace Ui {
class PwhContrast;
}

class QTableWidget;
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

    std::optional<QString> exec_window_tree();

    void on_add_clicked();
private:
    QString current_path_;
private:
    Ui::PwhContrast *ui;
};

#endif // PWHCONTRAST_H
