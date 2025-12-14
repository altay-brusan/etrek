#include "ExposureApplicationControlWidget.h"
#include "ui_ExposureApplicationControlWidget.h"

ExposureApplicationControlWidget::ExposureApplicationControlWidget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::ExposureApplicationControlWidget)
{
    ui->setupUi(this);

    // Connect radio buttons to emit applicationModeChanged signal
    connect(ui->maintenanceRadioButton, &QRadioButton::toggled, this, [this](bool checked) {
        if (checked) emit applicationModeChanged(ApplicationMode::Maintenance);
    });
    connect(ui->diagnosticRadioButton, &QRadioButton::toggled, this, [this](bool checked) {
        if (checked) emit applicationModeChanged(ApplicationMode::Diagnostic);
    });
}

ExposureApplicationControlWidget::~ExposureApplicationControlWidget()
{
    delete ui;
}

void ExposureApplicationControlWidget::setApplicationMode(ApplicationMode mode)
{
    switch (mode) {
        case ApplicationMode::Maintenance:
            ui->maintenanceRadioButton->setChecked(true);
            break;
        case ApplicationMode::Diagnostic:
            ui->diagnosticRadioButton->setChecked(true);
            break;
    }
}

ApplicationMode ExposureApplicationControlWidget::getApplicationMode() const
{
    if (ui->maintenanceRadioButton->isChecked()) return ApplicationMode::Maintenance;
    if (ui->diagnosticRadioButton->isChecked()) return ApplicationMode::Diagnostic;

    // Default to Diagnostic
    return ApplicationMode::Diagnostic;
}
