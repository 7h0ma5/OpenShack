#ifndef ADIF_H
#define ADIF_H

#include "logformat.h"

class Adif : public LogFormat {
public:
    Adif(QTextStream& stream) : LogFormat(stream) {}

private:
    bool importNext(QMap<QString, QString>& contact);
    void exportContact(QSqlQuery& query);
    void exportStart();

    void writeField(QString name, QString value, QString type="");
    void readField(QString& field, QString& value);
    QTime parseTime(QString time);
};

#endif // ADIF_H
