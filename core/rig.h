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

#endif // RIG_H
