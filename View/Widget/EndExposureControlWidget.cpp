#include "EndExposureControlWidget.h"
#include "ui_EndExposureControlWidget.h"

EndExposureControlWidget::EndExposureControlWidget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::EndExposureControlWidget)
{
    ui->setupUi(this);
}

EndExposureControlWidget::~EndExposureControlWidget()
{
    delete ui;
}
