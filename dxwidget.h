#ifndef DXWIDGET_H
#define DXWIDGET_H

#include <QWidget>
#include <QtNetwork>

namespace Ui {
class DxWidget;
}

class DxWidget : public QWidget {
    Q_OBJECT

public:
    explicit DxWidget(QWidget *parent = 0);
    ~DxWidget();

public slots:
    void toggleConnect();
    void receive();
    void send();
    void connected();
    void displayError(QAbstractSocket::SocketError error);

private:
    QTcpSocket* socket;
    Ui::DxWidget *ui;
};

#endif // DXWIDGET_H
