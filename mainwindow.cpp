#include <QSettings>
#include <QFileDialog>
#include <QLabel>
#include "mainwindow.h"
#include "settingsdialog.h"
#include "ui_mainwindow.h"
#include "adif.h"
#include "fldigi.h"
#include "rig.h"

MainWindow::MainWindow(QWidget* parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    QSettings settings;

    // restore the window geometry
    restoreGeometry(settings.value("geometry").toByteArray());
    restoreState(settings.value("windowState").toByteArray());

    QString op = settings.value("operator/callsign", "NOCALL").toString();
    QString grid  = settings.value("operator/grid", "NO GRID").toString();

    ui->statusBar->addWidget(new QLabel(op, ui->statusBar));
    ui->statusBar->addWidget(new QLabel(grid, ui->statusBar));

    Fldigi* fldigi = new Fldigi(this);
    connect(fldigi, SIGNAL(contactAdded()), ui->logbookWidget, SLOT(updateTable()));
}

void MainWindow::closeEvent(QCloseEvent* event) {
    QSettings settings;

    // save the window geometry
    settings.setValue("geometry", saveGeometry());
    settings.setValue("windowState", saveState());

    QMainWindow::closeEvent(event);
}

void MainWindow::rigConnect() {
    Rig::instance()->open();
}

void MainWindow::showSettings() {
    SettingsDialog sw;
    sw.exec();
}

void MainWindow::importAdif() {
    QString filename = QFileDialog::getOpenFileName(this, "ADIF File", "", "*.adi");

    ui->statusBar->showMessage(QString("Importing %1...").arg(filename));
    Adif adif;
    int count = adif.importFromFile(filename);
    ui->statusBar->showMessage(QString("Imported %1 contacts.").arg(count), 5000);
    ui->logbookWidget->updateTable();
}

void MainWindow::exportAdif() {
    QString filename = QFileDialog::getSaveFileName(this, "ADIF File", "logbook.adi", "*.adi");

    ui->statusBar->showMessage(QString("Exporting %1...").arg(filename));
    Adif adif;
    int count = adif.exportToFile(filename);
    ui->statusBar->showMessage(QString("Exported %1 contacts.").arg(count), 5000);
}

MainWindow::~MainWindow() {
    delete ui;
}
