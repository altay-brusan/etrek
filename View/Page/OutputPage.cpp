#include "OutputPage.h"
#include "ui_OutputPage.h"

OutputPage::OutputPage(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::OutputPage)
{
    ui->setupUi(this);
}

OutputPage::~OutputPage()
{
    delete ui;
}
