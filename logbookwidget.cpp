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

    model->setHeaderData(1, Qt::Horizontal, QObject::tr("Call"));
    model->setHeaderData(2, Qt::Horizontal, QObject::tr("RX RST"));
    model->setHeaderData(3, Qt::Horizontal, QObject::tr("TX RST"));
    model->setHeaderData(4, Qt::Horizontal, QObject::tr("Name"));
    model->setHeaderData(5, Qt::Horizontal, QObject::tr("Location"));
    model->setHeaderData(6, Qt::Horizontal, QObject::tr("Locator"));
    model->setHeaderData(7, Qt::Horizontal, QObject::tr("Time"));

    model->select();

    //addAction(deleteContact);
    ui->contactTable->setModel(model);
    ui->contactTable->hideColumn(0);
}

void LogbookWidget::deleteContact()
{
    foreach (QModelIndex index, ui->contactTable->selectionModel()->selectedRows()) {
        model->removeRow(index.row());
    }
}

LogbookWidget::~LogbookWidget()
{
    delete ui;
}
