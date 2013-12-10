#include <QTcpSocket>
#include <QtXml>
#include <QtDebug>
#include "fldigi.h"
#include "logformat/adif.h"

Fldigi::Fldigi(QObject *parent) :
    QTcpServer(parent)
{
    listen(QHostAddress::Any, 8421);
}

void Fldigi::incomingConnection(int socket) {
    QTcpSocket* sock = new QTcpSocket(this);
    connect(sock, SIGNAL(readyRead()), this, SLOT(readClient()));
    connect(sock, SIGNAL(disconnected()), this, SLOT(discardClient()));
    sock->setSocketDescriptor(socket);
}

void Fldigi::readClient() {
    QTcpSocket* sock = (QTcpSocket*)sender();

    QString data = QString(sock->readAll());
    int split = data.indexOf("\r\n\r\n", 0);
    data.remove(0, split+4);

    QXmlStreamReader xml(data);
    processMethodCall(sock, xml);

    sock->close();
    if (sock->state() == QTcpSocket::UnconnectedState) {
        delete sock;
    }
}

void Fldigi::discardClient() {
    QTcpSocket* sock = (QTcpSocket*)sender();
    sock->deleteLater();
}

void Fldigi::processMethodCall(QTcpSocket* sock, QXmlStreamReader& xml) {
    QByteArray response;

    while (!xml.atEnd() && !xml.hasError()) {
        xml.readNextStartElement();

        if (xml.name() == "methodCall") {
            qDebug() << "method call";
        }
        if (xml.name() == "methodName") {
            QString method = xml.readElementText();

            if (method == "log.add_record") {
                QString param = parseParam(xml);
                if (!param.isEmpty()) {
                    response = addRecord(param);
                }
            }
            else if (method == "system.listMethods") {
                response = listMethods();
            }
        }
    }

    QTextStream out(sock);

    if (!response.isEmpty()) {
        out << "HTTP/1.1 200 OK\r\n";
        out << "Content-Type: text/xml; charset=utf-8\r\n";
        out << "Content-Length: " << response.length() << "\r\n";
        out << "\r\n";
        out << response;
    }
    else {
        out << "HTTP/1.1 400 Internal Error\r\n";
    }
}

QString Fldigi::parseParam(QXmlStreamReader& xml) {
    while (!xml.atEnd() && !xml.hasError()) {
        xml.readNextStartElement();
        if (xml.name() == "value") {
            return xml.readElementText();
        }
    }

    return QString();
}

QByteArray Fldigi::listMethods() {
    QByteArray out;

    QXmlStreamWriter xml(&out);
    xml.writeStartDocument();
    xml.writeStartElement("methodResponse");
    xml.writeStartElement("params");
    xml.writeStartElement("param");
    xml.writeStartElement("value");
    xml.writeStartElement("array");
    xml.writeStartElement("data");

    xml.writeStartElement("value");
    xml.writeCharacters("log.add_record");
    xml.writeEndElement();

    xml.writeStartElement("value");
    xml.writeCharacters("system.listMethods");
    xml.writeEndElement();

    xml.writeEndElement();
    xml.writeEndElement();
    xml.writeEndElement();
    xml.writeEndElement();
    xml.writeEndElement();
    xml.writeEndElement();
    xml.writeEndDocument();

    return out;
}

QByteArray Fldigi::addRecord(QString data) {
    QByteArray out;

    QXmlStreamWriter xml(&out);
    xml.writeStartDocument();
    xml.writeStartElement("methodResponse");
    xml.writeStartElement("params");
    xml.writeStartElement("param");
    xml.writeStartElement("value");
    xml.writeEndElement();
    xml.writeEndElement();
    xml.writeEndElement();
    xml.writeEndElement();
    xml.writeEndDocument();

    QTextStream in(&data);
    Adif adif(in);
    int count = adif.runImport();

    if (count) {
        emit contactAdded();
    }

    return out;
}
