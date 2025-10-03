#ifndef MODBUSRTUSETTINGSWIDGET_H
#define MODBUSRTUSETTINGSWIDGET_H

#include <QWidget>

namespace Ui {
class ModBusRTUSettingsWidget;
}

class ModBusRTUSettingsWidget : public QWidget
{
    Q_OBJECT

public:
    explicit ModBusRTUSettingsWidget(QWidget *parent = nullptr);
    ~ModBusRTUSettingsWidget();

private:
    Ui::ModBusRTUSettingsWidget *ui;
};

#endif // MODBUSRTUSETTINGSWIDGET_H
