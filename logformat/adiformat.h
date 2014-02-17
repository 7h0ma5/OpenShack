#ifndef ADIFORMAT_H
#define ADIFORMAT_H

#include "logformat.h"

class AdiFormat : public LogFormat {
public:
    AdiFormat(QTextStream& stream) : LogFormat(stream) {}

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
