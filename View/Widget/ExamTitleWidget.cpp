#include "ExamTitleWidget.h"
#include "ui_ExamTitleWidget.h"

ExamTitleWidget::ExamTitleWidget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::ExamTitleWidget)
{
    ui->setupUi(this);
}

ExamTitleWidget::~ExamTitleWidget()
{
    delete ui;
}
