#include "logformat.h"

LogFormat::LogFormat(QTextStream& stream) : stream(stream) {

}

LogFormat::~LogFormat() {

}

void LogFormat::setDefaults(QMap<QString, QString>& defaults) {
    this->defaults = &defaults;
}

int LogFormat::runImport() {
    this->importStart();

    int count = 0;
    QMap<QString, QString> contact;

    while (true) {
        if (defaults) {
            contact.unite(*defaults);
        }

        if (!this->importNext(contact)) break;

        QSqlQuery query;
        query.prepare("INSERT INTO contacts (call, rst_sent, rst_rcvd, name, qth, grid, my_grid, date,"
                  "time_on, time_off, frequency, band, mode, cqz, ituz, tx_power, my_rig, comment, qsl_via) "
                  "VALUES (:call, :rst_sent, :rst_rcvd, :name, :qth, :grid, :my_grid, :date,"
                  ":time_on, :time_off, :frequency, :band, :mode, :cqz, :ituz, :tx_power, :my_rig, :comment, :qsl_via)");

        query.bindValue(":call",      contact.value("call", "NOCALL"));
        query.bindValue(":rst_rcvd",  contact.value("rst_rcvd"));
        query.bindValue(":rst_sent",  contact.value("rst_sent"));
        query.bindValue(":name",      contact.value("name"));
        query.bindValue(":qth",       contact.value("qth"));
        query.bindValue(":grid",      contact.value("grid"));
        query.bindValue(":my_grid",   contact.value("my_grid"));
        query.bindValue(":date",      contact.value("date"));
        query.bindValue(":frequency", contact.value("freq"));
        query.bindValue(":band",      contact.value("band"));
        query.bindValue(":mode",      contact.value("mode"));
        query.bindValue(":cqz",       contact.value("cqz"));
        query.bindValue(":ituz",      contact.value("ituz"));
        query.bindValue(":tx_power",  contact.value("tx_power"));
        query.bindValue(":my_rig",    contact.value("my_rig"));
        query.bindValue(":comment",   contact.value("comment"));
        query.bindValue(":qsl_via",   contact.value("qsl_via"));
        query.bindValue(":time_on",   contact.value("time_on"));
        query.bindValue(":time_off",  contact.value("time_off"));

        query.exec();
        count++;

        contact.clear();
    }

    this->importEnd();
    return count;
}

int LogFormat::runExport() {
    this->exportStart();

    QSqlQuery query("SELECT * FROM contacts");
    query.exec();

    int count = 0;
    while (query.next()) {
        this->exportContact(query);
        count++;
    }

    this->exportEnd();
    return count;
}
