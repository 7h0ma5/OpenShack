#include "rigwidget.h"
#include "ui_rigwidget.h"

RigWidget::RigWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::RigWidget)
{
    ui->setupUi(this);
}

RigWidget::~RigWidget()
{
    delete ui;
}
