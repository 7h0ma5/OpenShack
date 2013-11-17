#ifndef ADIF_H
#define ADIF_H

#include <QtCore>

class Adif {
public:
    int exportToFile(QString filename);
    int importFromFile(QString filename);

private:
    void writeField(QTextStream& out, QString name, QString value, QString type="");
    void readField(QTextStream& file, QString& field, QString& value);
    void insertContact(QMap<QString, QString>& data);
};

#endif // ADIF_H
