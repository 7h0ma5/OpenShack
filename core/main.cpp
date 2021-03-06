#include <QApplication>
#include <QtSql/QtSql>
#include <QMessageBox>
#include <QResource>
#include <QDir>
#include <QStandardPaths>
#include <QDebug>
#include "migration.h"
#include "ui/dbdialog.h"
#include "ui/mainwindow.h"
#include "rig.h"

static void loadStylesheet(QApplication* app) {
    QFile style(":/res/stylesheet.css");
    style.open(QFile::ReadOnly | QIODevice::Text);
    app->setStyleSheet(style.readAll());
    style.close();
}

static void setupTranslator(QApplication* app) {
    QTranslator* qtTranslator = new QTranslator(app);
    qtTranslator->load("qt_" + QLocale::system().name(),
    QLibraryInfo::location(QLibraryInfo::TranslationsPath));
    app->installTranslator(qtTranslator);

    QTranslator* translator = new QTranslator(app);
    translator->load(":/i18n/openshack_" + QLocale::system().name().left(2));
    app->installTranslator(translator);
}

static void createDataDirectory() {
    QDir dataDir(QStandardPaths::writableLocation(QStandardPaths::DataLocation));

    if (!dataDir.exists()) {
        dataDir.mkpath(dataDir.path());
    }
}

static bool openDatabase() {
    QSettings settings;
    QSqlDatabase db = QSqlDatabase::addDatabase("QMYSQL");
    db.setHostName(settings.value("db/hostname").toString());
    db.setPort(settings.value("db/port").toInt());
    db.setDatabaseName(settings.value("db/dbname").toString());
    db.setUserName(settings.value("db/username").toString());
    db.setPassword(settings.value("db/password").toString());

    if (!db.open()) {
        qCritical() << db.lastError();
        return false;
    }
    else {
        return true;
    }
}

static bool migrateDatabase() {
    Migration m;
    return m.run();
}

static void startRigThread() {
    QThread* rigThread = new QThread;
    Rig* rig = Rig::instance();
    rig->moveToThread(rigThread);
    QObject::connect(rigThread, SIGNAL(started()), rig, SLOT(start()));
    rigThread->start();
}

int main(int argc, char* argv[]) {
    QApplication app(argc, argv);

    app.setApplicationVersion(VERSION);
    app.setOrganizationName("DL2IC");
    app.setApplicationName("OpenShack");

    loadStylesheet(&app);
    setupTranslator(&app);
    createDataDirectory();

    while (!openDatabase()) {
        QMessageBox::critical(NULL, QMessageBox::tr("OpenShack Error"),
                              QMessageBox::tr("Could not connect to database."));

        DbDialog dbDialog;
        if (!dbDialog.exec()) {
            return 0;
        }
    }

    if (!migrateDatabase()) {
        QMessageBox::critical(NULL, QMessageBox::tr("OpenShack Error"),
                              QMessageBox::tr("Database migration failed."));
        return 1;
    }

    startRigThread();

    MainWindow w;
    QIcon icon(":/res/openshack.png");
    w.setWindowIcon(icon);
    w.show();

    return app.exec();
}
