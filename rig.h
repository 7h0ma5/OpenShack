#ifndef RIG_H
#define RIG_H

#include <QtCore>
#include <hamlib/rig.h>

class Rig : public QObject {
    Q_OBJECT

public:
    static Rig* instance();

public slots:
    void start();
    void update();
    void open();

    void setFrequency(double freq);
    void setMode(QString mod);
    void setPower(double power);

signals:
    void frequencyChanged(double freq);
    void modeChanged(QString mode);
    void powerChanged(double power);

private:
    Rig() { }

    Rig(Rig const&);
    void operator=(Rig const&);

    RIG* rig;
    int freq;
    unsigned int power;
    QString mode;
    QMutex rigLock;
};

class RigTypeModel : public QAbstractListModel {
    Q_OBJECT

public:
    RigTypeModel(QObject* parent = 0);
    int rowCount(const QModelIndex& parent = QModelIndex()) const;
    QVariant data(const QModelIndex& index, int role) const;
    QModelIndex index(int row, int column, const QModelIndex& parent = QModelIndex()) const;
    static int addRig(const struct rig_caps* caps, void* data);

private:
    QStringList rigList;
    QMap<QString, int> rigIds;
};

#endif // RIG_H
