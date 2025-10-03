#include "RS485SettingsWidget.h"
#include "ui_RS485SettingsWidget.h"

RS485SettingsWidget::RS485SettingsWidget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::RS485SettingsWidget)
{
    ui->setupUi(this);

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

	ui->baudRateComboBox->setStyleSheet(comboCss);
	ui->portNameComboBox->setStyleSheet(comboCss);
	ui->dataBitsComboBox->setStyleSheet(comboCss);
	ui->parityComboBox->setStyleSheet(comboCss);
	ui->stopBitsComboBox->setStyleSheet(comboCss);
    ui->flowControlComboBox->setStyleSheet(comboCss);
    ui->rsModeComboBox->setStyleSheet(comboCss);


}

RS485SettingsWidget::~RS485SettingsWidget()
{
    delete ui;
}
