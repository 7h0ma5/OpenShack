#ifndef MAPWIDGET_H
#define MAPWIDGET_H

#include <QWidget>
#include <QGraphicsScene>

namespace Ui {
class MapWidget;
}

class MapWidget : public QWidget
{
    Q_OBJECT

public:
    explicit MapWidget(QWidget *parent = 0);
    ~MapWidget();

public slots:
    void setTarget(double lat, double lon);

protected:
    void resizeEvent(QResizeEvent* event);

private:
    void redraw();
    void drawMap();
    void drawDaylightOverlay();
    void drawPoint(double lat, double lon);
    void drawLine(double latA, double lonA, double latB, double lonB);

    QGraphicsScene* scene;
    Ui::MapWidget *ui;
};

#endif // MAPWIDGET_H
