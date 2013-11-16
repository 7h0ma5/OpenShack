#ifndef ADIF_H
#define ADIF_H

#include <QtCore>

class Adif {
public:
    int exportToFile(QString filename);
    int importFromFile(QString filename);

private:
    void writeField(QTextStream& out, QString name, QString value, QString type="");
};

#endif // ADIF_H
