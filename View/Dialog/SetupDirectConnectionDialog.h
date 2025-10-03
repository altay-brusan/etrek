#ifndef SETUPDIRECTCONNECTIONDIALOG_H
#define SETUPDIRECTCONNECTIONDIALOG_H

#include <QDialog>

namespace Ui {
class SetupDirectConnectionDialog;
}

class SetupDirectConnectionDialog : public QDialog
{
    Q_OBJECT

public:
    explicit SetupDirectConnectionDialog(QWidget *parent = nullptr);
    ~SetupDirectConnectionDialog();

private:
    Ui::SetupDirectConnectionDialog *ui;
};

#endif // SETUPDIRECTCONNECTIONDIALOG_H
