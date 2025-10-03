#ifndef DAPCONFIGURATIONWIDGET_H
#define DAPCONFIGURATIONWIDGET_H

#include <QWidget>

namespace Ui {
class DapConfigurationWidget;
}

class DapConfigurationWidget : public QWidget
{
    Q_OBJECT

public:
    explicit DapConfigurationWidget(QWidget *parent = nullptr);
    ~DapConfigurationWidget();

private:
    Ui::DapConfigurationWidget *ui;
};

#endif // DAPCONFIGURATIONWIDGET_H
