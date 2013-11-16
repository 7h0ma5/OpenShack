#include <QtSql>
#include <QDebug>
#include "adif.h"

void Adif::exportToFile(QString filename) {
    QFile file(filename);
    file.open(QIODevice::WriteOnly | QIODevice::Text);
    QTextStream out(&file);

    writeField(out, "PROGRAMMID", "OpenShack");
    writeField(out, "ADIF_VER", "2.2.7");
    out << "<eoh>\n\n";

    QSqlQuery query("SELECT * FROM contacts");
    query.exec();

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
        writeField(out, "frequency", query.value("frequency").toString(), "N");
        writeField(out, "band", query.value("band").toString());
        writeField(out, "mode", query.value("mode").toString());
        writeField(out, "power", query.value("power").toString());
        writeField(out, "rig", query.value("rig").toString());
        writeField(out, "comment", query.value("comment").toString());

        out << "<eor>\n\n";
    }

    file.close();
}

void Adif::writeField(QTextStream& out, QString name, QString value, QString type) {
    if (value.isEmpty()) return;
    out << "<" << name << ":" << value.size();
    if (!type.isEmpty()) out << ":" << type;
    out << ">" << value << '\n';
    // TODO: handle unicode values
}

void Adif::importFromFile(QString filename) {
    // TODO
}
