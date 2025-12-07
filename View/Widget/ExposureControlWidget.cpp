#include "ExposureControlWidget.h"
#include "ui_ExposureControlWidget.h"

ExposureControlWidget::ExposureControlWidget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::ExposureControlWidget)
{
    ui->setupUi(this);

    // Connect spin boxes to emit parametersChanged signal
    connect(ui->kvpSpinBox, QOverload<int>::of(&QSpinBox::valueChanged), this, [this]() {
        emit parametersChanged(getKvp(), getMa(), getTime(), getMas());
    });
    connect(ui->maSpinBox, QOverload<int>::of(&QSpinBox::valueChanged), this, [this]() {
        emit parametersChanged(getKvp(), getMa(), getTime(), getMas());
    });
    connect(ui->timeSpinBox, QOverload<int>::of(&QSpinBox::valueChanged), this, [this]() {
        emit parametersChanged(getKvp(), getMa(), getTime(), getMas());
    });
    connect(ui->masSpinBox, QOverload<int>::of(&QSpinBox::valueChanged), this, [this]() {
        emit parametersChanged(getKvp(), getMa(), getTime(), getMas());
    });
}

ExposureControlWidget::~ExposureControlWidget()
{
    delete ui;
}

void ExposureControlWidget::setExposureParameters(int kvp, int ma, int time, int mas)
{
    ui->kvpSpinBox->setValue(kvp);
    ui->maSpinBox->setValue(ma);
    ui->timeSpinBox->setValue(time);
    ui->masSpinBox->setValue(mas);
}

void ExposureControlWidget::setKvp(int kvp)
{
    ui->kvpSpinBox->setValue(kvp);
}

void ExposureControlWidget::setMa(int ma)
{
    ui->maSpinBox->setValue(ma);
}

void ExposureControlWidget::setTime(int time)
{
    ui->timeSpinBox->setValue(time);
}

void ExposureControlWidget::setMas(int mas)
{
    ui->masSpinBox->setValue(mas);
}

int ExposureControlWidget::getKvp() const
{
    return ui->kvpSpinBox->value();
}

int ExposureControlWidget::getMa() const
{
    return ui->maSpinBox->value();
}

int ExposureControlWidget::getTime() const
{
    return ui->timeSpinBox->value();
}

int ExposureControlWidget::getMas() const
{
    return ui->masSpinBox->value();
}
