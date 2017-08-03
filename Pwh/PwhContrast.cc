#include "PwhContrast.h"
#include "ui_pwhcontrast.h"
#include <assert.h>
#include <memory>
//#include <QTableWidgetItem>

PwhContrast::PwhContrast(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::PwhContrast)
{
    ui->setupUi(this);
    setup_more_ui();
    set_table();

}

PwhContrast::~PwhContrast()
{
    delete ui;
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
