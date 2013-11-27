#include <QtSql>
#include <QDebug>
#include "adif.h"

int Adif::exportToFile(QString filename) {
    QFile file(filename);
    file.open(QIODevice::WriteOnly | QIODevice::Text);
    QTextStream out(&file);

    out << "# OpenShack ADIF Export " << QDateTime::currentDateTimeUtc().toString(Qt::ISODate) << "\n";
    writeField(out, "ADIF_VER", "2.2.7");
    writeField(out, "PROGRAMID", "OpenShack");
    writeField(out, "PROGRAMVERSION", "1.0");
    out << "<EOH>\n\n";

    QSqlQuery query("SELECT * FROM contacts");
    query.exec();

    int count = 0;
    while (query.next()) {
        QDate date = QDate::fromString(query.value("date").toString(), Qt::ISODate);
        QTime time_on = QTime::fromString(query.value("time_on").toString(), Qt::ISODate);
        QTime time_off = QTime::fromString(query.value("time_off").toString(), Qt::ISODate);

        writeField(out, "call", query.value("call").toString());
        writeField(out, "qso_date", date.toString("yyyyMMdd"), "D");
        writeField(out, "time_on", time_on.toString("hhmmss"), "T");
        writeField(out, "time_off", time_off.toString("hhmmss"), "T");
        writeField(out, "rst_rcvd", query.value("rst_rcvd").toString());
        writeField(out, "rst_sent", query.value("rst_sent").toString());
        writeField(out, "name", query.value("name").toString());
        writeField(out, "qth", query.value("qth").toString());
        writeField(out, "gridsquare", query.value("grid").toString());
        writeField(out, "my_gridsquare", query.value("my_grid").toString());
        writeField(out, "cqz", query.value("cqz").toString());
        writeField(out, "ituz", query.value("ituz").toString());
        writeField(out, "freq", query.value("frequency").toString(), "N");
        writeField(out, "freq_rx", query.value("frequency").toString(), "N");
        writeField(out, "band", query.value("band").toString());
        writeField(out, "band_rx", query.value("band").toString());
        writeField(out, "mode", query.value("mode").toString());
        writeField(out, "tx_pwr", query.value("tx_power").toString());
        writeField(out, "my_rig", query.value("my_rig").toString());
        writeField(out, "comment", query.value("comment").toString());
        writeField(out, "qsl_via", query.value("qsl_via").toString());

        out << "<eor>\n\n";
        count++;
    }

    file.close();
    return count;
}

void Adif::writeField(QTextStream& out, QString name, QString value, QString type) {
    if (value.isEmpty()) return;
    out << "<" << name << ":" << value.size();
    if (!type.isEmpty()) out << ":" << type;
    out << ">" << value << '\n';
    // TODO: handle unicode values
}

void Adif::readField(QTextStream& in, QString& field, QString& value) {
    char c;

    field = "";
    value = "";

    // find beginning of next field
    while (!in.atEnd()) {
        in >> c;
        if (c == '<') break;
    }
    if (in.atEnd() || c != '<') return;

    // read field name
    while (!in.atEnd()) {
        in >> c;
        if (c == ':' || c == '>') break;
        field.append(c);
    }
    field = field.toLower();
    if (c == '>' || in.atEnd()) return;

    // read field length
    QString lengthString;
    int length = 0;

    while (!in.atEnd()) {
        in >> c;
        if (c == ':' || c == '>') break;
        lengthString.append(c);
    }
    if (!lengthString.isEmpty()) {
        length = lengthString.toInt();
    }
    if (in.atEnd()) {
        return;
    }

     // read field type
    QString type = "";
    if (c == ':') {
        while (!in.atEnd()) {
            in >> c;
            if (c == '>') break;
            type.append(c);
        }
    }

    // read field value
    if (c == '>' && length) {
        value = QString(in.read(length));
    }
}

int Adif::importFromFile(QString filename) {
    QFile file(filename);
    file.open(QIODevice::ReadOnly | QIODevice::Text);

    QTextStream in(&file);
    int count = import(in);

    file.close();

    return count;
}

int Adif::import(QTextStream& in) {
    QString field;
    QString value;

    int count = 0;
    while (!in.atEnd()) {
        QMap<QString, QString> contact;
        while (!in.atEnd()) {
            readField(in, field, value);
            if (field == "eor") break;
            if (!value.isEmpty()) {
                contact[field] = value;
            }
        }
        if (field == "eor") {
            insertContact(contact);
            count++;
        }
    }

    return count;
}

void Adif::insertContact(QMap<QString, QString>& data) {
    QSqlQuery query;
    query.prepare("INSERT INTO contacts (call, rst_sent, rst_rcvd, name, qth, grid, my_grid, date,"
                  "time_on, time_off, frequency, band, mode, cqz, ituz, tx_power, my_rig, comment, qsl_via) "
                  "VALUES (:call, :rst_sent, :rst_rcvd, :name, :qth, :grid, :my_grid, :date,"
                  ":time_on, :time_off, :frequency, :band, :mode, :cqz, :ituz, :tx_power, :my_rig, :comment, :qsl_via)");

    query.bindValue(":call", data.value("call", "NOCALL"));
    query.bindValue(":rst_rcvd", data.value("rst_rcvd"));
    query.bindValue(":rst_sent", data.value("rst_sent"));
    query.bindValue(":name", data.value("name"));
    query.bindValue(":qth", data.value("qth"));
    query.bindValue(":grid", data.value("gridsquare"));
    query.bindValue(":my_grid", data.value("my_gridsquare"));
    query.bindValue(":date", QDate::fromString(data.value("qso_date"), "yyyyMMdd").toString(Qt::ISODate));
    query.bindValue(":frequency", data.value("freq"));
    query.bindValue(":band", data.value("band"));
    query.bindValue(":mode", data.value("mode"));
    query.bindValue(":cqz", data.value("cqz"));
    query.bindValue(":ituz", data.value("ituz"));
    query.bindValue(":tx_power", data.value("tx_pwr"));
    query.bindValue(":my_rig", data.value("my_rig"));
    query.bindValue(":comment", data.value("comment"));
    query.bindValue(":qsl_via", data.value("qsl_via"));

    QTime time_on = parseTime(data.value("time_on"));
    QTime time_off = parseTime(data.value("time_off"));

    if (time_on.isValid() && time_off.isNull()) {
        time_off = time_on;
    }
    if (time_off.isValid() && time_on.isNull()) {
        time_on = time_off;
    }

    query.bindValue(":time_on", time_on.toString(Qt::ISODate));
    query.bindValue(":time_off", time_off.toString(Qt::ISODate));

    query.exec();
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
