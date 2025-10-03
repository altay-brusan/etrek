#include "DapConfigurationWidget.h"
#include "ui_DapConfigurationWidget.h"

DapConfigurationWidget::DapConfigurationWidget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::DapConfigurationWidget)
{
    ui->setupUi(this);

    const char* groupCss =
        "QGroupBox {"
        "  border:1px solid rgb(120,120,120); border-radius:6px; margin-top:18px; }"
        "QGroupBox::title { subcontrol-origin: margin; left:10px; padding:0 6px; }";

    const char* comboCss =
        "QComboBox {"
        "  border:1px solid rgb(120,120,120); border-radius:4px;"
        "  background:rgb(74,74,74); min-height:28px; padding:2px 28px 2px 6px;" /* room for arrow */
        "}"
        "QComboBox:focus { border-color: rgb(160,160,160); }"

        /* drop-down button area */
        "QComboBox::drop-down {"
        "  width:26px; border-left:1px solid rgb(120,120,120); margin:0;"
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

    const char* doubleSpineCss =
        "QDoubleSpinBox  {"
        "  border:1px solid rgb(120,120,120); border-radius:4px;"
        "  background:rgb(74,74,74); min-height:28px; padding:2px 6px; }"
        "QDoubleSpinBox :focus { border-color: rgb(160,160,160); }";


	ui->settingGroupBox->setStyleSheet(groupCss);
	ui->modelComboBox->setStyleSheet(comboCss);
	ui->devicePositionComboBox->setStyleSheet(comboCss);
	ui->calibrationFactorDoubleSpinBox->setStyleSheet(doubleSpineCss);
	ui->resolutionComboBox->setStyleSheet(comboCss);
	ui->measuringUnitComboBox->setStyleSheet(comboCss);

}

DapConfigurationWidget::~DapConfigurationWidget()
{
    delete ui;
}
