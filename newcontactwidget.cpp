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
    query.prepare("SELECT name, location, locator FROM contacts "
                  "WHERE callsign = :callsign ORDER BY ID DESC");
    query.bindValue(":callsign", ui->callsignEdit->text());
    query.exec();

    if (!query.next()) {
        qDebug() << "first contact";
        return;
    }

    ui->nameEdit->setText(query.value(0).toString());
    ui->locationEdit->setText(query.value(1).toString());
    ui->locatorEdit->setText(query.value(2).toString());
}

void NewContactWidget::resetContact() {
    ui->callsignEdit->clear();
    ui->nameEdit->clear();
    ui->locationEdit->clear();
    ui->locatorEdit->clear();
}

void NewContactWidget::saveContact() {
    QSqlQuery query;
    query.prepare("INSERT INTO contacts (callsign, tx_rst, rx_rst, name, location, locator, time) "
                  "VALUES (:callsign, :tx_rst, :rx_rst, :name, :location, :locator, DATETIME('now'))");

    query.bindValue(":callsign", ui->callsignEdit->text());
    query.bindValue(":tx_rst", ui->txRstEdit->text());
    query.bindValue(":rx_rst", ui->rxRstEdit->text());
    query.bindValue(":name", ui->nameEdit->text());
    query.bindValue(":location", ui->locationEdit->text());
    query.bindValue(":locator", ui->locatorEdit->text());
    query.exec();

    resetContact();
}

NewContactWidget::~NewContactWidget() {
    delete ui;
}
