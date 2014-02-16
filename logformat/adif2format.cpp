#include <QSqlRecord>
#include <QDebug>
#include "adif2format.h"

void Adif2Format::exportStart() {
    stream << "# OpenShack ADIF Export " << QDateTime::currentDateTimeUtc().toString(Qt::ISODate) << "\n";
    writeField("ADIF_VER", "2.2.7");
    writeField("PROGRAMID", "OpenShack");
    writeField("PROGRAMVERSION", VERSION);
    stream << "<EOH>\n\n";
}

void Adif2Format::exportContact(QSqlRecord& record) {
    QDate date = QDate::fromString(record.value("date").toString(), Qt::ISODate);
    QTime time_on = QTime::fromString(record.value("time_on").toString(), Qt::ISODate);
    QTime time_off = QTime::fromString(record.value("time_off").toString(), Qt::ISODate);

    writeField("call", record.value("callsign").toString());
    writeField("qso_date", date.toString("yyyyMMdd"), "D");
    writeField("time_on", time_on.toString("hhmmss"), "T");
    writeField("time_off", time_off.toString("hhmmss"), "T");
    writeField("rst_rcvd", record.value("rst_rcvd").toString());
    writeField("rst_sent", record.value("rst_sent").toString());
    writeField("name", record.value("name").toString());
    writeField("qth", record.value("qth").toString());
    writeField("gridsquare", record.value("grid").toString());
    writeField("my_gridsquare", record.value("my_grid").toString());
    writeField("cqz", record.value("cqz").toString());
    writeField("ituz", record.value("ituz").toString());
    writeField("freq", record.value("frequency").toString(), "N");
    writeField("band", record.value("band").toString());
    writeField("mode", record.value("mode").toString());
    writeField("tx_pwr", record.value("tx_power").toString());
    writeField("my_rig", record.value("my_rig").toString());
    writeField("comment", record.value("comment").toString());
    writeField("qsl_via", record.value("qsl_via").toString());

    stream << "<eor>\n\n";
}

void Adif2Format::writeField(QString name, QString value, QString type) {
    if (value.isEmpty()) return;
    stream << "<" << name << ":" << value.size();
    if (!type.isEmpty()) stream << ":" << type;
    stream << ">" << value << '\n';
    // TODO: handle unicode values
}

void Adif2Format::readField(QString& field, QString& value) {
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

bool Adif2Format::readContact(QMap<QString, QString>& contact) {
    QString field;
    QString value;

    while (!stream.atEnd()) {
        readField(field, value);

        if (field == "eor") {
            return true;
        }

        if (!value.isEmpty()) {
             contact[field] = value;
        }
    }

    return false;
}

bool Adif2Format::importNext(QSqlRecord& record) {
    QMap<QString, QString> contact;

    if (!readContact(contact)) {
        return false;
    }

    record.setValue("callsign", contact.value("call", "NOCALL"));
    record.setValue("rst_rcvd", contact.value("rst_rcvd"));
    record.setValue("rst_sent", contact.value("rst_sent"));
    record.setValue("name", contact.value("name"));
    record.setValue("qth", contact.value("qth"));
    record.setValue("grid", contact.value("gridsquare"));
    record.setValue("my_grid", contact.value("my_gridsquare"));
    record.setValue("cqz", contact.value("cqz"));
    record.setValue("ituz", contact.value("ituz"));
    record.setValue("frequency", contact.value("freq"));
    record.setValue("band", contact.value("band"));
    record.setValue("mode", contact.value("mode"));
    record.setValue("tx_power", contact.value("tx_pwr"));
    record.setValue("my_rig", contact.value("my_rig"));
    record.setValue("comment", contact.value("comment"));
    record.setValue("qsl_via", contact.value("qsl_via"));

    QDate date = QDate::fromString(contact.value("qso_date"), "yyyyMMdd");
    record.setValue("date", date);

    QTime time_on = parseTime(contact.value("time_on"));
    QTime time_off = parseTime(contact.value("time_off"));

    if (time_on.isValid() && time_off.isNull()) {
        time_off = time_on;
    }
    if (time_off.isValid() && time_on.isNull()) {
        time_on = time_off;
    }

    record.setValue("time_on", time_on);
    record.setValue("time_off", time_off);

    return true;
}

QTime Adif2Format::parseTime(QString time) {
    switch (time.length()) {
    case 4:
        return QTime::fromString(time, "hhmm");

    case 6:
        return QTime::fromString(time, "hhmmss");

    default:
        return QTime();
    }
}
