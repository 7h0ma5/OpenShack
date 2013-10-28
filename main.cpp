#include <QApplication>
#include <QtSql/QtSql>
#include <QMessageBox>
#include <QResource>
#include <QDebug>
#include "mainwindow.h"
#include <hamlib/rig.h>

int main(int argc, char* argv[]) {
    RIG* myrig = rig_init(215);
    strncpy(myrig->state.rigport.pathname, "COM1", FILPATHLEN-1);

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

    QSqlDatabase db;
    db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName("openshack.db3");

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
               "callsign TEXT NOT NULL,"
               "tx_rst TEXT,"
               "rx_rst TEXT,"
               "name TEXT,"
               "location TEXT,"
               "locator TEXT,"
               "time TEXT NOT NULL)");

    MainWindow w;
    w.show();

    return app.exec();
}
