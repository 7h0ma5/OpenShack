#ifndef RIG_H
#define RIG_H

#include <QtCore>
#include <hamlib/rig.h>

class Rig {
private:
    static QStringList rigList;
    static int addRig(const struct rig_caps* caps, void* data);

public:
    Rig();

    static void init();
    static QStringList allModels();
};

#endif // RIG_H
