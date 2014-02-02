#include <QtSql/QtSql>
#include <QShortcut>
#include <QDebug>
#include "core/rig.h"
#include "core/utils.h"
#include "newcontactwidget.h"
#include "ui_newcontactwidget.h"

NewContactWidget::NewContactWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::NewContactWidget)
{
    ui->setupUi(this);

    rig = Rig::instance();

    connect(rig, SIGNAL(frequencyChanged(double)),
            this, SLOT(changeFrequency(double)));

    connect(rig, SIGNAL(modeChanged(QString)),
            this, SLOT(changeMode(QString)));

    connect(rig, SIGNAL(powerChanged(double)),
            this, SLOT(changePower(double)));

    contactTimer = new QTimer(this);
    connect(contactTimer, SIGNAL(timeout()), this, SLOT(updateTimeOff()));

    connect(&callbook, SIGNAL(callsignResult(const QMap<QString, QString>&)),
            this, SLOT(callsignResult(const QMap<QString, QString>&)));

    QStringListModel* rigModel = new QStringListModel(this);
    ui->rigEdit->setModel(rigModel);

    readSettings();
    reloadSettings();
    resetContact();
}

void NewContactWidget::readSettings() {
    QSettings settings;
    QString mode = settings.value("newcontact/mode", "CW").toString();
    double freq = settings.value("newcontact/frequency", 3.5).toDouble();
    QString rig = settings.value("newcontact/rig").toString();

    ui->modeEdit->setCurrentText(mode);
    ui->frequencyEdit->setValue(freq);
    ui->rigEdit->setCurrentText(rig);
}

void NewContactWidget::writeSettings() {
    QSettings settings;
    settings.setValue("newcontact/mode", ui->modeEdit->currentText());
    settings.setValue("newcontact/frequency", ui->frequencyEdit->value());
    settings.setValue("newcontact/rig", ui->rigEdit->currentText());
}

void NewContactWidget::reloadSettings() {
    QString selectedRig = ui->rigEdit->currentText();
    QSettings settings;
    QStringList rigs = settings.value("operator/rigs").toStringList();
    QStringListModel* model = (QStringListModel*)ui->rigEdit->model();
    model->setStringList(rigs);

    if (!selectedRig.isEmpty()) {
        ui->rigEdit->setCurrentText(selectedRig);
    }
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
    }
    else {
        startContactTimer();
        queryDatabase(callsign);
        queryDxcc(callsign);
        callbook.queryCallsign(callsign);
    }
}


void NewContactWidget::queryDxcc(QString callsign) {
    Dxcc* dxcc = cty.lookup(callsign);
    if (dxcc) {
         ui->dxccInfo->setText(dxcc->name);
         ui->cqEdit->setText(QString::number(dxcc->cqZone));
         ui->ituEdit->setText(QString::number(dxcc->ituZone));
         updateCoordinates(dxcc->lat, dxcc->lon, COORD_DXCC);
    }
}

void NewContactWidget::queryDatabase(QString callsign) {
    QSqlQuery query;
    query.prepare("SELECT name, qth, grid FROM contacts "
                  "WHERE callsign = :callsign ORDER BY date DESC LIMIT 1");
    query.bindValue(":callsign", callsign);
    query.exec();

    if (query.next()){
        ui->nameEdit->setText(query.value(0).toString());
        ui->qthEdit->setText(query.value(1).toString());
        ui->gridEdit->setText(query.value(2).toString());
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
    QString band = freqToBand(freq);

    if (band.isEmpty()) {
        band = "OOM!";
    }

    ui->bandText->setText(band);

    rig->setFrequency(freq);
}

void NewContactWidget::modeChanged() {
    QString mode = ui->modeEdit->currentText();
    rig->setMode(mode);
    setDefaultRst();
}

void NewContactWidget::gridChanged() {
    double lat, lon;
    bool valid = gridToCoord(ui->gridEdit->text(), lat, lon);
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
    ui->dxccInfo->clear();
    ui->distanceInfo->clear();
    ui->bearingInfo->clear();
    ui->qslViaEdit->clear();
    ui->cqEdit->clear();
    ui->ituEdit->clear();

    stopContactTimer();
    setDefaultRst();

    ui->callsignEdit->setFocus();
    coordPrec = COORD_NONE;
    emit newTarget(0, 0);
}

void NewContactWidget::saveContact() {
    QSettings settings;
    QSqlQuery query;
    query.prepare("INSERT INTO contacts (callsign, rst_sent, rst_rcvd, name, qth, grid, my_grid, date,"
                  "time_on, time_off, frequency, band, mode, cqz, ituz, tx_power, my_rig, comment, qsl_via) "
                  "VALUES (:callsign, :rst_sent, :rst_rcvd, :name, :qth, :grid, :my_grid, :date,"
                  ":time_on, :time_off, :frequency, :band, :mode, :cqz, :ituz, :tx_power, :my_rig, :comment, :qsl_via)");

    query.bindValue(":callsign", ui->callsignEdit->text());
    query.bindValue(":rst_sent", ui->rstSentEdit->text());
    query.bindValue(":rst_rcvd", ui->rstRcvdEdit->text());
    query.bindValue(":name", ui->nameEdit->text());
    query.bindValue(":qth", ui->qthEdit->text());
    query.bindValue(":grid", ui->gridEdit->text());
    query.bindValue(":my_grid", settings.value("operator/grid"));
    query.bindValue(":date", ui->dateEdit->date().toString(Qt::ISODate));
    query.bindValue(":time_on", ui->timeOnEdit->time().toString(Qt::ISODate));
    query.bindValue(":time_off", ui->timeOffEdit->time().toString(Qt::ISODate));
    query.bindValue(":frequency", ui->frequencyEdit->value());
    query.bindValue(":band", ui->bandText->text());
    query.bindValue(":mode", ui->modeEdit->currentText());
    query.bindValue(":cqz", ui->cqEdit->text());
    query.bindValue(":ituz", ui->ituEdit->text());
    query.bindValue(":tx_power", ui->powerEdit->value());
    query.bindValue(":my_rig", ui->rigEdit->currentText());
    query.bindValue(":comment", ui->commentEdit->toPlainText());
    query.bindValue(":qsl_via", ui->qslViaEdit->text());

    if (query.exec()) {
        resetContact();
        emit contactAdded();
    }
    else {
        qDebug() << query.lastQuery();
        qDebug() << query.lastError();
    }
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
    gridToCoord(myGrid, myLat, myLon);

    double distance = coordDistance(myLat, myLon, lat, lon);
    int bearing = coordBearing(myLat, myLon, lat, lon);

    ui->distanceInfo->setText(QString::number(distance, '.', 1) + " km");
    ui->bearingInfo->setText(QString("%1°").arg(bearing));

    coordPrec = prec;

    emit newTarget(lat, lon);
}

void NewContactWidget::changeFrequency(double freq) {
    ui->frequencyEdit->blockSignals(true);
    ui->frequencyEdit->setValue(freq);
    ui->frequencyEdit->blockSignals(false);
}

void NewContactWidget::changeMode(QString mode) {
    ui->modeEdit->blockSignals(true);
    ui->modeEdit->setCurrentText(mode);
    ui->modeEdit->blockSignals(false);
}

void NewContactWidget::changePower(double power) {
    ui->powerEdit->blockSignals(true);
    ui->powerEdit->setValue(power);
    ui->powerEdit->blockSignals(false);
}

void NewContactWidget::tuneDx(QString callsign, double frequency) {
    resetContact();
    ui->callsignEdit->setText(callsign);
    ui->frequencyEdit->setValue(frequency);
    callsignChanged();
    stopContactTimer();
}

void NewContactWidget::setDefaultRst() {
    QString mode = ui->modeEdit->currentText();
    if (mode == "SSB" || mode == "FM" || mode == "AM") {
        ui->rstRcvdEdit->setText("59");
        ui->rstSentEdit->setText("59");
    }
    else {
        ui->rstRcvdEdit->setText("599");
        ui->rstSentEdit->setText("599");
    }
}

NewContactWidget::~NewContactWidget() {
    writeSettings();
    delete ui;
}
