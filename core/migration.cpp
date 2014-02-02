#include <QProgressDialog>
#include <QtSql>
#include <QDebug>
#include "core/migration.h"

/**
 * Migrate the database to the latest schema version.
 * Returns true on success.
 */
bool Migration::run() {
    int currentVersion = getVersion();

    if (currentVersion == latestVersion) {
        qDebug() << "database already up to date";
        return true;
    }
    else {
        qCritical() << "database from the future";
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

/**
 * Returns the current user_version of the database.
 */
int Migration::getVersion() {
    QSqlQuery query("SELECT version FROM schema_versions "
                    "ORDER BY version DESC LIMIT 1");

    if (query.first()) {
        return query.value(0).toInt();
    }
    else {
        return 0;
    }
}

/**
 * Changes the user_version of the database to version.
 * Returns true of the operation was successful.
 */
bool Migration::setVersion(int version) {
    QSqlQuery query;
    query.prepare("INSERT INTO schema_versions (version, updated)"
                  "VALUES (:version, UTC_TIMESTAMP())");

    query.bindValue(":version", version);

    if (!query.exec()) {
        qWarning() << "setting schema version failed" << query.lastError();
        return false;
    }
    else {
        return true;
    }
}

/**
 * Migrate the database to the given version.
 * Returns true if the operation was successful.
 */
bool Migration::migrate(int toVersion) {
    qDebug() << "migrate to" << toVersion;

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
    bool result = true;

    QSqlQuery query;

    result &= query.exec("CREATE TABLE IF NOT EXISTS schema_versions"
                         "(version INT PRIMARY KEY,"
                         "updated DATETIME NOT NULL)");

    result &= query.exec("CREATE TABLE IF NOT EXISTS contacts"
                         "(id INT AUTO_INCREMENT PRIMARY KEY,"
                         "date DATE NOT NULL, time_on TIME NOT NULL, time_off TIME,"
                         "callsign VARCHAR(25) NOT NULL,"
                         "rst_sent VARCHAR(25), rst_rcvd VARCHAR(25),"
                         "name VARCHAR(50), qth VARCHAR(75), grid VARCHAR(12), my_grid VARCHAR(12),"
                         "cqz INTEGER, ituz INTEGER,"
                         "frequency DECIMAL(12,6) UNSIGNED, band VARCHAR(10), mode VARCHAR(10),"
                         "my_rig VARCHAR(25), tx_power DECIMAL(8,2) UNSIGNED,"
                         "comment TEXT, qsl_via VARCHAR(25))");

    return result;
}
