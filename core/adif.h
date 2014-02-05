#ifndef ADIF_H
#define ADIF_H

#include "logformat.h"

class Adif : public LogFormat {
public:
    Adif(QTextStream& stream) : LogFormat(stream) {}

private:
    bool importNext(QSqlRecord& contact);
    void exportContact(QSqlRecord& record);
    void exportStart();

    void writeField(QString name, QString value, QString type="");
    void readField(QString& field, QString& value);
    bool readContact(QMap<QString, QString>& contact);
    QTime parseTime(QString time);
};

#endif // ADIF_H
