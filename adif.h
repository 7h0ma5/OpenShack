#ifndef ADIF_H
#define ADIF_H

#include <QtCore>

class Adif {
public:
    int exportToFile(QString filename);
    int importFromFile(QString filename);
    int import(QTextStream& in);

private:
    void writeField(QTextStream& out, QString name, QString value, QString type="");
    void readField(QTextStream& file, QString& field, QString& value);
    void insertContact(QMap<QString, QString>& data);
    QTime parseTime(QString time);
};

#endif // ADIF_H
