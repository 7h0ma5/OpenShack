#include <QSettings>
#include "settingsdialog.h"
#include "ui_settingsdialog.h"
#include "rig.h"

SettingsDialog::SettingsDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::SettingsDialog)
{
    ui->setupUi(this);

    QStringListModel* rigModel = new QStringListModel();
    rigModel->setStringList(Rig::allModels());
    ui->rigModelSelect->setModel(rigModel);

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
}

void SettingsDialog::writeSettings() {
    QSettings settings;

    settings.setValue("operator/callsign", ui->callsignEdit->text());
    settings.setValue("operator/grid", ui->locatorEdit->text());
}

SettingsDialog::~SettingsDialog() {
    delete ui;
}
