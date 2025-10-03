#include "AecControlWidget.h"
#include "ui_AecControlWidget.h"

AecControlWidget::AecControlWidget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::AecControlWidget)
{
    ui->setupUi(this);
}

AecControlWidget::~AecControlWidget()
{
    delete ui;
}
