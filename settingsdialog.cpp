#include <QSettings>
#include "settingsdialog.h"
#include "ui_settingsdialog.h"
#include "rig.h"

SettingsDialog::SettingsDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::SettingsDialog)
{
    ui->setupUi(this);

    RigTypeModel* rigTypeModel = new RigTypeModel(this);
    ui->rigModelSelect->setModel(rigTypeModel);

    readSettings();
}

void SettingsDialog::save() {
    writeSettings();
    accept();
}

void SettingsDialog::readSettings() {
    QSettings settings;
    ui->callsignEdit->setText(settings.value("operator/callsign").toString());
    ui->locatorEdit->setText(settings.value("operator/grid").toString());

    ui->rigModelSelect->setCurrentIndex(settings.value("rig/modelrow").toInt());
    ui->rigPortEdit->setText(settings.value("rig/port").toString());
    ui->rigBaudEdit->setValue(settings.value("rig/baudrate").toInt());

    ui->hamQthUsernameEdit->setText(settings.value("hamqth/username").toString());
    ui->hamQthPasswordEdit->setText(settings.value("hamqth/password").toString());
}

void SettingsDialog::writeSettings() {
    QSettings settings;

    settings.setValue("operator/callsign", ui->callsignEdit->text());
    settings.setValue("operator/grid", ui->locatorEdit->text());

    int row = ui->rigModelSelect->currentIndex();
    QModelIndex index = ui->rigModelSelect->model()->index(row, 0);
    settings.setValue("rig/model", index.internalId());
    settings.setValue("rig/modelrow", row);
    settings.setValue("rig/port", ui->rigPortEdit->text());
    settings.setValue("rig/baudrate", ui->rigBaudEdit->value());

    settings.setValue("hamqth/username", ui->hamQthUsernameEdit->text());
    settings.setValue("hamqth/password", ui->hamQthPasswordEdit->text());
}

SettingsDialog::~SettingsDialog() {
    delete ui;
}
