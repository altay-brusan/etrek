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

void ExamTitleWidget::setPatientName(const QString& name)
{
    ui->patientNameValueLbl->setText(name);
}

void ExamTitleWidget::setPatientId(const QString& id)
{
    ui->patientIdValueLbl->setText(id);
}

void ExamTitleWidget::setGender(const QString& gender)
{
    ui->genderValueLbl->setText(gender);
}

void ExamTitleWidget::setAge(int age)
{
    ui->ageValueLbl->setText(QString::number(age));
}

void ExamTitleWidget::setAccessionNumber(const QString& accessionNumber)
{
    ui->accessionNumberValueLbl->setText(accessionNumber);
}
