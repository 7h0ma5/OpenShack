#include <QtSql>
#include "logformat.h"
#include "utils.h"
#include "cty.h"

LogFormat::LogFormat(QTextStream& stream) : stream(stream) {
    this->defaults = 0;
}

LogFormat::~LogFormat() {

}

void LogFormat::setDefaults(QMap<QString, QString>& defaults) {
    this->defaults = &defaults;
}

void LogFormat::setDateRange(QDate start, QDate end) {
    this->startDate = start;
    this->endDate = end;
}

int LogFormat::runImport() {
    this->importStart();

    Cty cty;

    int count = 0;

    QSqlTableModel model;
    model.setTable("contacts");
    QSqlRecord record = model.record();

    while (true) {
        record.clearValues();

        if (!this->importNext(record)) break;

        if (!endDate.isNull()) {
            if (!inDateRange(record.value("date").toDate())) {
                continue;
            }
        }

        if (defaults) {
            foreach (QString key, defaults->keys()) {
                if (record.value(key).isNull()) {
                    record.setValue(key, defaults->value(key));
                }
            }
        }

        Dxcc* dxcc = cty.lookup(record.value("callsign").toString());

        if (record.value("ituz").isNull() && dxcc) {
            record.setValue("ituz", QString::number(dxcc->ituZone));
        }
        if (record.value("cqz").isNull() && dxcc) {
            record.setValue("cqz", QString::number(dxcc->cqZone));
        }
        if (record.value("band").isNull() && !record.value("frequency").isNull()) {
            double freq = record.value("frequency").toDouble();
            record.setValue("band", freqToBand(freq));
        }

        record.setValue("id", QVariant());
        model.insertRecord(-1, record);
        model.submitAll();

        count++;
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
        QSqlRecord record = query.record();
        this->exportContact(record);
        count++;
    }

    this->exportEnd();
    return count;
}

bool LogFormat::inDateRange(QString date) {
    return inDateRange(QDate::fromString(date, Qt::ISODate));
}

bool LogFormat::inDateRange(QDate date) {
    return date >= startDate && date <= endDate;
}

