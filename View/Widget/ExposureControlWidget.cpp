#include "ExposureControlWidget.h"
#include "ui_ExposureControlWidget.h"

ExposureControlWidget::ExposureControlWidget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::ExposureControlWidget)
{
    ui->setupUi(this);
}

ExposureControlWidget::~ExposureControlWidget()
{
    delete ui;
}
