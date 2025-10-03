#ifndef ETHERNETSETTINGSWIDGET_H
#define ETHERNETSETTINGSWIDGET_H

#include <QWidget>

namespace Ui {
class EthernetSettingsWidget;
}

class EthernetSettingsWidget : public QWidget
{
    Q_OBJECT
protected:
    bool eventFilter(QObject* obj, QEvent* ev) override;
public:
    explicit EthernetSettingsWidget(QWidget *parent = nullptr);
    ~EthernetSettingsWidget();

private:
    Ui::EthernetSettingsWidget *ui;
    void setInvalid(QWidget* w, bool invalid);
};

#endif // ETHERNETSETTINGSWIDGET_H
