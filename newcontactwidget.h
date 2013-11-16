#ifndef NEWCONTACTWIDGET_H
#define NEWCONTACTWIDGET_H

#include <QWidget>
#include "hamqth.h"
#include "cty.h"

namespace Ui {
class NewContactWidget;
}

class HamQTH;

enum CoordPrecision {
    COORD_NONE = 0,
    COORD_DXCC = 1,
    COORD_GRID = 2,
    COORD_FULL = 3
};

class NewContactWidget : public QWidget {
    Q_OBJECT

public:
    explicit NewContactWidget(QWidget *parent = 0);
    ~NewContactWidget();

signals:
    void contactAdded();
    void newTarget(double lat, double lon);

public slots:
    void callsignChanged();
    void frequencyChanged();
    void resetContact();
    void saveContact();
    void gridChanged();
    void updateTimeOff();
    void startContactTimer();
    void stopContactTimer();
    void callsignResult(const QMap<QString, QString>& data);
    void updateCoordinates(double lat, double lon, CoordPrecision prec);

private:
    QString callsign;
    Cty cty;
    HamQTH callbook;
    QTimer* contactTimer;
    Ui::NewContactWidget *ui;
    CoordPrecision coordPrec;
};

#endif // NEWCONTACTWIDGET_H
