#ifndef NEWCONTACTWIDGET_H
#define NEWCONTACTWIDGET_H

#include <QWidget>

namespace Ui {
class NewContactWidget;
}

class NewContactWidget : public QWidget {
    Q_OBJECT

public:
    explicit NewContactWidget(QWidget *parent = 0);
    ~NewContactWidget();

signals:
    void contactAdded();

public slots:
    void callsignChanged();
    void resetContact();
    void saveContact();
    void gridChanged();
    void updateTimeOff();
    void startContactTimer();
    void stopContactTimer();

private:
    QTimer* contactTimer;
    Ui::NewContactWidget *ui;
};

#endif // NEWCONTACTWIDGET_H
