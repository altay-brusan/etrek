#include "ConnectionSetupWidget.h"
#include "ui_ConnectionSetupWidget.h"

#include <QPushButton>
#include <QVBoxLayout>
#include <QDialogButtonBox>
#include <QVariant>
#include <QDialog>

// include settings widget here (not in header)
#include "ConnectionSettingWidget.h"

ConnectionSetupWidget::ConnectionSetupWidget(QWidget* parent)
    : QWidget(parent)
    , ui(new Ui::ConnectionSetupWidget)
{
    ui->setupUi(this);

    const char* buttonCss =
        "QPushButton {"
        "  border:1px solid rgb(120,120,120);"
        "  border-radius:4px;"
        "  background:rgb(74,74,74);"
        "  min-height:28px;"
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
	
    const char* frameCss =
        "QFrame#detectorFrame, "
        "QFrame#tubeFrame, "
        "QFrame#generatorFrame, "
        "QFrame#dapMeterFrame, "
        "QFrame#collimatorFrame {"
        "  border: 1px solid rgb(120,120,120);"
        "  border-radius: 6px;"
        "  margin: 6px;"
        "}";


    // Apply styles
	ui->detector1SettingBtn->setStyleSheet(buttonCss);
	ui->detector2SettingBtn->setStyleSheet(buttonCss);
    ui->detector1PositionerSettingBtn->setStyleSheet(buttonCss);
    ui->detector2PositionerSettingBtn->setStyleSheet(buttonCss);
    ui->tube1PositionerSettingBtn->setStyleSheet(buttonCss);
    ui->tube2PositionerSettingBtn->setStyleSheet(buttonCss);
	ui->generatorSettingBtn->setStyleSheet(buttonCss);
	ui->dapSettingBtn->setStyleSheet(buttonCss);
	ui->collimatorSettingBtn->setStyleSheet(buttonCss);
	
    ui->detectorFrame->setStyleSheet(frameCss);
	ui->tubeFrame->setStyleSheet(frameCss);
	ui->generatorFrame->setStyleSheet(frameCss);
	ui->dapMeterFrame->setStyleSheet(frameCss);
	ui->collimatorFrame->setStyleSheet(frameCss);

    ui->mainHorizontalLayout->setStretch(0, 2);
    ui->mainHorizontalLayout->setStretch(1, 3);

    // Properties for each button
    ui->detector1SettingBtn->setProperty("deviceType", "DETECTOR");
    ui->detector1SettingBtn->setProperty("deviceId", 1);

    ui->detector2SettingBtn->setProperty("deviceType", "DETECTOR");
    ui->detector2SettingBtn->setProperty("deviceId", 2);

    ui->detector1PositionerSettingBtn->setProperty("deviceType", "POSITIONER");
    ui->detector1PositionerSettingBtn->setProperty("deviceId", 1);

    ui->tube1PositionerSettingBtn->setProperty("deviceType", "POSITIONER");
    ui->tube1PositionerSettingBtn->setProperty("deviceId", 2);

    ui->generatorSettingBtn->setProperty("deviceType", "GENERATOR");
    ui->generatorSettingBtn->setProperty("deviceId", 1);

    ui->dapSettingBtn->setProperty("deviceType", "DAP_METER");
    ui->dapSettingBtn->setProperty("deviceId", 1);

    ui->collimatorSettingBtn->setProperty("deviceType", "COLLIMATOR");
    ui->collimatorSettingBtn->setProperty("deviceId", 1);

    // One slot for all buttons
    connect(ui->detector1SettingBtn, &QPushButton::clicked, this, &ConnectionSetupWidget::onDeviceSettingsClicked);
    connect(ui->detector2SettingBtn, &QPushButton::clicked, this, &ConnectionSetupWidget::onDeviceSettingsClicked);
    connect(ui->detector1PositionerSettingBtn, &QPushButton::clicked, this, &ConnectionSetupWidget::onDeviceSettingsClicked);
    connect(ui->detector2PositionerSettingBtn, &QPushButton::clicked, this, &ConnectionSetupWidget::onDeviceSettingsClicked);
    connect(ui->tube1PositionerSettingBtn, &QPushButton::clicked, this, &ConnectionSetupWidget::onDeviceSettingsClicked);
    connect(ui->tube2PositionerSettingBtn, &QPushButton::clicked, this, &ConnectionSetupWidget::onDeviceSettingsClicked);
    connect(ui->generatorSettingBtn, &QPushButton::clicked, this, &ConnectionSetupWidget::onDeviceSettingsClicked);
    connect(ui->dapSettingBtn, &QPushButton::clicked, this, &ConnectionSetupWidget::onDeviceSettingsClicked);
    connect(ui->collimatorSettingBtn, &QPushButton::clicked, this, &ConnectionSetupWidget::onDeviceSettingsClicked);

    // Build the modal dialog ONCE and reuse
    ensureSettingsDialog();
}

ConnectionSetupWidget::~ConnectionSetupWidget()
{
    delete ui; // dialog + settings widget are children; Qt cleans them up
}

void ConnectionSetupWidget::ensureSettingsDialog()
{
    if (m_settingsHostDialog) return;

    m_settingsHostDialog = new QDialog(this);
    m_settingsHostDialog->setModal(true);
    m_settingsHostDialog->setWindowTitle(tr("Device Connection Settings"));

    auto* lay = new QVBoxLayout(m_settingsHostDialog);
    lay->setContentsMargins(12, 12, 12, 12);

    // Create the settings widget and put it inside the dialog
    m_settingsWidget = new ConnectionSettingWidget(m_settingsHostDialog);
    lay->addWidget(m_settingsWidget);

    auto* buttons = new QDialogButtonBox(
        QDialogButtonBox::Ok | QDialogButtonBox::Cancel,
        Qt::Horizontal,
        m_settingsHostDialog
    );
    lay->addWidget(buttons);

    connect(buttons, &QDialogButtonBox::accepted, m_settingsHostDialog, &QDialog::accept);
    connect(buttons, &QDialogButtonBox::rejected, m_settingsHostDialog, &QDialog::reject);
}

void ConnectionSetupWidget::onDeviceSettingsClicked()
{
    auto* btn = qobject_cast<QPushButton*>(sender());
    if (!btn) return;

    const QString deviceType = btn->property("deviceType").toString();
    const int     deviceId = btn->property("deviceId").toInt();

    ensureSettingsDialog();

    // Provide context to the settings widget
    m_settingsWidget->setDeviceContext(deviceType, deviceId);

    // Customize title per device
    m_settingsHostDialog->setWindowTitle(tr("%1 Settings (ID %2)").arg(deviceType).arg(deviceId));

    if (m_settingsHostDialog->exec() == QDialog::Accepted) {
        const QJsonObject params = m_settingsWidget->getParameters();
        saveDeviceConnection(deviceType, deviceId, params);
    }
}

void ConnectionSetupWidget::saveDeviceConnection(const QString& deviceType, int deviceId, const QJsonObject& params)
{
    // TODO: persist (DB/service)
    // Example:
    // ConnectionRepository::instance().save(deviceType, deviceId, params);
    Q_UNUSED(deviceType)
        Q_UNUSED(deviceId)
        Q_UNUSED(params)
}
