#ifndef RS485SETTINGSWIDGET_H
#define RS485SETTINGSWIDGET_H

#include <QWidget>

namespace Ui {
class RS485SettingsWidget;
}

class RS485SettingsWidget : public QWidget
{
    Q_OBJECT

public:
    explicit RS485SettingsWidget(QWidget *parent = nullptr);
    ~RS485SettingsWidget();

private:
    Ui::RS485SettingsWidget *ui;
};

#endif // RS485SETTINGSWIDGET_H
