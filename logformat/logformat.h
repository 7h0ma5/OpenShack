#ifndef LOGFORMAT_H
#define LOGFORMAT_H

#include <QtCore>
#include <QMap>
#include <QtSql>

class LogFormat
{
public:
    LogFormat(QTextStream& stream);
    ~LogFormat();

    int runImport();
    int runExport();
    void setDefaults(QMap<QString, QString>& defaults);

protected:
    virtual void importStart() {}
    virtual void importEnd() {}
    virtual bool importNext(QMap<QString, QString>&) { return false; }

    virtual void exportStart() {}
    virtual void exportEnd() {}
    virtual void exportContact(QSqlQuery&) {}

    QTextStream& stream;
    QMap<QString, QString>* defaults;
};

#endif // LOGFORMAT_H
