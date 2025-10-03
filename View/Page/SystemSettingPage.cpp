#include "SystemSettingPage.h"
#include "ui_SystemSettingPage.h"
#include "WorkflowConfigurationWidget.h"
#include "ImageCommentConfigurationWidget.h"
#include "ConnectionSetupWidget.h"
#include "DetectorConfigurationWidget.h"
#include "GeneratorConfigurationWidget.h"
#include "DapConfigurationWidget.h"
#include "CollimatorConfigurationWidget.h"
#include "WorkListConfigurationWidget.h"
#include "PacsEntityConfigurationWidget.h"
#include "ViewConfigurationWidget.h"
#include "ProcedureConfigurationWidget.h"
#include "TechniqueConfigurationWidget.h"

SystemSettingPage::SystemSettingPage(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::SystemSettingPage)
{
    ui->setupUi(this);

    m_workflowConfigurationPlaceholder = new QVBoxLayout(ui->workflowTab);
    m_workflowConfigurationPlaceholder->setContentsMargins(0, 0, 0, 0);

    m_imageCommentPlaceholder = new QVBoxLayout(ui->imageCommentTab);
    m_imageCommentPlaceholder->setContentsMargins(0, 0, 0, 0); //Optional: removes spacing

    m_connectionSetupPlaceholder = new QVBoxLayout(ui->connectionTab);
    m_connectionSetupPlaceholder->setContentsMargins(0, 0, 0, 0); //Optional: removes spacing

    m_detectorConfigurationPlaceholder = new QVBoxLayout(ui->detectorTab);
    m_detectorConfigurationPlaceholder->setContentsMargins(0, 0, 0, 0); //Optional: removes spacing

    m_generatorConfigurationPlaceholder = new QVBoxLayout(ui->generatorTab);
    m_generatorConfigurationPlaceholder->setContentsMargins(0, 0, 0, 0); //Optional: removes spacing

    m_dapConfigurationPlaceholder = new QVBoxLayout(ui->dapTab);
    m_dapConfigurationPlaceholder->setContentsMargins(0, 0, 0, 0); //Optional: removes spacing

    m_collimatorConfigurationPlaceholder = new QVBoxLayout(ui->collimatorTab);
    m_collimatorConfigurationPlaceholder->setContentsMargins(0, 0, 0, 0); //Optional: removes spacing

    m_workListConfigurationPlaceholder = new QVBoxLayout(ui->worklistTab);
    m_workListConfigurationPlaceholder->setContentsMargins(0, 0, 0, 0); //Optional: removes spacing

    m_pacsEntityConfigurationPlaceholder = new QVBoxLayout(ui->pacsEntityTab);
    m_pacsEntityConfigurationPlaceholder->setContentsMargins(0, 0, 0, 0); //Optional: removes spacing

    m_viewConfigureationPlaceholder = new QVBoxLayout(ui->viewTab);
    m_viewConfigureationPlaceholder->setContentsMargins(0, 0, 0, 0);

    m_procedureConfigurationPlaceholder = new QVBoxLayout(ui->procedureTab);
    m_procedureConfigurationPlaceholder->setContentsMargins(0, 0, 0, 0);

    m_techniqueConfigurationPlaceholder = new QVBoxLayout(ui->techniqueSettingTab);
    m_techniqueConfigurationPlaceholder->setContentsMargins(0, 0, 0, 0);

    connect(ui->buttonBox, &QDialogButtonBox::accepted, this, [this]() {
            // Handle OK/Accept button pressed
		    emit saveSettings();

        });

    connect(ui->buttonBox, &QDialogButtonBox::rejected, this, [this]() {
            // Handle Cancel button pressed
		    emit closeSettings();
        });

    connect(ui->buttonBox, &QDialogButtonBox::clicked, this, [this](QAbstractButton* button) {
        if (ui->buttonBox->buttonRole(button) == QDialogButtonBox::ApplyRole) {
		    // Handle Apply button pressed	
            emit applySettings();
        }
        });

    ui->settingTab->setStyleSheet(
        "QTabBar::tab { "
        "background-color: rgb(83, 83, 83); "
        "color: rgb(208, 208, 208); "
        "} "
        "QTabBar::tab:selected { "
        "background-color: rgb(60, 60, 60); "
        "color: rgb(255, 255, 255); "
        "}"
        );


}

void SystemSettingPage::closePage()
{
    hide();  // or setVisible(false);
}

void SystemSettingPage::setWorklistFieldConfigurationWidget(QWidget* widget)
{
    m_worklistFieldConfigurationPlaceholder->addWidget(widget);
}
void SystemSettingPage::setWorkflowConfigurationWidget(QWidget* widget)
{
	m_workflowConfigurationPlaceholder->addWidget(widget);
}

void SystemSettingPage::setConnectionSetupWidget(QWidget* widget)
{
    m_connectionSetupPlaceholder->addWidget(widget);
}

void SystemSettingPage::setImageCommentWidget(QWidget* widget)
{
    m_imageCommentPlaceholder->addWidget(widget);
}

void SystemSettingPage::setDetectorConfigurationWidget(QWidget* widget)
{
	m_detectorConfigurationPlaceholder->addWidget(widget);
}

void SystemSettingPage::setGeneratorConfigurationWidget(QWidget* widget)
{
	m_generatorConfigurationPlaceholder->addWidget(widget);
}

void SystemSettingPage::setDapConfigurationWidget(QWidget* widget)
{
    m_dapConfigurationPlaceholder->addWidget(widget);
}

void SystemSettingPage::setCollimatorConfigurationWidget(QWidget* widget)
{
    m_collimatorConfigurationPlaceholder->addWidget(widget);
}

void SystemSettingPage::setWorkListConfigurationWidget(QWidget* widget)
{
	m_workListConfigurationPlaceholder->addWidget(widget);
}

void SystemSettingPage::setPacsEntityConfigurationWidget(QWidget* widget)
{
	m_pacsEntityConfigurationPlaceholder->addWidget(widget);
}

void SystemSettingPage::setProcedureConfigurationWidget(QWidget* widget)
{
	m_procedureConfigurationPlaceholder->addWidget(widget);
}

void SystemSettingPage::setViewConfigurationWidget(QWidget* widget)
{
	m_viewConfigureationPlaceholder->addWidget(widget);
}

void SystemSettingPage::setTechniqueConfigurationWidget(QWidget* widget)
{
	m_techniqueConfigurationPlaceholder->addWidget(widget);
}

SystemSettingPage::~SystemSettingPage()
{
    delete ui;
}
