#include <QtSql/QtSql>
#include <QDebug>
#include "rig.h"
#include "hamqth.h"
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

    callbook = new HamQTH(this);
    connect(callbook, SIGNAL(callsignResult(const QMap<QString, QString>&)),
            this, SLOT(callsignResult(const QMap<QString, QString>&)));

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

    callbook->queryCallsign(ui->callsignEdit->text());

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

void NewContactWidget::callsignResult(const QMap<QString, QString>& data) {
    if (!data.value("name").isEmpty() && ui->nameEdit->text().isEmpty()) {
        ui->nameEdit->setText(data.value("name"));
    }

    if (!data.value("grid").isEmpty() && ui->gridEdit->text().isEmpty()) {
        ui->gridEdit->setText(data.value("grid"));
    }

    if (!data.value("location").isEmpty() && ui->locationEdit->text().isEmpty()) {
        ui->locationEdit->setText(data.value("location"));
    }
}

void NewContactWidget::frequencyChanged() {
    double freq = ui->frequencyEdit->value();
    QString band = "OOB!";

    if (freq <= 2.0 && freq >= 1.8) band = "160m";
    else if (freq <= 3.8 && freq >= 3.5) band = "80m";
    else if (freq <= 7.5 && freq >= 7.0) band = "40m";
    else if (freq <= 10.150 && freq >= 10.1) band = "30m";
    else if (freq <= 14.350 && freq >= 14.0) band = "20m";
    else if (freq <= 18.168 && freq >= 18.068) band = "17m";
    else if (freq <= 21.450 && freq >= 21.000) band = "15m";
    else if (freq <= 24.990 && freq >= 24.890) band = "12m";
    else if (freq <= 29.700 && freq >= 28.000) band = "10m";
    else if (freq <= 52 && freq >= 50)  band = "6m";
    else if (freq <= 148 && freq >= 144) band = "2m";
    else if (freq <= 440 && freq >= 430) band = "70cm";

    ui->bandText->setText(band);
    Rig::setFrequency(freq*1e6);
}

void NewContactWidget::gridChanged() {
    QSettings settings;
    QString myGrid = settings.value("operator/grid").toString();

    double distance;
    bool valid = grid_distance(myGrid, ui->gridEdit->text(), distance);

    if (!valid) {
        ui->distanceInfo->setText("Invalid Grid");
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
    query.prepare("INSERT INTO contacts (call, rst_rx, rst_tx, name, qth, grid, date, time_on, time_off, frequency, band, mode, power, comment) "
                  "VALUES (:call, :rst_rx, :rst_tx, :name, :qth, :grid, DATE('now'), TIME('now'), TIME('now'), :frequency, :band, :mode, :power, :comment)");

    query.bindValue(":call", ui->callsignEdit->text());
    query.bindValue(":rst_rx", ui->rxRstEdit->text());
    query.bindValue(":rst_tx", ui->txRstEdit->text());
    query.bindValue(":name", ui->nameEdit->text());
    query.bindValue(":qth", ui->locationEdit->text());
    query.bindValue(":grid", ui->gridEdit->text());
    query.bindValue(":frequency", QString::number(ui->frequencyEdit->value(), '.', 6));
    query.bindValue(":band", ui->bandText->text());
    query.bindValue(":mode", ui->modeEdit->currentText());
    query.bindValue(":power", QString::number(ui->powerEdit->value(), '.', 2));
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
