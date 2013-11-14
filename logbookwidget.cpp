#include <QtSql>
#include "logbookwidget.h"
#include "ui_logbookwidget.h"

LogbookWidget::LogbookWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::LogbookWidget)
{
    ui->setupUi(this);

    model = new QSqlTableModel;
    model->setTable("contacts");
    model->setEditStrategy(QSqlTableModel::OnFieldChange);

    model->setHeaderData(1, Qt::Horizontal, QObject::tr("Date"));
    model->setHeaderData(2, Qt::Horizontal, QObject::tr("Time on"));
    model->setHeaderData(3, Qt::Horizontal, QObject::tr("Time off"));
    model->setHeaderData(4, Qt::Horizontal, QObject::tr("Call"));
    model->setHeaderData(5, Qt::Horizontal, QObject::tr("RX RST"));
    model->setHeaderData(6, Qt::Horizontal, QObject::tr("TX RST"));
    model->setHeaderData(7, Qt::Horizontal, QObject::tr("Name"));
    model->setHeaderData(8, Qt::Horizontal, QObject::tr("Location"));
    model->setHeaderData(9, Qt::Horizontal, QObject::tr("Locator"));
    model->setHeaderData(10, Qt::Horizontal, QObject::tr("Frequency"));
    model->setHeaderData(11, Qt::Horizontal, QObject::tr("Band"));
    model->setHeaderData(12, Qt::Horizontal, QObject::tr("Mode"));
    model->setHeaderData(13, Qt::Horizontal, QObject::tr("Transmitter"));
    model->setHeaderData(14, Qt::Horizontal, QObject::tr("Power"));
    model->setHeaderData(15, Qt::Horizontal, QObject::tr("Comment"));

    model->select();

    //addAction(deleteContact);
    ui->contactTable->setModel(model);
    ui->contactTable->hideColumn(0);
    ui->contactTable->hideColumn(3);
    ui->contactTable->hideColumn(15);
    ui->contactTable->addAction(ui->deleteContact);
    ui->contactTable->sortByColumn(0);
    ui->contactTable->horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);
}

void LogbookWidget::deleteContact()
{
    foreach (QModelIndex index, ui->contactTable->selectionModel()->selectedRows()) {
        model->removeRow(index.row());
    }
}

void LogbookWidget::updateTable() {
    model->select();
}

LogbookWidget::~LogbookWidget()
{
    delete ui;
}
