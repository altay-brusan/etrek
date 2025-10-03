#ifndef COLLIMATORCONFIGURATIONWIDGET_H
#define COLLIMATORCONFIGURATIONWIDGET_H

#include <QWidget>

namespace Ui {
class CollimatorConfigurationWidget;
}

class CollimatorConfigurationWidget : public QWidget
{
    Q_OBJECT

public:
    explicit CollimatorConfigurationWidget(QWidget *parent = nullptr);
    ~CollimatorConfigurationWidget();

private:
    Ui::CollimatorConfigurationWidget *ui;
};

#endif // COLLIMATORCONFIGURATIONWIDGET_H
