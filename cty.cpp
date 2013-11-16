#include <QNetworkAccessManager>
#include <QUrl>
#include <QUrlQuery>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QStandardPaths>
#include <QDebug>
#include <QStringRef>
#include <QDir>
#include <QFile>
#include "cty.h"

#define CTY_URL "http://www.country-files.com/cty/cty.dat"

Cty::Cty() {
    QDir dir(QStandardPaths::writableLocation(QStandardPaths::DataLocation));

    if (dir.exists("cty.dat")) {
        qDebug() << "use cached cty.dat at" << dir.path();
        QFile file(dir.filePath("cty.dat"));
        file.open(QIODevice::ReadOnly);
        parseData(file.readAll());
        file.close();
    }
    else {
        download();
    }
}

void Cty::download() {
    nam = new QNetworkAccessManager(this);
    connect(nam, SIGNAL(finished(QNetworkReply*)),
            this, SLOT(processReply(QNetworkReply*)));

    QUrl url(CTY_URL);
    nam->get(QNetworkRequest(url));
    qDebug() << "download cty.dat from" << url.toString();
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
    QByteArray data = reply->readAll();
    parseData(data);

    QDir dir(QStandardPaths::writableLocation(QStandardPaths::DataLocation));

    QFile file(dir.filePath("cty.dat"));
    file.open(QIODevice::WriteOnly);
    file.write(data);
    file.close();

    delete reply;
    delete nam;
}

void Cty::parseData(QByteArray data) {
    QList<QByteArray> contries = data.split(';');
    foreach (QByteArray country, contries) {
        QList<QByteArray> columns = country.split(':');
        if (columns.count() != 9) continue;;

        Dxcc* dxcc = new Dxcc;
        dxcc->name = QString(columns.at(0).trimmed());
        dxcc->cqZone = columns.at(1).trimmed().toInt();
        dxcc->ituZone = columns.at(2).trimmed().toInt();
        strncpy(dxcc->continent, columns.at(3).trimmed().data(), 2);
        dxcc->lat = columns.at(4).trimmed().toFloat();
        dxcc->lon = -columns.at(5).trimmed().toFloat();
        dxcc->utcOffset = columns.at(6).trimmed().toFloat();

        dxccList.append(dxcc);

        dxccMap[QString(columns.at(7).trimmed())] = dxcc;

        QList<QByteArray> prefixes = columns.at(8).split(',');
        foreach (QByteArray prefix, prefixes) {
            dxccMap[QString(prefix.trimmed())] = dxcc;
        }
    }
}

Cty::~Cty() {
    foreach (Dxcc* dxcc, dxccList) {
        delete dxcc;
    }
}
