#ifndef CANBUSSETTINGSWIDGET_H
#define CANBUSSETTINGSWIDGET_H

#include <QWidget>

namespace Ui {
class CanBusSettingsWidget;
}

class CanBusSettingsWidget : public QWidget
{
    Q_OBJECT

public:
    explicit CanBusSettingsWidget(QWidget *parent = nullptr);
    ~CanBusSettingsWidget();

private:
    Ui::CanBusSettingsWidget *ui;
};

#endif // CANBUSSETTINGSWIDGET_H
