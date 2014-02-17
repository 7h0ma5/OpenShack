#include <QtSql>
#include "logformat.h"
#include "adiformat.h"
#include "adxformat.h"
#include "jsonformat.h"
#include "core/utils.h"
#include "core/cty.h"

LogFormat::LogFormat(QTextStream& stream) : stream(stream) {
    this->defaults = 0;
}

LogFormat::~LogFormat() {

}

LogFormat* LogFormat::open(QString type, QTextStream& stream) {
    type = type.toLower();

    if (type == "adi") {
        return open(LogFormat::ADI, stream);
    }
    else if (type == "adx") {
        return open(LogFormat::ADX, stream);
    }
    else if (type == "json") {
        return open(LogFormat::JSON, stream);
    }
    else if (type == "cabrillo") {
        return open(LogFormat::JSON, stream);
    }
    else {
        return NULL;
    }
}

LogFormat* LogFormat::open(LogFormat::Type type, QTextStream& stream) {
    switch (type) {
    case LogFormat::ADI:
        return new AdiFormat(stream);

    case LogFormat::ADX:
        return new AdxFormat(stream);

    case LogFormat::JSON:
        return new JsonFormat(stream);

    case LogFormat::CABRILLO:
        return NULL;

    default:
        return NULL;
    }
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

        if (dateRangeSet()) {
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

    QSqlQuery query;
    if (dateRangeSet()) {
        query.prepare("SELECT * FROM contacts"
                      " WHERE (date BETWEEN :start_date AND :end_date)"
                      " ORDER BY date ASC, time_on ASC");
        query.bindValue(":start_date", startDate);
        query.bindValue(":end_date", endDate);
    }
    else {
        query.prepare("SELECT * FROM contacts ORDER BY date ASC, time_on ASC");
    }

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

bool LogFormat::dateRangeSet() {
    return !startDate.isNull() && !endDate.isNull();
}

bool LogFormat::inDateRange(QString date) {
    return inDateRange(QDate::fromString(date, Qt::ISODate));
}

bool LogFormat::inDateRange(QDate date) {
    return date >= startDate && date <= endDate;
}

