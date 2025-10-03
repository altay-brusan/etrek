#include "ViewPage.h"
#include "ui_ViewPage.h"

ViewPage::ViewPage(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::ViewPage)
{
    ui->setupUi(this);
}

ViewPage::~ViewPage()
{
    delete ui;
}
