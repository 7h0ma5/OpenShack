#include <QFileDialog>
#include "importdialog.h"
#include "ui_importdialog.h"
#include "logformat/adif.h"

ImportDialog::ImportDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ImportDialog)
{
    ui->setupUi(this);

    QSettings settings;

    ui->allCheckBox->setChecked(true);
    ui->startDateEdit->setDate(QDate::currentDate());
    ui->endDateEdit->setDate(QDate::currentDate());
    ui->gridEdit->setText(settings.value("operator/grid").toString());
}

void ImportDialog::browse() {
    QString filename = QFileDialog::getOpenFileName(this, "ADIF File", "", "*.adi");
    ui->fileEdit->setText(filename);
}

void ImportDialog::toggleAll() {
    ui->startDateEdit->setEnabled(!ui->allCheckBox->isChecked());
    ui->endDateEdit->setEnabled(!ui->allCheckBox->isChecked());
}

void ImportDialog::import() {
    QFile file(ui->fileEdit->text());
    file.open(QFile::ReadOnly | QFile::Text);
    QTextStream in(&file);

    QMap<QString, QString> defaults;

    if (ui->gridCheckBox->isChecked()) {
        defaults["my_grid"] = ui->gridEdit->text();
    }

    if (ui->commentCheckBox->isChecked()) {
        defaults["comment"] = ui->commentEdit->text();
    }

    Adif adif(in);
    adif.setDefaults(defaults);

    if (!ui->allCheckBox->isChecked()) {
        adif.setDateRange(ui->startDateEdit->date(), ui->endDateEdit->date());
    }

    int count = adif.runImport();

    ui->statusLabel->setText(tr("Imported %n contacts.", "", count));
}

ImportDialog::~ImportDialog()
{
    delete ui;
}
