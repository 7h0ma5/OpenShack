#ifndef IMPORTDIALOG_H
#define IMPORTDIALOG_H

#include <QDialog>

namespace Ui {
class ImportDialog;
}

class ImportDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ImportDialog(QWidget *parent = 0);
    ~ImportDialog();

public slots:
    void browse();
    void toggleAll();
    void import();

private:
    Ui::ImportDialog *ui;
};

#endif // IMPORTDIALOG_H
