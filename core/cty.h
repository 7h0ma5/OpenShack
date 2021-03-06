#ifndef CTY_H
#define CTY_H

#include <QObject>
#include <QMap>

class QNetworkAccessManager;
class QNetworkReply;

struct Dxcc {
    QString name;
    int cqZone;
    int ituZone;
    char continent[3];
    float lat;
    float lon;
    float utcOffset;
};

class Cty : public QObject {
    Q_OBJECT

public:
    Cty();
    ~Cty();

    Dxcc* lookup(QString callsign);

public slots:
    void processReply(QNetworkReply* reply);

private:
    void download();
    void parseData(QByteArray data);

    QNetworkAccessManager* nam;
    QMap<QString, Dxcc*> dxccMap;
    QList<Dxcc*> dxccList;
};

#endif // CTY_H
