#ifndef CONNECTIONSETUPWIDGET_H
#define CONNECTIONSETUPWIDGET_H

#include <QWidget>
#include <QObject>
#include <QJsonObject>

class QDialog;                    // <— forward
namespace Ui { class ConnectionSetupWidget; }

class ConnectionSettingWidget;    // <— forward (no include here)

class ConnectionSetupWidget : public QWidget
{
    Q_OBJECT
public:
    explicit ConnectionSetupWidget(QWidget* parent = nullptr);
    ~ConnectionSetupWidget();

private slots:
    void onDeviceSettingsClicked();

private:
    void ensureSettingsDialog();  // creates dialog + settings widget once

    QDialog* m_settingsHostDialog = nullptr;     // created once, reused
    ConnectionSettingWidget* m_settingsWidget = nullptr;     // lives inside dialog
    Ui::ConnectionSetupWidget* ui = nullptr;

    void saveDeviceConnection(const QString& deviceType, int deviceId, const QJsonObject& params);
};

#endif // CONNECTIONSETUPWIDGET_H
