#include <QApplication>
#include <QtSql/QtSql>
#include <QMessageBox>
#include <QResource>
#include <QDir>
#include <QStandardPaths>
#include <QDebug>
#include "migration.h"
#include "ui/mainwindow.h"
#include "rig.h"

int main(int argc, char* argv[]) {
    QApplication app(argc, argv);

    app.setApplicationVersion(VERSION);
    app.setOrganizationName("DL2IC");
    app.setApplicationName("OpenShack");

    /*
     * Load stylesheet
     */
    QFile style(":/stylesheet.css");
    style.open(QFile::ReadOnly | QIODevice::Text);
    app.setStyleSheet(style.readAll());
    style.close();

    /*
     * Setup QTranslator
     */
    QTranslator qtTranslator;
    qtTranslator.load("qt_" + QLocale::system().name(),
            QLibraryInfo::location(QLibraryInfo::TranslationsPath));
    app.installTranslator(&qtTranslator);

    QTranslator translator;
    translator.load(":/i18n/openshack_" + QLocale::system().name().left(2));
    app.installTranslator(&translator);

    /*
     * Create data directory
     */
    QDir dataDir(QStandardPaths::writableLocation(QStandardPaths::DataLocation));

    if (!dataDir.exists()) {
        dataDir.mkpath(dataDir.path());
    }

    /*
     * Open Database
     */
    QSqlDatabase db;
    db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName(dataDir.filePath("openshack.db3"));

    // Quit if the connection to the DB fails
    if (!db.open()) {
        QMessageBox::critical(NULL, QMessageBox::tr("OpenShack Error"),
                              QMessageBox::tr("Could not connect to database."));
        return 1;
    }

    // Migrate database
    Migration m;
    if (!m.run()) {
        QMessageBox::critical(NULL, QMessageBox::tr("OpenShack Error"),
                              QMessageBox::tr("Database migration failed."));
        return 1;
    }

    /*
     * Create MainWindow
     */
    MainWindow w;

    QIcon icon(":/res/openshack.png");
    w.setWindowIcon(icon);

    w.show();

    /*
     * Start rig worker thread
     */
    QThread* rigThread = new QThread;
    Rig* rig = Rig::instance();
    rig->moveToThread(rigThread);
    QObject::connect(rigThread, SIGNAL(started()), rig, SLOT(start()));
    rigThread->start();

    return app.exec();
}
