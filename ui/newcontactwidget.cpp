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

    connect(rig, &Rig::frequencyChanged,
            this, &NewContactWidget::changeFrequency);

    connect(rig, &Rig::modeChanged,
            this, &NewContactWidget::changeMode);

    connect(rig, &Rig::powerChanged,
            this, &NewContactWidget::changePower);

    contactTimer = new QTimer(this);
    connect(contactTimer, &QTimer::timeout, this, &NewContactWidget::updateTimeOff);

    connect(&callbook, &HamQTH::callsignResult, this, &NewContactWidget::callsignResult);

    new QShortcut(QKeySequence(Qt::Key_Escape), this, SLOT(resetContact()), 0, Qt::ApplicationShortcut);
    new QShortcut(QKeySequence(Qt::Key_F10), this, SLOT(saveContact()), 0, Qt::ApplicationShortcut);
    new QShortcut(QKeySequence(Qt::Key_F9), this, SLOT(stopContactTimer()), 0, Qt::ApplicationShortcut);

    QStringListModel* rigModel = new QStringListModel(this);
    ui->rigEdit->setModel(rigModel);

    reloadSettings();
    readSettings();
    resetContact();
}

void NewContactWidget::readSettings() {
    QSettings settings;
    QString mode = settings.value("newcontact/mode", "CW").toString();
    double freq = settings.value("newcontact/frequency", 3.5).toDouble();
    QString rig = settings.value("newcontact/rig").toString();
    double power = settings.value("newcontact/power", 100).toDouble();

    ui->modeEdit->setCurrentText(mode);
    ui->frequencyEdit->setValue(freq);
    ui->rigEdit->setCurrentText(rig);
    ui->powerEdit->setValue(power);
}

void NewContactWidget::writeSettings() {
    QSettings settings;
    settings.setValue("newcontact/mode", ui->modeEdit->currentText());
    settings.setValue("newcontact/frequency", ui->frequencyEdit->value());
    settings.setValue("newcontact/rig", ui->rigEdit->currentText());
    settings.setValue("newcontact/power", ui->powerEdit->value());
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

    updateTime();

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
        ui->callsignEdit->setStyleSheet("background-color: #99ff99;");
    }
    else {
        ui->callsignEdit->setStyleSheet("");
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

    if (!data.value("qsl_via").isEmpty() && ui->qslViaEdit->text().isEmpty()) {
        ui->qslViaEdit->setText(data.value("qsl_via"));
    }

    if (ui->callsignEdit->styleSheet().isEmpty()) {
        ui->callsignEdit->setStyleSheet("background-color: #bbddff;");
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
    updateTime();
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

    ui->callsignEdit->setStyleSheet("");
    ui->callsignEdit->setFocus();
    callsign = QString();
    coordPrec = COORD_NONE;
    emit newTarget(0, 0);
}

void NewContactWidget::saveContact() {
    QSettings settings;
    QSqlTableModel model;
    model.setTable("contacts");

    QSqlRecord record = model.record();
    record.setValue("id", QVariant());
    record.setValue("callsign", ui->callsignEdit->text());
    record.setValue("rst_sent", ui->rstSentEdit->text());
    record.setValue("rst_rcvd", ui->rstRcvdEdit->text());
    record.setValue("name", ui->nameEdit->text());
    record.setValue("qth", ui->qthEdit->text());
    record.setValue("grid", ui->gridEdit->text());
    record.setValue("my_grid", settings.value("operator/grid"));
    record.setValue("date", ui->dateEdit->date().toString(Qt::ISODate));
    record.setValue("time_on", ui->timeOnEdit->time().toString(Qt::ISODate));
    record.setValue("time_off", ui->timeOffEdit->time().toString(Qt::ISODate));
    record.setValue("frequency", ui->frequencyEdit->value());
    record.setValue("band", ui->bandText->text());
    record.setValue("mode", ui->modeEdit->currentText());
    record.setValue("cqz", ui->cqEdit->text().toInt());
    record.setValue("ituz", ui->ituEdit->text().toInt());
    record.setValue("tx_power", ui->powerEdit->value());
    record.setValue("my_rig", ui->rigEdit->currentText());
    record.setValue("comment", ui->commentEdit->toPlainText());
    record.setValue("qsl_via", ui->qslViaEdit->text());

    if (!model.insertRecord(-1, record)) {
        qDebug() << model.lastError();
        return;
    }

    if (!model.submitAll()) {
        qDebug() << model.lastError();
        return;
    }

    resetContact();
    emit contactAdded();
}

void NewContactWidget::startContactTimer() {
    if (!contactTimer->isActive()) {
        contactTimer->start(1000);
    }
}

void NewContactWidget::stopContactTimer() {
    if (contactTimer->isActive()) {
        contactTimer->stop();
    }
    updateTimeOff();
}

void NewContactWidget::updateTime() {
    ui->dateEdit->setDate(QDate::currentDate());
    ui->timeOnEdit->setTime(QDateTime::currentDateTimeUtc().time());
    ui->timeOffEdit->setTime(QDateTime::currentDateTimeUtc().time());
    startContactTimer();
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
