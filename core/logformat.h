#ifndef LOGFORMAT_H
#define LOGFORMAT_H

#include <QtCore>
#include <QMap>
#include <QtSql>

class LogFormat
{
public:
    LogFormat(QTextStream& stream);
    virtual ~LogFormat();

    int runImport();
    int runExport();
    void setDefaults(QMap<QString, QString>& defaults);
    void setDateRange(QDate start, QDate end);

protected:
    virtual void importStart() {}
    virtual void importEnd() {}
    virtual bool importNext(QMap<QString, QString>&) { return false; }

    virtual void exportStart() {}
    virtual void exportEnd() {}
    virtual void exportContact(QSqlQuery&) {}

    QTextStream& stream;
    QMap<QString, QString>* defaults;

private:
    bool inDateRange(QString date);
    bool inDateRange(QDate date);
    QDate startDate, endDate;
};

#endif // LOGFORMAT_H
