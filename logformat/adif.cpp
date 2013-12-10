#include <QtSql>
#include <QDebug>
#include "adif.h"

void Adif::exportStart() {
    stream << "# OpenShack ADIF Export " << QDateTime::currentDateTimeUtc().toString(Qt::ISODate) << "\n";
    writeField("ADIF_VER", "2.2.7");
    writeField("PROGRAMID", "OpenShack");
    writeField("PROGRAMVERSION", "1.0");
    stream << "<EOH>\n\n";
}

void Adif::exportContact(QSqlQuery& query) {
    QDate date = QDate::fromString(query.value("date").toString(), Qt::ISODate);
    QTime time_on = QTime::fromString(query.value("time_on").toString(), Qt::ISODate);
    QTime time_off = QTime::fromString(query.value("time_off").toString(), Qt::ISODate);

    writeField("call", query.value("call").toString());
    writeField("qso_date", date.toString("yyyyMMdd"), "D");
    writeField("time_on", time_on.toString("hhmmss"), "T");
    writeField("time_off", time_off.toString("hhmmss"), "T");
    writeField("rst_rcvd", query.value("rst_rcvd").toString());
    writeField("rst_sent", query.value("rst_sent").toString());
    writeField("name", query.value("name").toString());
    writeField("qth", query.value("qth").toString());
    writeField("gridsquare", query.value("grid").toString());
    writeField("my_gridsquare", query.value("my_grid").toString());
    writeField("cqz", query.value("cqz").toString());
    writeField("ituz", query.value("ituz").toString());
    writeField("freq", query.value("frequency").toString(), "N");
    writeField("freq_rx", query.value("frequency").toString(), "N");
    writeField("band", query.value("band").toString());
    writeField("band_rx", query.value("band").toString());
    writeField("mode", query.value("mode").toString());
    writeField("tx_pwr", query.value("tx_power").toString());
    writeField("my_rig", query.value("my_rig").toString());
    writeField("comment", query.value("comment").toString());
    writeField("qsl_via", query.value("qsl_via").toString());

    stream << "<eor>\n\n";
}

void Adif::writeField(QString name, QString value, QString type) {
    if (value.isEmpty()) return;
    stream << "<" << name << ":" << value.size();
    if (!type.isEmpty()) stream << ":" << type;
    stream << ">" << value << '\n';
    // TODO: handle unicode values
}

void Adif::readField(QString& field, QString& value) {
    char c;

    field = "";
    value = "";

    // find beginning of next field
    while (!stream.atEnd()) {
        stream >> c;
        if (c == '<') break;
    }
    if (stream.atEnd() || c != '<') return;

    // read field name
    while (!stream.atEnd()) {
        stream >> c;
        if (c == ':' || c == '>') break;
        field.append(c);
    }
    field = field.toLower();
    if (c == '>' || stream.atEnd()) return;

    // read field length
    QString lengthString;
    int length = 0;

    while (!stream.atEnd()) {
        stream >> c;
        if (c == ':' || c == '>') break;
        lengthString.append(c);
    }
    if (!lengthString.isEmpty()) {
        length = lengthString.toInt();
    }
    if (stream.atEnd()) {
        return;
    }

     // read field type
    QString type = "";
    if (c == ':') {
        while (!stream.atEnd()) {
            stream >> c;
            if (c == '>') break;
            type.append(c);
        }
    }

    // read field value
    if (c == '>' && length) {
        value = QString(stream.read(length));
    }
}

bool Adif::importNext(QMap<QString, QString>& contact) {
    QString field;
    QString value;

    bool success = false;

    while (!stream.atEnd()) {
        readField(field, value);

        if (field == "eor") {
            success = true;
            break;
        }

        if (!value.isEmpty()) {
             contact[field] = value;
        }
    }

    if (!success) return false;

    contact["grid"] = contact.value("gridsquare");
    contact["my_grid"] = contact.value("my_gridsquare");
    contact["tx_power"] = contact.value("tx_pwr");
    contact["date"] = QDate::fromString(contact.value("qso_date"), "yyyyMMdd").toString(Qt::ISODate);

    QTime time_on = parseTime(contact.value("time_on"));
    QTime time_off = parseTime(contact.value("time_off"));

    if (time_on.isValid() && time_off.isNull()) {
        time_off = time_on;
    }
    if (time_off.isValid() && time_on.isNull()) {
        time_on = time_off;
    }

    contact["time_on"] = time_on.toString(Qt::ISODate);
    contact["time_off"] = time_off.toString(Qt::ISODate);

    return true;
}

QTime Adif::parseTime(QString time) {
    switch (time.length()) {
    case 4:
        return QTime::fromString(time, "hhmm");

    case 6:
        return QTime::fromString(time, "hhmmss");

    default:
        return QTime();
    }
}
