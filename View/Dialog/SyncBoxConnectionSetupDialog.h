#ifndef SYNCBOXCONNECTIONSETUPDIALOG_H
#define SYNCBOXCONNECTIONSETUPDIALOG_H

#include <QDialog>

namespace Ui {
class SyncBoxConnectionSetupDialog;
}

class SyncBoxConnectionSetupDialog : public QDialog
{
    Q_OBJECT

public:
    explicit SyncBoxConnectionSetupDialog(QWidget *parent = nullptr);
    ~SyncBoxConnectionSetupDialog();

private:
    Ui::SyncBoxConnectionSetupDialog *ui;
};

#endif // SYNCBOXCONNECTIONSETUPDIALOG_H
