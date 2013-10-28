#include <QDebug>
#include "dxwidget.h"
#include "ui_dxwidget.h"

DxWidget::DxWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::DxWidget)
{
    ui->setupUi(this);
    socket = new QTcpSocket(this);

    connect(socket, SIGNAL(readyRead()), this, SLOT(receive()));
    connect(socket, SIGNAL(connected()), this, SLOT(connected()));
    connect(socket, SIGNAL(error(QAbstractSocket::SocketError)),
            this, SLOT(displayError(QAbstractSocket::SocketError)));
}

void DxWidget::toggleConnect() {
    if (socket->isOpen()) {
        ui->sendButton->setEnabled(false);
        ui->connectButton->setEnabled(true);
        ui->connectButton->setText("Connect");

        socket->disconnect();
        socket->close();
    }
    else {
        QStringList server = ui->serverSelect->currentText().split(":");
        QString host = server[0];
        int port = server[1].toInt();

        socket->connectToHost(host, port);
        ui->connectButton->setEnabled(false);
        ui->connectButton->setText("Connecting");
    }
}

void DxWidget::send() {
    QByteArray data;
    data.append(ui->commandEdit->text());
    data.append("\n");

    socket->write(data);

    ui->commandEdit->clear();

    qDebug() << QString(data);
}

void DxWidget::receive() {
    qDebug() << "receiving data";
    QString data(socket->readAll());
    ui->log->appendPlainText(data);
}

void DxWidget::displayError(QAbstractSocket::SocketError) {
    ui->sendButton->setEnabled(false);
    ui->connectButton->setEnabled(true);
    ui->connectButton->setText("Connect");
}

void DxWidget::connected() {
    ui->sendButton->setEnabled(true);
    ui->connectButton->setEnabled(true);
    ui->connectButton->setText("Disconnect");
}

DxWidget::~DxWidget() {
    delete ui;
}
