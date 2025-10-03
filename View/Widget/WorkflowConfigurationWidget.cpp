#include "WorkflowConfigurationWidget.h"
#include "ui_WorkflowConfigurationWidget.h"
#include "EnvironmentSetting.h"
#include "EnvironmentSettingUtils.h" 

using namespace Etrek::Device;

WorkflowConfigurationWidget::WorkflowConfigurationWidget(const EnvironmentSetting& environmentSetting,const QVector<GeneralEquipment>& equipments, const QVector<Institution>& institutes,QWidget *parent)
    : QWidget(parent)
	, m_currentSettings(environmentSetting)
	, m_equipments(equipments)
	, m_institutions(institutes)
    , ui(new Ui::WorkflowConfigurationWidget)
{
    ui->setupUi(this);
    setStile();
    populateFromFirstEquipment();
    populateEnvironmentSettingsUI();
}

void WorkflowConfigurationWidget::populateFromFirstEquipment()
{
    if (m_equipments.isEmpty()) return;

    const GeneralEquipment& ge = m_equipments.first();

    // ----- Device group -----
    ui->deviceSerialNumberLineEdit->setText(ge.DeviceSerialNumber);
    ui->deviceUidLineEdit->setText(ge.DeviceUID);
    ui->manufacturerLineEdit->setText(ge.Manufacturer);
    ui->modelNameLineEdit->setText(ge.ModelName);
    ui->softwareVersionLineEdit->setText(ge.SoftwareVersions);
    ui->positionerLineEdit->setText(ge.GantryId);             // using GantryId as "Positioner"
    ui->stationNameLineEdit->setText(ge.StationName);
    ui->departmentLineEdit->setText(ge.DepartmentName);

    // Manufacture Date (read-only)
    if (ge.DateOfManufacture.isValid())
        ui->manufactureDateEdit->setDate(ge.DateOfManufacture);
    ui->manufactureDateEdit->setReadOnly(true);
    ui->manufactureDateEdit->setButtonSymbols(QAbstractSpinBox::NoButtons);
    ui->manufactureDateEdit->setCalendarPopup(false);

    // Calibration Date (editable as usual unless you want read-only too)
    if (ge.DateOfLastCalibration.isValid())
        ui->calibrationDateEdit->setDate(ge.DateOfLastCalibration);

    // ----- Site group (Institution) -----
    // Prefer the institution embedded on GE; if not found, try to resolve from the provided list.
    Institution inst = ge.Institution;
    if (inst.Id <= 0 && !m_institutions.isEmpty()) {
        // fallback: try to match by name if present
        if (!inst.Name.isEmpty()) {
            for (const auto& i : m_institutions) {
                if (i.Name == inst.Name) { inst = i; break; }
            }
        }
        else if (ge.Institution.Id > 0) {
            for (const auto& i : m_institutions) {
                if (i.Id == ge.Institution.Id) { inst = i; break; }
            }
        }
    }

    ui->institutionNameLineEdit->setText(inst.Name);
    ui->institutionTypeLineEdit->setText(inst.InstitutionType);
    ui->institutionIdLineEdit->setText(inst.InstitutionId);
    ui->addressLineEdit->setText(inst.Address);
    ui->contactInfoLineEdit->setText(inst.ContactInformation);
}


void WorkflowConfigurationWidget::setStile()
{
    // ----- minimal styles -----
    const char* lineCss =
        "QLineEdit {"
        "  border:1px solid rgb(120,120,120);"
        "  border-radius:4px;"
        "  background:rgb(74,74,74);"
        "  min-height:20px;"
        "  padding:2px 6px;"
        "  width:18px;"
        "}"
        "QLineEdit:focus {"
        "  border-color: rgb(160,160,160);"
        "}"
        /* read-only but still enabled */
        "QLineEdit:read-only:enabled {"
        "  color: rgb(160,160,160);"
        "}"
        /* invalid cue (by objectName) */
        "QLineEdit#hostIPLineEdit[invalid=\"true\"] {"
        "  border:1px solid rgb(200,80,80);"
        "}";


    const char* spinCss =
        "QSpinBox {"
        "  border:1px solid rgb(120,120,120); border-radius:4px;"
        "  background:rgb(74,74,74); min-height:20px; padding:2px 6px; }"
        "QSpinBox:focus { border-color: rgb(160,160,160); }";

    const char* listCss =
        "QListView {"
        "  border: 1px solid rgb(120,120,120);"
        "  border-radius: 4px;"
        "  background: rgb(74,74,74);"   // contrast with the container bg
        "}";

    const char* buttonCss =
        "QPushButton {"
        "  border:1px solid rgb(120,120,120);"
        "  border-radius:4px;"
        "  background:rgb(74,74,74);"
        "  min-height:20px;"
        "  padding:4px 12px;"
        "  color: rgb(230,230,230);"
        "}"
        "QPushButton:hover {"
        "  border-color: rgb(160,160,160);"
        "  background:rgb(92,92,92);"
        "}"
        "QPushButton:pressed {"
        "  background:rgb(64,64,64);"
        "}"
        "QPushButton:focus {"
        "  border-color: rgb(180,180,180);"
        "}"
        "QPushButton:disabled {"
        "  border-color: rgb(80,80,80);"
        "  background:rgb(60,60,60);"
        "  color: rgb(130,130,130);"
        "}";

    const char* checkCss =
        "QCheckBox { spacing: 8px; }"
        "QCheckBox::indicator { width:16px; height:16px; }"
        "QCheckBox::indicator:unchecked {"
        "  border:1px solid rgb(120,120,120); background:rgb(74,74,74); border-radius:3px; }"
        "QCheckBox::indicator:unchecked:hover { border-color: rgb(160,160,160); }"
        "QCheckBox::indicator:checked {"
        "  border:1px solid rgb(160,160,160); background:rgb(54,69,79); border-radius:3px; }";

    const char* comboCss =
        "QComboBox {"
        "  border:1px solid rgb(120,120,120); border-radius:4px;"
        "  background:rgb(74,74,74); min-height:20px; padding:2px 20px 2px 6px;" /* room for arrow */
        "}"
        "QComboBox:focus { border-color: rgb(160,160,160); }"

        /* drop-down button area */
        "QComboBox::drop-down {"
        "  width:20px; border-left:1px solid rgb(120,120,120); margin:0;"
        "}"

        /* the arrow itself */
        "QComboBox::down-arrow {"
        "  image: url(:/Images/Asset/Icon/chevron-down.svg);"
        "  width: 12px; height: 12px;"
        "}"
        "QComboBox::down-arrow:disabled {"
        "  image: url(:/Images/Asset/Icon/chevron-down.svg);"
        "  /* optional: reduce opacity via a lighter icon if you have one */"
        "}"

        /* popup list */
        "QComboBox QAbstractItemView {"
        "  border:1px solid rgb(120,120,120); background:rgb(74,74,74);"
        "  selection-background-color: rgb(92,92,92);"
        "}";

    const char* groupCss =
        "QGroupBox {"
        "  border:1px solid rgb(120,120,120); border-radius:6px; margin-top:18px; }"
        "QGroupBox::title { subcontrol-origin: margin; left:10px; padding:0 6px; }";

    const char* dateCss =
        "QDateEdit {"
        "  border:1px solid rgb(120,120,120); border-radius:4px;"
        "  background:rgb(74,74,74); min-height:20px; padding:2px 6px; }"
        "QDateEdit:focus { border-color: rgb(160,160,160); }"
        "QDateEdit[readOnly = \"true\"] { color: rgb(160,160,160); }"
        "QDateEdit::drop-down { width:20px; border-left:1px solid rgb(120,120,120); margin:0; }";


    ui->workflowConfigurationGroupBox->setStyleSheet(groupCss);
    ui->siteGroupBox->setStyleSheet(groupCss);
    ui->deviceGroupBox->setStyleSheet(groupCss);

    // FIX: correct object name
    ui->studyLevelComboBox->setStyleSheet(comboCss);
    ui->lutTypeComboBox->setStyleSheet(comboCss);

    ui->workListClearPeriodLblSpinBox->setStyleSheet(spinCss);
    ui->refreshWorklistPeriodSpinBox->setStyleSheet(spinCss);
    ui->deleteLogFilePeriodSpinBox->setStyleSheet(spinCss);

    ui->automaticRefreshWorklistCheckBox->setStyleSheet(checkCss);
    ui->automaticFreeUpDiskSpaceCheckBox->setStyleSheet(checkCss);
    ui->enableMppsCheckBox->setStyleSheet(checkCss);
    ui->continueOnEchoFailCheckBox->setStyleSheet(checkCss);

    ui->institutionNameLineEdit->setStyleSheet(lineCss);
    ui->stationNameLineEdit->setStyleSheet(lineCss);
    ui->institutionIdLineEdit->setStyleSheet(lineCss);
    ui->departmentLineEdit->setStyleSheet(lineCss);
    ui->deviceSerialNumberLineEdit->setStyleSheet(lineCss);
    ui->deviceUidLineEdit->setStyleSheet(lineCss);
    ui->manufacturerLineEdit->setStyleSheet(lineCss);
    ui->modelNameLineEdit->setStyleSheet(lineCss);
    ui->softwareVersionLineEdit->setStyleSheet(lineCss);
    ui->positionerLineEdit->setStyleSheet(lineCss);
    ui->institutionTypeLineEdit->setStyleSheet(lineCss);
    ui->addressLineEdit->setStyleSheet(lineCss);
    ui->contactInfoLineEdit->setStyleSheet(lineCss);

    ui->manufactureDateEdit->setStyleSheet(dateCss);
    ui->calibrationDateEdit->setStyleSheet(dateCss);

    ui->clearDatabaseBtn->setStyleSheet(buttonCss);
    ui->resetDatabaseBtn->setStyleSheet(buttonCss);
    ui->activateBtn->setStyleSheet(buttonCss);

    // (Optional) set ranges/defaults explicitly
    ui->workListClearPeriodLblSpinBox->setRange(0, 365);
    ui->refreshWorklistPeriodSpinBox->setRange(1, 999);
    ui->deleteLogFilePeriodSpinBox->setRange(0, 365);
}

void WorkflowConfigurationWidget::populateEnvironmentSettingsUI()
{
      // --- Combo: Study Level ---
    {
        const QString studyStr = EnvironmentSettingUtils::toString(m_currentSettings.StudyLevelValue);
        int idx = ui->studyLevelComboBox->findText(studyStr, Qt::MatchFixedString);
        if (idx < 0) {
            // fallback by known strings if enum-text mismatch ever happens
            idx = (studyStr.compare("Single-Series Study", Qt::CaseInsensitive) == 0) ? 1 : 0;
        }
        ui->studyLevelComboBox->setCurrentIndex(idx);
    }

    // --- Combo: LUT ---
    {
        const QString lutStr = EnvironmentSettingUtils::toString(m_currentSettings.LookupTableValue);
        int idx = ui->lutTypeComboBox->findText(lutStr, Qt::MatchFixedString);
        if (idx < 0) {
            // fallback: treat unknown as "None"
            idx = ui->lutTypeComboBox->findText("None", Qt::MatchFixedString);
            if (idx < 0) idx = 0;
        }
        ui->lutTypeComboBox->setCurrentIndex(idx);
    }

    // --- Spin boxes ---
    ui->workListClearPeriodLblSpinBox->setValue(m_currentSettings.WorklistClearPeriodDays);
    ui->refreshWorklistPeriodSpinBox->setValue(m_currentSettings.WorklistRefreshPeriodSeconds);
    ui->deleteLogFilePeriodSpinBox->setValue(m_currentSettings.DeleteLogPeriodDays);

    // --- Flags ---
    ui->automaticRefreshWorklistCheckBox->setChecked(m_currentSettings.AutoRefreshWorklist);
    ui->automaticFreeUpDiskSpaceCheckBox->setChecked(m_currentSettings.AutoClearDiskSpace);
    ui->enableMppsCheckBox->setChecked(m_currentSettings.EnableMPPS);
    ui->continueOnEchoFailCheckBox->setChecked(m_currentSettings.ContinueOnEchoFail);
}

WorkflowConfigurationWidget::~WorkflowConfigurationWidget()
{
    delete ui;
}
