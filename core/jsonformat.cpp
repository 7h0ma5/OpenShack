#include <QJsonArray>
#include <QJsonDocument>
#include <QSqlRecord>
#include "core/jsonformat.h"

void JsonFormat::exportContact(QSqlRecord& record) {
    QJsonObject contact;
    int fieldCount = record.count();
    for (int i = 0; i < fieldCount; i++) {
        QString fieldName = record.fieldName(i);
        QString fieldValue = record.value(i).toString();
        if (fieldValue.isEmpty()) continue;
        contact[fieldName] = QJsonValue(fieldValue);
    }
    data.append(contact);
}

void JsonFormat::exportEnd() {
    QJsonDocument doc(data);
    QByteArray json = doc.toJson();
    stream << json;
}

bool JsonFormat::importNext(QSqlRecord& record) {
    return false;
}
