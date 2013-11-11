#ifndef RIGWIDGET_H
#define RIGWIDGET_H

#include <QWidget>

namespace Ui {
class RigWidget;
}

class RigWidget : public QWidget
{
    Q_OBJECT

public:
    explicit RigWidget(QWidget *parent = 0);
    ~RigWidget();

private:
    Ui::RigWidget *ui;
};

#endif // RIGWIDGET_H
