#include <QFileDialog>
#include "ui/exportdialog.h"
#include "ui_exportdialog.h"
#include "core/adif.h"

ExportDialog::ExportDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ExportDialog)
{
    ui->setupUi(this);

    ui->allCheckBox->setChecked(true);
    ui->startDateEdit->setDate(QDate::currentDate());
    ui->endDateEdit->setDate(QDate::currentDate());
}

void ExportDialog::browse() {
    QString filename = QFileDialog::getSaveFileName(this, "ADIF File", "logbook.adi", "*.adi");
    ui->fileEdit->setText(filename);
}

void ExportDialog::toggleAll() {
    ui->startDateEdit->setEnabled(!ui->allCheckBox->isChecked());
    ui->endDateEdit->setEnabled(!ui->allCheckBox->isChecked());
}

void ExportDialog::runExport() {
    QFile file(ui->fileEdit->text());
    file.open(QFile::WriteOnly | QFile::Text);
    QTextStream out(&file);

    Adif adif(out);

    if (!ui->allCheckBox->isChecked()) {
        adif.setDateRange(ui->startDateEdit->date(), ui->endDateEdit->date());
    }

    int count = adif.runExport();

    ui->statusLabel->setText(tr("Exported %n contacts.", "", count));
}

ExportDialog::~ExportDialog() {
    delete ui;
}
