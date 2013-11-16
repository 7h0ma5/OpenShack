#include <QGraphicsTextItem>
#include <QTime>
#include <QDebug>
#include <cmath>
#include "mapwidget.h"
#include "ui_mapwidget.h"

MapWidget::MapWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::MapWidget)
{
    ui->setupUi(this);

    QPixmap pix(":/data/map/nasabluemarble.jpg");
    scene = new QGraphicsScene(ui->mapView);
    scene->addPixmap(pix);
    scene->setSceneRect(pix.rect());
    ui->mapView->setScene(scene);
    ui->mapView->fitInView(scene->sceneRect(), Qt::KeepAspectRatio);

    drawPoint(50.775278, 6.082778);
    drawLine(50.775278, 6.082778, 43, -75);
    drawDaylightOverlay();
}

void MapWidget::drawPoint(double lat, double lon) {
    int x = (lon+180)/360*scene->width();
    int y = scene->height()/2 - (lat/90)*(scene->height()/2);
    scene->addEllipse(x, y, 5, 5, QPen(QColor(255, 0, 0)), QBrush(QColor(255, 0, 0), Qt::SolidPattern));
    qDebug() << "draw point" << lat << lon << "to" << x << y;
}

void MapWidget::drawLine(double latA, double lonA, double latB, double lonB) {
    QPainterPath path;

    int xA = (lonA+180)/360*scene->width();
    int yA = scene->height()/2 - (latA/90)*(scene->height()/2);

    int xB = (lonB+180)/360*scene->width();
    int yB = scene->height()/2 - (latB/90)*(scene->height()/2);

    path.moveTo(xA,yA);
    path.lineTo(xB, yB);

    path.closeSubpath();

    scene->addPath(path, QPen(QColor(255, 0, 0)), QBrush(QColor(255, 0, 0), Qt::SolidPattern));
}

void MapWidget::drawDaylightOverlay() {
    int secondOfDay = QTime(0, 0, 0).secsTo(QTime::currentTime());
    int dayOfYear = QDate::currentDate().dayOfYear();
/*
    double offset_x = (time.hour() * 3600 + time.minute() * 60 + time.second())/86400.0;

    int equinox = QDate(0, 3, 20).dayOfYear();
    double offset_sin = ((365 - equinox + date.dayOfYear()) % 365)/365.0;
    double offset_y = sin(offset_sin*2*M_PI);

    drawPoint(90+(offset_y*180), 180-(offset_x*360));

    qDebug() << offset_x << offset_sin << offset_y;
                */
}

void MapWidget::resizeEvent(QResizeEvent*) {
    ui->mapView->fitInView(scene->sceneRect(), Qt::KeepAspectRatio);
}

MapWidget::~MapWidget()
{
    delete ui;
}
