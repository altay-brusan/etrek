#include "SetupDirectConnectionDialog.h"
#include "ui_SetupDirectConnectionDialog.h"

SetupDirectConnectionDialog::SetupDirectConnectionDialog(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::SetupDirectConnectionDialog)
{
    ui->setupUi(this);
}

SetupDirectConnectionDialog::~SetupDirectConnectionDialog()
{
    delete ui;
}
