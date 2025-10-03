#include "ExposureApplicationControlWidget.h"
#include "ui_ExposureApplicationControlWidget.h"

ExposureApplicationControlWidget::ExposureApplicationControlWidget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::ExposureApplicationControlWidget)
{
    ui->setupUi(this);
}

ExposureApplicationControlWidget::~ExposureApplicationControlWidget()
{
    delete ui;
}
