#ifndef MODEBUSTCPSETTINGSWIDGET_H
#define MODEBUSTCPSETTINGSWIDGET_H

#include <QWidget>

namespace Ui {
class ModBusTCPSettingsWidget;
}

class ModBusTCPSettingsWidget : public QWidget
{
    Q_OBJECT

public:
    explicit ModBusTCPSettingsWidget(QWidget *parent = nullptr);
    ~ModBusTCPSettingsWidget();
protected:
    bool eventFilter(QObject* obj, QEvent* ev) override;
private:
    void setInvalid(QWidget* w, bool invalid);
    Ui::ModBusTCPSettingsWidget *ui;
};

#endif // MODEBUSTCPSETTINGSWIDGET_H
