#include <QApplication>
#include <QtSql/QtSql>
#include <QMessageBox>
#include <QResource>
#include <QDir>
#include <QStandardPaths>
#include <QDebug>
#include "mainwindow.h"
#include "rig.h"

int main(int argc, char* argv[]) {
    QApplication app(argc, argv);

    QCoreApplication::setOrganizationName("DL2IC");
    QCoreApplication::setApplicationName("OpenShack");

    QTranslator qtTranslator;
    qtTranslator.load("qt_" + QLocale::system().name(),
            QLibraryInfo::location(QLibraryInfo::TranslationsPath));
    app.installTranslator(&qtTranslator);

    QTranslator translator;
    translator.load(":/i18n/openshack_" + QLocale::system().name().left(2));
    app.installTranslator(&translator);

    QDir dataDir(QStandardPaths::writableLocation(QStandardPaths::DataLocation));

    if (!dataDir.exists()) {
        dataDir.mkpath(dataDir.path());
    }

    QSqlDatabase db;
    db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName(dataDir.filePath("openshack.db3"));

    // Quit if the connection to the DB fails
    if (!db.open()) {
        QMessageBox::critical(NULL, QMessageBox::tr("OpenShack Error"),
                              QMessageBox::tr("Could not connect to database."));
        return 0;
    }

    // Create tables
    QSqlQuery query;
    query.exec("CREATE TABLE IF NOT EXISTS contacts"
               "(id INTEGER PRIMARY KEY,"
               "date TEXT, time_on TEXT, time_off TEXT,"
               "call TEXT NOT NULL,"
               "rst_rx TEXT, rst_tx TEXT,"
               "name TEXT, qth TEXT, grid TEXT, my_grid TEXT,"
               "cqz INTEGER, ituz INTEGER,"
               "frequency TEXT, band TEXT, mode TEXT,"
               "rig TEXT, power TEXT,"
               "comment TEXT, qsl_via TEXT)");

    MainWindow w;
    w.show();

    //Rig::connect();

    return app.exec();
}
