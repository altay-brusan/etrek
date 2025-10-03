#include "BuckyControlWidget.h"
#include "ui_BuckyControlWidget.h"

BuckyControlWidget::BuckyControlWidget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::BuckyControlWidget)
{
    ui->setupUi(this);
}

BuckyControlWidget::~BuckyControlWidget()
{
    delete ui;
}
