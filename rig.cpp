#include <QDebug>
#include "rig.h"

QStringList Rig::rigList = QStringList();

Rig::Rig()
{
}

int Rig::addRig(const struct rig_caps* caps, void* data) {
    rigList << QString("%1 %2 (%3)").arg(caps->mfg_name, caps->model_name, caps->version);
    return -1;
}

void Rig::init() {
    rig_load_all_backends();
    rig_list_foreach(&addRig, NULL);
    rigList.sort();
}

QStringList Rig::allModels() {
    return rigList;
}
