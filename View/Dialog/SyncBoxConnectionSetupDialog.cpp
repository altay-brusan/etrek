#include "SyncBoxConnectionSetupDialog.h"
#include "ui_SyncBoxConnectionSetupDialog.h"

SyncBoxConnectionSetupDialog::SyncBoxConnectionSetupDialog(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::SyncBoxConnectionSetupDialog)
{
    ui->setupUi(this);
}

SyncBoxConnectionSetupDialog::~SyncBoxConnectionSetupDialog()
{
    delete ui;
}
