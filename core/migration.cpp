#include <QProgressDialog>
#include <QtSql>
#include <QDebug>
#include "core/migration.h"

/**
 * @brief
 */
bool Migration::run() {
    int currentVersion = getVersion();

    if (currentVersion >= latestVersion) {
        qDebug() << "database already up to date";
        return true;
    }

    while (true) {
        currentVersion = getVersion();

        if (currentVersion < latestVersion) {
            bool res = migrate(currentVersion+1);
            if (!res || getVersion() == currentVersion) {
                qCritical() << "migration failed";
                return false;
            }
        }
        else break;
    }

    qDebug() << "database migration successful";

    return true;
}

int Migration::getVersion() {
    QSqlQuery query("PRAGMA user_version");

    if (query.first()) {
        return query.value(0).toInt();
    }
    else {
        qWarning() << "pragma user_version failed" << query.lastError();
        return 0;
    }
}

bool Migration::setVersion(int version) {
    QString queryString = QString("PRAGMA user_version = %1").arg(version);
    QSqlQuery query(queryString);

    if (!query.exec()) {
        qWarning() << "pragma set user_version failed" << query.lastError();
        return false;
    }
    else {
        return true;
    }
}

bool Migration::migrate(int toVersion) {
    qDebug() << "migrate to" << toVersion;
    QThread::sleep(5);

    QSqlDatabase db = QSqlDatabase::database();
    if (!db.transaction()) {
        qCritical() << "transaction failed";
        return false;
    }

    bool result = false;
    switch (toVersion) {
    case 1: result = migrate1(); break;
    }

    if (result && setVersion(toVersion) && db.commit()) {
        return true;
    }
    else {
        if (!db.rollback()) {
            qCritical() << "rollback failed";
        }
        return false;
    }
}

bool Migration::migrate1() {
    QSqlQuery query;
    return query.exec("CREATE TABLE IF NOT EXISTS contacts"
                      "(id INTEGER PRIMARY KEY,"
                      "date TEXT, time_on TEXT, time_off TEXT,"
                      "call TEXT NOT NULL,"
                      "rst_sent TEXT, rst_rcvd TEXT,"
                      "name TEXT, qth TEXT, grid TEXT, my_grid TEXT,"
                      "cqz INTEGER, ituz INTEGER,"
                      "frequency TEXT, band TEXT, mode TEXT,"
                      "my_rig TEXT, tx_power TEXT,"
                      "comment TEXT, qsl_via TEXT)");
}
