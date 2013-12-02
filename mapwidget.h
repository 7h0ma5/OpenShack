#ifndef MAPWIDGET_H
#define MAPWIDGET_H

#include <QWidget>
#include <QGraphicsScene>

namespace Ui {
class MapWidget;
}

class QVector2D;

class MapWidget : public QWidget
{
    Q_OBJECT

public:
    explicit MapWidget(QWidget* parent = 0);
    ~MapWidget();

public slots:
    void setTarget(double lat, double lon);
    void redraw();

protected:
    void showEvent(QShowEvent* event);
    void resizeEvent(QResizeEvent* event);

private:
    void drawMap();
    void drawNightOverlay();
    void drawPoint(QPoint point);
    void drawLine(QPoint pointA, QPoint pointB);

    void pointToRad(QPoint point, double& lat, double& lon);
    void pointToCoord(QPoint point, double& lat, double& lon);
    QPoint radToPoint(double lat, double lon);
    QPoint coordToPoint(double lat, double lon);

    QGraphicsScene* scene;
    Ui::MapWidget* ui;
};

#endif // MAPWIDGET_H
