#include "DetectorControlWidget.h"
#include "ui_DetectorControlWidget.h"

DetectorControlWidget::DetectorControlWidget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::DetectorControlWidget)
{
    ui->setupUi(this);
}

DetectorControlWidget::~DetectorControlWidget()
{
    delete ui;
}
