#include <QtSql/QtSql>
#include <QDebug>
#include "newcontactwidget.h"
#include "ui_newcontactwidget.h"
#include "utils.h"

NewContactWidget::NewContactWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::NewContactWidget)
{
    ui->setupUi(this);

    contactTimer = new QTimer(this);
    connect(contactTimer, SIGNAL(timeout()), this, SLOT(updateTimeOff()));

    resetContact();
}

void NewContactWidget::callsignChanged() {
    ui->timeOnEdit->setTime(QTime::currentTime());
    ui->timeOffEdit->setTime(QTime::currentTime());

    if (ui->callsignEdit->text().isEmpty()) {
        stopContactTimer();
        return;
    }

    startContactTimer();

    QSqlQuery query;
    query.prepare("SELECT name, qth, grid, date FROM contacts "
                  "WHERE call = :call ORDER BY ID DESC");
    query.bindValue(":call", ui->callsignEdit->text());
    query.exec();

    ui->contactInfo->setText(QString::number(query.size()));

    if (!query.next()) {
        ui->contactInfo->setText("First Contact!");
    }
    else {
        ui->nameEdit->setText(query.value(0).toString());
        ui->locationEdit->setText(query.value(1).toString());
        ui->gridEdit->setText(query.value(2).toString());
        ui->contactInfo->setText(query.value(3).toString());
    }
}

void NewContactWidget::gridChanged() {
    QSettings settings;
    QString myGrid = settings.value("operator/grid").toString();

    double distance;
    bool valid = grid_distance(myGrid, ui->gridEdit->text(), distance);

    if (!valid) {
        ui->distanceInfo->setText("");
        return;
    }

    ui->distanceInfo->setText(QString::number(distance, '.',0) + " km");
}

void NewContactWidget::resetContact() {
    ui->dateEdit->setDate(QDate::currentDate());
    ui->timeOnEdit->setTime(QTime::currentTime());
    ui->timeOffEdit->setTime(QTime::currentTime());
    ui->callsignEdit->clear();
    ui->nameEdit->clear();
    ui->locationEdit->clear();
    ui->gridEdit->clear();
    ui->commentEdit->clear();
    stopContactTimer();
}

void NewContactWidget::saveContact() {
    QSqlQuery query;
    query.prepare("INSERT INTO contacts (call, rst_rx, rst_tx, name, qth, grid, date, time_on, time_off, frequency, band, mode, comment) "
                  "VALUES (:call, :rst_rx, :rst_tx, :name, :qth, :grid, DATE('now'), TIME('now'), TIME('now'), :frequency, :band, :mode, :comment)");

    query.bindValue(":call", ui->callsignEdit->text());
    query.bindValue(":rst_rx", ui->rxRstEdit->text());
    query.bindValue(":rst_tx", ui->txRstEdit->text());
    query.bindValue(":name", ui->nameEdit->text());
    query.bindValue(":qth", ui->locationEdit->text());
    query.bindValue(":grid", ui->gridEdit->text());
    query.bindValue(":frequency", QString::number(ui->frequencyEdit->value(), '.', 6));
    query.bindValue(":band", ui->bandEdit->currentText());
    query.bindValue(":mode", ui->modeEdit->currentText());
    query.bindValue(":comment", ui->commentEdit->toPlainText());
    query.exec();

    resetContact();

    emit contactAdded();
}

void NewContactWidget::startContactTimer() {
    if (!contactTimer->isActive()) {
        contactTimer->start(1000);
        ui->stopContactTimerButton->setEnabled(true);
    }
}

void NewContactWidget::stopContactTimer() {
    if (contactTimer->isActive()) {
        contactTimer->stop();
        ui->stopContactTimerButton->setEnabled(false);
    }
}

void NewContactWidget::updateTimeOff() {
    ui->timeOffEdit->setTime(QTime::currentTime());
}

NewContactWidget::~NewContactWidget() {
    delete ui;
}
