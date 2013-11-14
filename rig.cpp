#include <QDebug>
#include <QSettings>
#include <cstring>
#include "rig.h"

RIG* Rig::rig = 0;

void Rig::connect() {
    QSettings settings;
    int model = settings.value("rig/model").toInt();
    int baudrate = settings.value("rig/baudrate").toInt();
    QByteArray portStr = settings.value("rig/port").toByteArray();
    const char* port = portStr.constData();

    rig = rig_init(model);
    strncpy(rig->state.rigport.pathname, port, FILPATHLEN - 1);
    rig->state.rigport.parm.serial.rate = baudrate;

    int status = rig_open(rig);

    if (status != RIG_OK) {
        qWarning() << "rig connection error";
    }
    else {
        qDebug() << "connected to rig";
    }

    freq_t freq;
    rig_get_freq(rig, RIG_VFO_A, &freq);

    qDebug() << "freq" << freq;
}

void Rig::setFrequency(double freq) {
    rig_set_freq(rig, RIG_VFO_A, freq);
}

RigTypeModel::RigTypeModel(QObject* parent)
    : QAbstractListModel(parent)
{
    rig_load_all_backends();
    rig_list_foreach(addRig, this);
    rigList.sort();
}

int RigTypeModel::rowCount(const QModelIndex& parent) const {
    return rigList.count();
}

QVariant RigTypeModel::data(const QModelIndex& index, int role) const {
    if (role == Qt::DisplayRole) {
        qDebug() << "value" << index.row();
        return rigList.value(index.row());
    }
    return QVariant();
}

QModelIndex RigTypeModel::index(int row, int column, const QModelIndex& parent) const {
    if (!hasIndex(row, column, parent)) {
        return QModelIndex();
    }

    int rigId = rigIds[rigList.value(row)];
    if (rigId)
        return createIndex(row, column, rigId);
    else
        return QModelIndex();
}

int RigTypeModel::addRig(const struct rig_caps* caps, void* data) {
    RigTypeModel* rigTypeModel = (RigTypeModel*)data;
    QString name = QString("%1 %2 (%3)").arg(caps->mfg_name, caps->model_name, caps->version);
    rigTypeModel->rigList.append(name);
    rigTypeModel->rigIds[name] = caps->rig_model;
    return -1;
}
