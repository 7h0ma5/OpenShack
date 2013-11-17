#include <QtSql>
#include <QDebug>
#include "adif.h"

int Adif::exportToFile(QString filename) {
    QFile file(filename);
    file.open(QIODevice::WriteOnly | QIODevice::Text);
    QTextStream out(&file);

    writeField(out, "PROGRAMMID", "OpenShack");
    writeField(out, "ADIF_VER", "2.2.7");
    out << "<eoh>\n\n";

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
        writeField(out, "rst_rcvd", query.value("rst_rx").toString());
        writeField(out, "rst_sent", query.value("rst_tx").toString());
        writeField(out, "name", query.value("name").toString());
        writeField(out, "qth", query.value("qth").toString());
        writeField(out, "gridsquare", query.value("grid").toString());
        writeField(out, "cqz", query.value("cqz").toString());
        writeField(out, "ituz", query.value("ituz").toString());
        writeField(out, "freq", query.value("frequency").toString(), "N");
        writeField(out, "freq_rx", query.value("frequency").toString(), "N");
        writeField(out, "band", query.value("band").toString());
        writeField(out, "band_rx", query.value("band").toString());
        writeField(out, "mode", query.value("mode").toString());
        writeField(out, "power", query.value("power").toString());
        writeField(out, "rig", query.value("rig").toString());
        writeField(out, "comment", query.value("comment").toString());

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

    QString field;
    QString value;

    while (!in.atEnd() && field != "eoh") {
        readField(in, field, value);
    }

    while (!in.atEnd()) {
        QMap<QString, QString> contact;
        while (!in.atEnd()) {
            readField(in, field, value);
            if (field == "eor") break;
            if (!value.isEmpty()) {
                contact[field.toLower()] = value;
            }
        }
        if (field == "eor") {
            insertContact(contact);
        }
    }

    file.close();

    return 0;
}

void Adif::insertContact(QMap<QString, QString>& data) {
    QSqlQuery query;
    query.prepare("INSERT INTO contacts (call, rst_rx, rst_tx, name, qth, grid, date,"
                  "time_on, time_off, frequency, band, mode, cqz, ituz, power, rig, comment) "
                  "VALUES (:call, :rst_rx, :rst_tx, :name, :qth, :grid, :date,"
                  ":time_on, :time_off, :frequency, :band, :mode, :cqz, :ituz, :power, :rig, :comment)");

    query.bindValue(":call", data.value("call", "NOCALL"));
    query.bindValue(":rst_rx", data.value("rst_rcvd"));
    query.bindValue(":rst_tx", data.value("rst_sent"));
    query.bindValue(":name", data.value("name"));
    query.bindValue(":qth", data.value("qth"));
    query.bindValue(":grid", data.value("gridsquare"));
    query.bindValue(":date", QDate::fromString(data.value("qso_date"), "yyyyMMdd").toString(Qt::ISODate));
    query.bindValue(":time_on", QTime::fromString(data.value("time_on"), "hhmmss").toString(Qt::ISODate));
    query.bindValue(":time_off", QTime::fromString(data.value("time_off"), "hhmmss").toString(Qt::ISODate));
    query.bindValue(":frequency", data.value("freq"));
    query.bindValue(":band", data.value("band"));
    query.bindValue(":mode", data.value("mode"));
    query.bindValue(":cqz", data.value("cqz"));
    query.bindValue(":ituz", data.value("ituz"));
    query.bindValue(":power", data.value("power"));
    query.bindValue(":rig", data.value("rig"));
    query.bindValue(":comment", data.value("comment"));
    query.exec();

    qDebug() << query.lastError();
}
