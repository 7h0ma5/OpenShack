#include <QtSql/QtSql>
#include <QDebug>
#include "newcontactwidget.h"
#include "ui_newcontactwidget.h"

NewContactWidget::NewContactWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::NewContactWidget)
{
    ui->setupUi(this);
}

void NewContactWidget::callsignChanged() {
    if (ui->callsignEdit->text().isEmpty()) {
        return;
    }

    QSqlQuery query;
    query.prepare("SELECT name, qth, grid FROM contacts "
                  "WHERE call = :call ORDER BY ID DESC");
    query.bindValue(":call", ui->callsignEdit->text());
    query.exec();

    if (!query.next()) {
        qDebug() << "first contact";
        return;
    }

    ui->nameEdit->setText(query.value(0).toString());
    ui->locationEdit->setText(query.value(1).toString());
    ui->gridEdit->setText(query.value(2).toString());
}

void NewContactWidget::resetContact() {
    ui->callsignEdit->clear();
    ui->nameEdit->clear();
    ui->locationEdit->clear();
    ui->gridEdit->clear();
}

void NewContactWidget::saveContact() {
    QSqlQuery query;
    query.prepare("INSERT INTO contacts (call, rst_rx, rst_tx, name, qth, grid, time_on, time_off, frequency, band, mode) "
                  "VALUES (:call, :rst_rx, :rst_tx, :name, :qth, :grid, DATETIME('now'), DATETIME('now'), :frequency, :band, :mode)");

    query.bindValue(":call", ui->callsignEdit->text());
    query.bindValue(":rst_rx", ui->rxRstEdit->text());
    query.bindValue(":rst_tx", ui->txRstEdit->text());
    query.bindValue(":name", ui->nameEdit->text());
    query.bindValue(":qth", ui->locationEdit->text());
    query.bindValue(":grid", ui->gridEdit->text());
    query.bindValue(":frequency", QString::number(ui->frequencyEdit->value(), '.', 6));
    query.bindValue(":band", ui->bandEdit->currentText());
    query.bindValue(":mode", ui->modeEdit->currentText());
    query.exec();

    resetContact();
}

NewContactWidget::~NewContactWidget() {
    delete ui;
}
