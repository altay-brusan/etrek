#ifndef RS232SETTINGSWIDGET_H
#define RS232SETTINGSWIDGET_H

#include <QWidget>

namespace Ui {
class RS232SettingsWidget;
}

class RS232SettingsWidget : public QWidget
{
    Q_OBJECT

public:
    explicit RS232SettingsWidget(QWidget *parent = nullptr);
    ~RS232SettingsWidget();

private:
    Ui::RS232SettingsWidget *ui;
};

#endif // RS232SETTINGSWIDGET_H
