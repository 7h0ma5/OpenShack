#include <QSettings>
#include <QFileDialog>
#include "mainwindow.h"
#include "settingsdialog.h"
#include "ui_mainwindow.h"
#include "adif.h"

MainWindow::MainWindow(QWidget* parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    QSettings settings;
    restoreGeometry(settings.value("geometry").toByteArray());
    restoreState(settings.value("windowState").toByteArray());
}

void MainWindow::closeEvent(QCloseEvent* event) {
    QSettings settings;
    settings.setValue("geometry", saveGeometry());
    settings.setValue("windowState", saveState());
    QMainWindow::closeEvent(event);
}

void MainWindow::showSettings() {
    SettingsDialog sw;
    sw.exec();
}

void MainWindow::importAdif() {
    QString filename = QFileDialog::getOpenFileName(this, "ADIF File", "", "*.adi");

    Adif adif;
    adif.importFromFile(filename);
}

void MainWindow::exportAdif() {
    QString filename = QFileDialog::getSaveFileName(this, "ADIF File", "logbook.adi", "*.adi");

    Adif adif;
    adif.exportToFile(filename);
}

MainWindow::~MainWindow() {
    delete ui;
}
