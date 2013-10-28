#include <QSettings>
#include "settingsdialog.h"
#include "ui_settingsdialog.h"

SettingsDialog::SettingsDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::SettingsDialog)
{
    ui->setupUi(this);
    readSettings();
}

void SettingsDialog::save() {
    writeSettings();
    accept();
}

void SettingsDialog::readSettings() {
    QSettings settings;
    ui->callsignEdit->setText(settings.value("operator/callsign").toString());
    ui->locatorEdit->setText(settings.value("operator/locator").toString());
}

void SettingsDialog::writeSettings() {
    QSettings settings;

    settings.setValue("operator/callsign", ui->callsignEdit->text());
    settings.setValue("operator/locator", ui->locatorEdit->text());
}

SettingsDialog::~SettingsDialog() {
    delete ui;
}
