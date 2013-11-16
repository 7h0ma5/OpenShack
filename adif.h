#ifndef ADIF_H
#define ADIF_H

#include <QtCore>

class Adif {
public:
    void exportToFile(QString filename);
    void importFromFile(QString filename);

private:
    void writeField(QTextStream& out, QString name, QString value, QString type="");
};

#endif // ADIF_H
