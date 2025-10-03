#include "ConnectionSettingWidget.h"
#include "ui_ConnectionSettingWidget.h"

#include <QComboBox>
#include <QVBoxLayout>
#include <QLabel>
#include <QMetaType>

// Settings detail widgets (fix names/paths as in your project)
#include "RS232SettingsWidget.h"
#include "RS485SettingsWidget.h"
#include "CanBusSettingsWidget.h"
#include "ModBusRTUSettingsWidget.h"
// NOTE: ensure the header/class name is correct. If your class is ModBusTCPSettingsWidget,
// include that and use the same class name below (NOT "ModeBus...").
#include "ModBusTCPSettingsWidget.h"
#include "EthernetSettingsWidget.h"

using namespace Etrek::Device;

ConnectionSettingWidget::ConnectionSettingWidget(QWidget* parent)
    : QWidget(parent)
    , ui(new Ui::ConnectionSettingWidget)
{
    ui->setupUi(this);

    // If you store enums in QVariant, it helps to register them.
    qRegisterMetaType<ConnectionProtocol>("Etrek::Device::ConnectionProtocol");

    buildFactories();
    populateProtocols();

    connect(ui->protocolListComboBox,
        qOverload<int>(&QComboBox::currentIndexChanged),
        this, &ConnectionSettingWidget::onProtocolChanged);

    if (ui->protocolListComboBox->count() > 0)
        onProtocolChanged(ui->protocolListComboBox->currentIndex());

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

	ui->protocolListComboBox->setStyleSheet(comboCss);



}

ConnectionSettingWidget::~ConnectionSettingWidget()
{
    delete ui;
}

void ConnectionSettingWidget::buildFactories()
{
    m_factories.insert(ConnectionProtocol::RS_232, [](QWidget* p) { return new RS232SettingsWidget(p); });
    m_factories.insert(ConnectionProtocol::RS_485, [](QWidget* p) { return new RS485SettingsWidget(p); });
    m_factories.insert(ConnectionProtocol::CAN, [](QWidget* p) { return new CanBusSettingsWidget(p); });
    m_factories.insert(ConnectionProtocol::MODBUS_RTU, [](QWidget* p) { return new ModBusRTUSettingsWidget(p); });
    m_factories.insert(ConnectionProtocol::MODBUS_TCP, [](QWidget* p) { return new ModBusTCPSettingsWidget(p); });
    m_factories.insert(ConnectionProtocol::LAN, [](QWidget* p) { return new EthernetSettingsWidget(p); });
    // TODO: WIFI/USB/ANALOG when ready
}

void ConnectionSettingWidget::populateProtocols()
{
    ui->protocolListComboBox->clear();
    ui->protocolListComboBox->setDuplicatesEnabled(false);

    const auto names = ConnectionProtocolUtils::allStrings(); // QVector<QString>
    for (const auto& name : names) {
        const auto proto = ConnectionProtocolUtils::parse(name);
        ui->protocolListComboBox->addItem(name, QVariant::fromValue(proto));
    }
}

void ConnectionSettingWidget::onProtocolChanged(int index)
{
    if (index < 0) return;

    const QVariant v = ui->protocolListComboBox->itemData(index);
    const auto proto = v.isValid()
        ? v.value<ConnectionProtocol>()
        : ConnectionProtocolUtils::parse(ui->protocolListComboBox->itemText(index));

    QWidget* w = nullptr;
    if (m_factories.contains(proto)) {
        w = m_factories.value(proto)(ui->protocolDetailPlaceholder);
    }
    else {
        auto* lbl = new QLabel(ui->protocolDetailPlaceholder);
        lbl->setAlignment(Qt::AlignCenter);
        lbl->setWordWrap(true);
        lbl->setText(ConnectionProtocolUtils::toString(proto)
            + " settings are not implemented yet.");
        w = lbl;
    }

    setDetailWidget(w);
}

QVBoxLayout* ConnectionSettingWidget::ensurePlaceholderLayout()
{
    auto* layout = qobject_cast<QVBoxLayout*>(ui->protocolDetailPlaceholder->layout());
    if (!layout) {
        layout = new QVBoxLayout(ui->protocolDetailPlaceholder);
        layout->setContentsMargins(8, 8, 8, 8);
        layout->setSpacing(8);
    }
    return layout;
}

void ConnectionSettingWidget::setDetailWidget(QWidget* w)
{
    auto* layout = ensurePlaceholderLayout();

    while (auto* item = layout->takeAt(0)) {
        if (auto* oldW = item->widget())
            oldW->deleteLater();
        delete item;
    }

    m_currentDetail = w;
    if (m_currentDetail)
        layout->addWidget(m_currentDetail);
}

void ConnectionSettingWidget::setDeviceContext(const QString& deviceType, int deviceId)
{
    m_deviceType = deviceType;
    m_deviceId = deviceId;

    // Optional: load existing values for this device
}

QJsonObject ConnectionSettingWidget::getParameters() const
{
    QJsonObject o;
    o["deviceType"] = m_deviceType;
    o["deviceId"] = m_deviceId;

    // Example: collect fields from the active detail widget & common controls
    // o["ip"]   = ui->ipLineEdit->text().trimmed();
    // o["port"] = ui->portSpinBox->value();
    // o["baud"] = ui->baudComboBox->currentText();
    // You can also query m_currentDetail if it exposes a method to serialize itself.

    return o;
}
