#ifndef CONNECTIONSETTINGWIDGET_H
#define CONNECTIONSETTINGWIDGET_H

#include <QWidget>
#include <QHash>
#include <functional>
#include <QJsonObject>
#include "ConnectionProtocol.h"

class QVBoxLayout;

namespace Ui { class ConnectionSettingWidget; }

class ConnectionSettingWidget : public QWidget
{
    Q_OBJECT
public:
    explicit ConnectionSettingWidget(QWidget* parent = nullptr);
    void setDeviceContext(const QString& deviceType, int deviceId);
    QJsonObject getParameters() const;
    ~ConnectionSettingWidget();

private slots:
    void onProtocolChanged(int index);

private:
    Ui::ConnectionSettingWidget* ui = nullptr;
    QWidget* m_currentDetail = nullptr;
    QString m_deviceType;
    int m_deviceId = 0;

    QHash<Etrek::Device::ConnectionProtocol, std::function<QWidget* (QWidget*)>> m_factories;

    void populateProtocols();
    void buildFactories();
    void setDetailWidget(QWidget* w);
    QVBoxLayout* ensurePlaceholderLayout();
};

#endif // CONNECTIONSETTINGWIDGET_H
