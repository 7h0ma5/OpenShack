#ifndef ADIF_H
#define ADIF_H

#include <QtCore>

class Adif
{
public:
    static void exportToFile(QString filename);
    static void importFromFile(QString filename);

private:
    static void writeField(QTextStream& out, QString name, QString value, QString type="");
};

#endif // ADIF_H
