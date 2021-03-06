#include <QSettings>
#include "ui/dbdialog.h"
#include "ui_dbdialog.h"

DbDialog::DbDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DbDialog)
{
    ui->setupUi(this);

    QSettings settings;

    ui->hostnameEdit->setText(settings.value("db/hostname", "localhost").toString());
    ui->portEdit->setValue(settings.value("db/port", 3306).toInt());
    ui->dbnameEdit->setText(settings.value("db/dbname", "openshack").toString());
    ui->usernameEdit->setText(settings.value("db/username").toString());
    ui->passwordEdit->setText(settings.value("db/password").toString());
}

void DbDialog::connect() {
    QSettings settings;
    settings.setValue("db/hostname", ui->hostnameEdit->text());
    settings.setValue("db/port", ui->portEdit->value());
    settings.setValue("db/dbname", ui->dbnameEdit->text());
    settings.setValue("db/username", ui->usernameEdit->text());
    settings.setValue("db/password", ui->passwordEdit->text());
    accept();
}

void DbDialog::quit() {
    reject();
}

DbDialog::~DbDialog()
{
    delete ui;
}
