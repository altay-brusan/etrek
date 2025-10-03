#include "StudyControlWidget.h"
#include "ui_StudyControlWidget.h"

StudyControlWidget::StudyControlWidget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::StudyControlWidget)
{
    ui->setupUi(this);
}

StudyControlWidget::~StudyControlWidget()
{
    delete ui;
}
