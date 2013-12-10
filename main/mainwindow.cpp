#include <QSettings>
#include <QFileDialog>
#include <QMessageBox>
#include <QLabel>
#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "dialog/settingsdialog.h"
#include "dialog/importdialog.h"
#include "logformat/adif.h"
#include "interface/fldigi.h"
#include "interface/rig.h"

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
    ImportDialog dialog;
    dialog.exec();
    ui->logbookWidget->updateTable();
}

void MainWindow::exportAdif() {
    QString filename = QFileDialog::getSaveFileName(this, "ADIF File", "logbook.adi", "*.adi");

    QFile file(filename);
    file.open(QFile::WriteOnly | QFile::Text);
    QTextStream stream(&file);

    ui->statusBar->showMessage(tr("Exporting %1...").arg(filename));
    Adif adif(stream);
    int count = adif.runExport();
    ui->statusBar->showMessage(tr("Exported %n contacts.", "", count), 5000);
}

void MainWindow::showAbout() {
    QString aboutText = "<h1>OpenShack %1</h1>"
                        "<p>&copy; 2013 Thomas Gatzweiler DL2IC</p>"
                        "<p><a href='http://dl2ic.de'>www.dl2ic.de</a></p>";

    QString version = QCoreApplication::applicationVersion();
    aboutText = aboutText.arg(version);

    QMessageBox::about(this, "About", aboutText);
}

MainWindow::~MainWindow() {
    delete ui;
}