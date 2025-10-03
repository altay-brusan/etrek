#include "BodySizeWidget.h"
#include "ui_BodySizeWidget.h"

BodySizeWidget::BodySizeWidget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::BodySizeWidget)
{
    ui->setupUi(this);
}

BodySizeWidget::~BodySizeWidget()
{
    delete ui;
}
