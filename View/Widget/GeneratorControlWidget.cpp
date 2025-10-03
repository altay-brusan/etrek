#include "GeneratorControlWidget.h"
#include "ui_GeneratorControlWidget.h"

GeneratorControlWidget::GeneratorControlWidget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::GeneratorControlWidget)
{
    ui->setupUi(this);
}

GeneratorControlWidget::~GeneratorControlWidget()
{
    delete ui;
}
