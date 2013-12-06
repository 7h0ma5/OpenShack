#include <QtSql>
#include <QMessageBox>
#include <QStyledItemDelegate>
#include "logbookwidget.h"
#include "ui_logbookwidget.h"

class CallsignDelegate : public QStyledItemDelegate {
public:
    CallsignDelegate(QObject *parent = 0) :
        QStyledItemDelegate(parent) { }

    void initStyleOption(QStyleOptionViewItem *option, const QModelIndex& index) const {
        QStyledItemDelegate::initStyleOption(option, index);
        option->font.setBold(true);
    }
};

class DateFormatDelegate : public QStyledItemDelegate {
public:
    DateFormatDelegate(QObject *parent = 0) :
        QStyledItemDelegate(parent) { }

    virtual QString displayText(const QVariant& value, const QLocale& locale) const {
        return value.toDate().toString(locale.dateFormat(QLocale::ShortFormat));
    }
};

class TimeFormatDelegate : public QStyledItemDelegate {
public:
    TimeFormatDelegate(QObject *parent = 0) :
        QStyledItemDelegate(parent) { }

    virtual QString displayText(const QVariant& value, const QLocale& locale) const {
        return value.toTime().toString(locale.timeFormat(QLocale::ShortFormat));
    }
};

class StringFormatDelegate : public QStyledItemDelegate {
public:
    StringFormatDelegate(QString format, QObject *parent = 0) :
        QStyledItemDelegate(parent), format(format) { }

    virtual QString displayText(const QVariant& value, const QLocale&) const {
        return format.arg(value.toString());
    }

private:
    QString format;
};


LogbookWidget::LogbookWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::LogbookWidget)
{
    ui->setupUi(this);

    model = new QSqlTableModel;
    model->setTable("contacts");
    model->setEditStrategy(QSqlTableModel::OnFieldChange);

    model->setHeaderData(1, Qt::Horizontal, tr("Date"));
    model->setHeaderData(2, Qt::Horizontal, tr("Time on"));
    model->setHeaderData(3, Qt::Horizontal, tr("Time off"));
    model->setHeaderData(4, Qt::Horizontal, tr("Call"));
    model->setHeaderData(5, Qt::Horizontal, tr("RST Sent"));
    model->setHeaderData(6, Qt::Horizontal, tr("RST Rcvd"));
    model->setHeaderData(7, Qt::Horizontal, tr("Name"));
    model->setHeaderData(8, Qt::Horizontal, tr("Location"));
    model->setHeaderData(9, Qt::Horizontal, tr("Locator"));
    model->setHeaderData(10, Qt::Horizontal, tr("My Locator"));
    model->setHeaderData(11, Qt::Horizontal, tr("CQ"));
    model->setHeaderData(12, Qt::Horizontal, tr("ITU"));
    model->setHeaderData(13, Qt::Horizontal, tr("Frequency"));
    model->setHeaderData(14, Qt::Horizontal, tr("Band"));
    model->setHeaderData(15, Qt::Horizontal, tr("Mode"));
    model->setHeaderData(16, Qt::Horizontal, tr("Rig"));
    model->setHeaderData(17, Qt::Horizontal, tr("Power"));
    model->setHeaderData(18, Qt::Horizontal, tr("Comment"));
    model->setHeaderData(19, Qt::Horizontal, tr("QSL via"));

    model->select();

    ui->contactTable->setModel(model);
    ui->contactTable->hideColumn(0);
    ui->contactTable->addAction(ui->deleteContact);
    ui->contactTable->sortByColumn(0);
    ui->contactTable->horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);

    ui->contactTable->setItemDelegateForColumn(1, new DateFormatDelegate(ui->contactTable));
    ui->contactTable->setItemDelegateForColumn(2, new TimeFormatDelegate(ui->contactTable));
    ui->contactTable->setItemDelegateForColumn(3, new TimeFormatDelegate(ui->contactTable));
    ui->contactTable->setItemDelegateForColumn(4, new CallsignDelegate(ui->contactTable));
    ui->contactTable->setItemDelegateForColumn(13, new StringFormatDelegate("%1 MHz", ui->contactTable));
    ui->contactTable->setItemDelegateForColumn(17, new StringFormatDelegate("%1 W", ui->contactTable));
}

void LogbookWidget::deleteContact()
{
    QMessageBox::StandardButton reply;
    reply = QMessageBox::question(this, tr("Delete"), tr("Delete the selected contacts?"),
                                  QMessageBox::Yes|QMessageBox::No);

    if (reply != QMessageBox::Yes) return;

    foreach (QModelIndex index, ui->contactTable->selectionModel()->selectedRows()) {
        model->removeRow(index.row());
    }
    ui->contactTable->clearSelection();
    model->select();
}

void LogbookWidget::updateTable() {
    model->select();
}

LogbookWidget::~LogbookWidget()
{
    delete ui;
}
