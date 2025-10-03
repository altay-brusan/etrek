#include "FocalSpotControlWidget.h"
#include "ui_FocalSpotControlWidget.h"

FocalPointControlWidget::FocalPointControlWidget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::FocalPointControlWidget)
{
    ui->setupUi(this);
}

FocalPointControlWidget::~FocalPointControlWidget()
{
    delete ui;
}
