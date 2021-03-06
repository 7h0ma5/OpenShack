#include <QDebug>
#include <QSettings>
#include "dxwidget.h"
#include "ui_dxwidget.h"

int DxTableModel::rowCount(const QModelIndex&) const {
    return dxData.count();
}

int DxTableModel::columnCount(const QModelIndex&) const {
    return 5;
}

QVariant DxTableModel::data(const QModelIndex& index, int role) const {
    if (role == Qt::DisplayRole) {
        QStringList row = dxData.at(index.row());
        return row.at(index.column());
    }
    return QVariant();
}

QVariant DxTableModel::headerData(int section, Qt::Orientation orientation, int role) const {
    if (role != Qt::DisplayRole || orientation != Qt::Horizontal) return QVariant();

    switch (section) {
    case 0: return tr("Time");
    case 1: return tr("Callsign");
    case 2: return tr("Frequency");
    case 3: return tr("Spotter");
    case 4: return tr("Comment");
    default: return QVariant();
    }
}

void DxTableModel::addEntry(QStringList entry) {
    beginInsertRows(QModelIndex(), dxData.count(), dxData.count());
    dxData.append(entry);
    endInsertRows();
}

QString DxTableModel::getCallsign(const QModelIndex& index) {
    return dxData.at(index.row()).at(1);
}

QString DxTableModel::getFrequency(const QModelIndex& index) {
    return dxData.at(index.row()).at(2);
}

void DxTableModel::clear() {
    beginResetModel();
    dxData.clear();
    endResetModel();
}

DxWidget::DxWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::DxWidget) {

    socket = NULL;

    ui->setupUi(this);
    dxTableModel = new DxTableModel(this);
    ui->dxTable->setModel(dxTableModel);
}

void DxWidget::toggleConnect() {
    if (socket && socket->isOpen()) {
        disconnectCluster();

    }
    else {
        connectCluster();
    }
}

void DxWidget::connectCluster() {
    QStringList server = ui->serverSelect->currentText().split(":");
    QString host = server[0];
    int port = server[1].toInt();

    socket = new QTcpSocket(this);

    connect(socket, SIGNAL(readyRead()), this, SLOT(receive()));
    connect(socket, SIGNAL(connected()), this, SLOT(connected()));
    connect(socket, SIGNAL(error(QAbstractSocket::SocketError)),
            this, SLOT(socketError(QAbstractSocket::SocketError)));

    ui->connectButton->setEnabled(false);
    ui->connectButton->setText(tr("Connecting..."));

    ui->log->clear();
    ui->dxTable->clearSelection();
    dxTableModel->clear();
    ui->dxTable->repaint();

    socket->connectToHost(host, port);
}

void DxWidget::disconnectCluster() {
    ui->sendButton->setEnabled(false);
    ui->connectButton->setEnabled(true);
    ui->connectButton->setText(tr("Connect"));

    socket->disconnect();
    socket->close();

    delete socket;
    socket = NULL;
}

void DxWidget::send() {
    QByteArray data;
    data.append(ui->commandEdit->text());
    data.append("\n");

    socket->write(data);

    ui->commandEdit->clear();
}

void DxWidget::receive() {
    QSettings settings;
    QString data(socket->readAll());
    QStringList lines = data.split(QRegExp("(\a|\n|\r)+"));

    foreach (QString line, lines) {
        if (line.startsWith("login") || line.contains(QRegExp("enter your call(sign)?:"))) {
            QByteArray call = settings.value("operator/callsign").toByteArray();
            call.append("\n");
            socket->write(call);
        }

        if (line.startsWith("DX")) {
            int index = 0;

            QRegExp spotterRegExp("DX DE (([A-Z]|[0-9]|\\/)+):?", Qt::CaseInsensitive);
            index = spotterRegExp.indexIn(line, index);
            QString spotter = spotterRegExp.cap(1);
            index += spotter.size();

            QRegExp freqRegExp("([0-9]+\\.[0-9]+)");
            index = freqRegExp.indexIn(line, index);
            QString freq = freqRegExp.cap(1);
            index += freq.size();

            QRegExp callRegExp("(([A-Z]|[0-9]|\\/)+)");
            index = callRegExp.indexIn(line, index);
            QString call = callRegExp.cap(1);
            index += call.size();

            QRegExp commentRegExp(" (.*) ([0-9]{4})Z");
            index = commentRegExp.indexIn(line, index);
            QString comment = commentRegExp.cap(1).trimmed();
            QString time = commentRegExp.cap(2);

            QStringList entry;
            entry << time << call << freq << spotter << comment;

            dxTableModel->addEntry(entry);
            ui->dxTable->repaint();
        }

        ui->log->appendPlainText(line);
    }
}

void DxWidget::socketError(QAbstractSocket::SocketError) {
    ui->sendButton->setEnabled(false);
    ui->connectButton->setEnabled(true);
    ui->connectButton->setText(tr("Connect"));
}

void DxWidget::connected() {
    ui->sendButton->setEnabled(true);
    ui->connectButton->setEnabled(true);
    ui->connectButton->setText(tr("Disconnect"));
}

void DxWidget::rawModeChanged() {
    if (ui->rawCheckBox->isChecked()) {
        ui->stack->setCurrentIndex(1);
    }
    else {
        ui->stack->setCurrentIndex(0);
    }
}

void DxWidget::entryDoubleClicked(QModelIndex index) {
    QString callsign = dxTableModel->getCallsign(index);
    QString frequency = dxTableModel->getFrequency(index);
    emit tuneDx(callsign, frequency.toDouble()/1000.0);
}

DxWidget::~DxWidget() {
    delete ui;
}
