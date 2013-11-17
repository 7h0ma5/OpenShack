#include <QtSql/QtSql>
#include <QShortcut>
#include <QDebug>
#include "rig.h"
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

    connect(&callbook, SIGNAL(callsignResult(const QMap<QString, QString>&)),
            this, SLOT(callsignResult(const QMap<QString, QString>&)));

    resetContact();
}

void NewContactWidget::callsignChanged() {
    if (ui->callsignEdit->text() == callsign) {
        return;
    }
    else {
        callsign = ui->callsignEdit->text();
    }

    ui->timeOnEdit->setTime(QDateTime::currentDateTimeUtc().time());
    ui->timeOffEdit->setTime(QDateTime::currentDateTimeUtc().time());

    if (callsign.isEmpty()) {
        stopContactTimer();
        return;
    }

    startContactTimer();

    QSqlQuery query;
    query.prepare("SELECT name, qth, grid, date FROM contacts "
                  "WHERE call = :call ORDER BY ID DESC");
    query.bindValue(":call", callsign);
    query.exec();

    if (!query.next()) {
        ui->contactInfo->setText("First Contact!");
    }
    else {
        ui->nameEdit->setText(query.value(0).toString());
        ui->qthEdit->setText(query.value(1).toString());
        ui->gridEdit->setText(query.value(2).toString());
        ui->contactInfo->setText(query.value(3).toString());
    }

    callbook.queryCallsign(callsign);

    Dxcc* dxcc = cty.lookup(callsign);
    if (dxcc) {
         ui->dxccInfo->setText(dxcc->name);
         ui->cqEdit->setText(QString::number(dxcc->cqZone));
         ui->ituEdit->setText(QString::number(dxcc->ituZone));
         updateCoordinates(dxcc->lat, dxcc->lon, COORD_DXCC);
    }
}

void NewContactWidget::callsignResult(const QMap<QString, QString>& data) {
    if (!data.value("name").isEmpty() && ui->nameEdit->text().isEmpty()) {
        ui->nameEdit->setText(data.value("name"));
    }

    if (!data.value("grid").isEmpty() && ui->gridEdit->text().isEmpty()) {
        ui->gridEdit->setText(data.value("grid"));
    }

    if (!data.value("qth").isEmpty() && ui->qthEdit->text().isEmpty()) {
        ui->qthEdit->setText(data.value("qth"));
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
    double lat, lon;
    bool valid = grid_to_coord(ui->gridEdit->text(), lat, lon);
    if (!valid) return;
    updateCoordinates(lat, lon, COORD_GRID);
}

void NewContactWidget::resetContact() {
    ui->dateEdit->setDate(QDate::currentDate());
    ui->timeOnEdit->setTime(QDateTime::currentDateTimeUtc().time());
    ui->timeOffEdit->setTime(QDateTime::currentDateTimeUtc().time());
    ui->callsignEdit->clear();
    ui->nameEdit->clear();
    ui->qthEdit->clear();
    ui->gridEdit->clear();
    ui->commentEdit->clear();
    ui->contactInfo->clear();
    ui->dxccInfo->clear();
    ui->distanceInfo->clear();
    ui->qslViaEdit->clear();
    stopContactTimer();
    ui->callsignEdit->setFocus();

    coordPrec = COORD_NONE;
    emit newTarget(0, 0);
}

void NewContactWidget::saveContact() {
    QSettings settings;
    QSqlQuery query;
    query.prepare("INSERT INTO contacts (call, rst_rx, rst_tx, name, qth, grid, my_grid, date,"
                  "time_on, time_off, frequency, band, mode, cqz, ituz, power, rig, comment, qsl_via) "
                  "VALUES (:call, :rst_rx, :rst_tx, :name, :qth, :grid, :my_grid, :date,"
                  ":time_on, :time_off, :frequency, :band, :mode, :cqz, :ituz, :power, :rig, :comment, :qsl_via)");

    query.bindValue(":call", ui->callsignEdit->text());
    query.bindValue(":rst_rx", ui->rxRstEdit->text());
    query.bindValue(":rst_tx", ui->txRstEdit->text());
    query.bindValue(":name", ui->nameEdit->text());
    query.bindValue(":qth", ui->qthEdit->text());
    query.bindValue(":grid", ui->gridEdit->text());
    query.bindValue(":my_grid", settings.value("operator/grid"));
    query.bindValue(":date", ui->dateEdit->date().toString(Qt::ISODate));
    query.bindValue(":time_on", ui->timeOnEdit->time().toString(Qt::ISODate));
    query.bindValue(":time_off", ui->timeOffEdit->time().toString(Qt::ISODate));
    query.bindValue(":frequency", QString::number(ui->frequencyEdit->value(), '.', 4));
    query.bindValue(":band", ui->bandText->text());
    query.bindValue(":mode", ui->modeEdit->currentText());
    query.bindValue(":cqz", ui->cqEdit->text());
    query.bindValue(":ituz", ui->ituEdit->text());
    query.bindValue(":power", QString::number(ui->powerEdit->value(), '.', 2));
    query.bindValue(":rig", ui->rigEdit->currentText());
    query.bindValue(":comment", ui->commentEdit->text());
    query.bindValue(":qsl_via", ui->qslViaEdit->text());
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
    ui->timeOffEdit->setTime(QDateTime::currentDateTimeUtc().time());
}

void NewContactWidget::updateCoordinates(double lat, double lon, CoordPrecision prec) {
    if (prec <= coordPrec) return;

    QSettings settings;
    QString myGrid = settings.value("operator/grid").toString();

    double myLat, myLon;
    grid_to_coord(myGrid, myLat, myLon);

    double distance = coord_distance(myLat, myLon, lat, lon);

    ui->distanceInfo->setText(QString::number(distance, '.', 1) + " km");

    coordPrec = prec;

    emit newTarget(lat, lon);
}

NewContactWidget::~NewContactWidget() {
    delete ui;
}
