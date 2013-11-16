#include <QNetworkAccessManager>
#include <QUrl>
#include <QUrlQuery>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QDebug>
#include <QStringRef>
#include "cty.h"

#define CTY_URL "http://www.country-files.com/cty/cty.dat"

void Cty::import() {
    nam = new QNetworkAccessManager(this);
    connect(nam, SIGNAL(finished(QNetworkReply*)),
            this, SLOT(processReply(QNetworkReply*)));

    QUrl url(CTY_URL);
    nam->get(QNetworkRequest(url));
}

Dxcc* Cty::lookup(QString callsign) {
    for (int i = callsign.length(); i > 0; i--) {
        QStringRef substring(&callsign, 0, i);
        if (dxccMap.contains(substring.toString())) {
            return dxccMap[substring.toString()];
        }
    }
    return NULL;
}

void Cty::processReply(QNetworkReply* reply) {
    QByteArray bytes = reply->readAll();
    QList<QByteArray> countries = bytes.split(';');

    foreach (QByteArray countryRow, countries) {
        QList<QByteArray> columns = countryRow.split(':');
        if (columns.count() != 9) { qDebug() << "error"; continue; };

        Dxcc* dxcc = new Dxcc;
        dxcc->name = QString(columns.at(0).trimmed());
        dxcc->cqZone = columns.at(1).trimmed().toInt();
        dxcc->ituZone = columns.at(2).trimmed().toInt();
        strncpy(dxcc->continent, columns.at(3).trimmed().data(), 2);
        dxcc->lat = columns.at(4).trimmed().toFloat();
        dxcc->lon = columns.at(5).trimmed().toFloat();
        dxcc->utcOffset = columns.at(6).trimmed().toFloat();

        dxccList.append(dxcc);

        dxccMap[QString(columns.at(7).trimmed())] = dxcc;

        QList<QByteArray> prefixes = columns.at(8).split(',');
        foreach (QByteArray prefix, prefixes) {
            dxccMap[QString(prefix.trimmed())] = dxcc;
        }
    }

    delete reply;
    delete nam;
}

Cty::~Cty() {
    foreach (Dxcc* dxcc, dxccList) {
        delete dxcc;
    }
}
