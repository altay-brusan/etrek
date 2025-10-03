#include "CollimatorConfigurationWidget.h"
#include "ui_CollimatorConfigurationWidget.h"

CollimatorConfigurationWidget::CollimatorConfigurationWidget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::CollimatorConfigurationWidget)
{
    ui->setupUi(this);
}

CollimatorConfigurationWidget::~CollimatorConfigurationWidget()
{
    delete ui;
}
