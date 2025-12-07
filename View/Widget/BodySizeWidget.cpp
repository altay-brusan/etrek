#include "BodySizeWidget.h"
#include "ui_BodySizeWidget.h"

BodySizeWidget::BodySizeWidget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::BodySizeWidget)
{
    ui->setupUi(this);

    // Connect radio buttons to emit patientSizeChanged signal
    connect(ui->infantRadioButton, &QRadioButton::toggled, this, [this](bool checked) {
        if (checked) emit patientSizeChanged(PatientSize::Infant);
    });
    connect(ui->smallRadioButton, &QRadioButton::toggled, this, [this](bool checked) {
        if (checked) emit patientSizeChanged(PatientSize::Small);
    });
    connect(ui->normalRadioButton, &QRadioButton::toggled, this, [this](bool checked) {
        if (checked) emit patientSizeChanged(PatientSize::Normal);
    });
    connect(ui->largeRadioButton, &QRadioButton::toggled, this, [this](bool checked) {
        if (checked) emit patientSizeChanged(PatientSize::Large);
    });
}

BodySizeWidget::~BodySizeWidget()
{
    delete ui;
}

void BodySizeWidget::setPatientSize(PatientSize size)
{
    switch (size) {
        case PatientSize::Infant:
            ui->infantRadioButton->setChecked(true);
            break;
        case PatientSize::Small:
            ui->smallRadioButton->setChecked(true);
            break;
        case PatientSize::Normal:
            ui->normalRadioButton->setChecked(true);
            break;
        case PatientSize::Large:
            ui->largeRadioButton->setChecked(true);
            break;
    }
}

PatientSize BodySizeWidget::getPatientSize() const
{
    if (ui->infantRadioButton->isChecked()) return PatientSize::Infant;
    if (ui->smallRadioButton->isChecked()) return PatientSize::Small;
    if (ui->normalRadioButton->isChecked()) return PatientSize::Normal;
    if (ui->largeRadioButton->isChecked()) return PatientSize::Large;

    // Default to Normal if nothing is checked
    return PatientSize::Normal;
}
