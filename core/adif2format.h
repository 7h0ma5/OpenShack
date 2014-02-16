#ifndef ADIF2FORMAT_H
#define ADIF2FORMAT_H

#include "logformat.h"

class Adif2Format : public LogFormat {
public:
    Adif2Format(QTextStream& stream) : LogFormat(stream) {}

private:
    bool importNext(QSqlRecord& contact);
    void exportContact(QSqlRecord& record);
    void exportStart();

    void writeField(QString name, QString value, QString type="");
    void readField(QString& field, QString& value);
    bool readContact(QMap<QString, QString>& contact);
    QTime parseTime(QString time);
};

#endif // ADIF2FORMAT_H
