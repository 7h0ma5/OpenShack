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

    ui->fromDateEdit->setDate(QDate::currentDate());
    ui->toDateEdit->setDate(QDate::currentDate());
    ui->gridEdit->setText(settings.value("operator/grid").toString());
}

void ImportDialog::browse() {
    QString filename = QFileDialog::getOpenFileName(this, "ADIF File", "", "*.adi");
    ui->fileEdit->setText(filename);
}

void ImportDialog::toggleAll() {
    ui->fromDateEdit->setEnabled(!ui->allCheckBox->isChecked());
    ui->toDateEdit->setEnabled(!ui->allCheckBox->isChecked());
}

void ImportDialog::import() {
    QFile file(ui->fileEdit->text());
    file.open(QFile::ReadOnly | QFile::Text);
    QTextStream in(&file);

    Adif adif(in);
    int count = adif.runImport();

    ui->statusLabel->setText(tr("Imported %n contacts.", "", count));
}

ImportDialog::~ImportDialog()
{
    delete ui;
}
