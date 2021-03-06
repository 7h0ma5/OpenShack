#include <QNetworkAccessManager>
#include <QUrl>
#include <QUrlQuery>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QSettings>
#include <QtXml>
#include <QDebug>
#include "hamqth.h"

#define API_URL "http://www.hamqth.com/xml.php"

HamQTH::HamQTH(QObject* parent) :
    QObject(parent)
{
    nam = new QNetworkAccessManager(this);
    connect(nam, SIGNAL(finished(QNetworkReply*)),
            this, SLOT(processReply(QNetworkReply*)));
}

void HamQTH::queryCallsign(QString callsign) {
    if (sessionId.isEmpty()) {
        queuedCallsign = callsign;
        authenticate();
        return;
    }

    queuedCallsign = "";

    QUrlQuery query;
    query.addQueryItem("id", sessionId);
    query.addQueryItem("callsign", callsign);
    query.addQueryItem("prg", "OpenShack");

    QUrl url(API_URL);
    url.setQuery(query);

    nam->get(QNetworkRequest(url));
}

void HamQTH::authenticate() {
    QSettings settings;
    QString username = settings.value("hamqth/username").toString();
    QString password = settings.value("hamqth/password").toString();

    QUrlQuery query;
    query.addQueryItem("u", username);
    query.addQueryItem("p", password);

    QUrl url(API_URL);
    url.setQuery(query);

    nam->get(QNetworkRequest(url));
}

void HamQTH::processReply(QNetworkReply* reply) {
    if (reply->error() != QNetworkReply::NoError) {
        qDebug() << "HamQTH error" << reply->errorString();
        delete reply;
        return;
    }

    QXmlStreamReader xml(reply->readAll());

    QMap<QString, QString> data;

    while (!xml.atEnd() && !xml.hasError()) {
        QXmlStreamReader::TokenType token = xml.readNext();
        if (token != QXmlStreamReader::StartElement) {
            continue;
        }
        if (xml.name() == "session_id") {
            sessionId = xml.readElementText();
        }
        else if (xml.name() == "nick") {
            data["name"] = xml.readElementText();
        }
        else if (xml.name() == "qth") {
            data["qth"] = xml.readElementText();
        }
        else if (xml.name() == "grid") {
            data["grid"] = xml.readElementText();
        }
        else if (xml.name() == "qsl_via") {
            data["qsl_via"] = xml.readElementText();
        }
    }

    delete reply;

    if (data.size()) {
        emit callsignResult(data);
    }

    if (!queuedCallsign.isEmpty()) {
        queryCallsign(queuedCallsign);
    }
}
