#ifndef EXPOSUREAPPLICATIONCONTROLWIDGET_H
#define EXPOSUREAPPLICATIONCONTROLWIDGET_H

#include <QWidget>

namespace Ui {
class ExposureApplicationControlWidget;
}

class ExposureApplicationControlWidget : public QWidget
{
    Q_OBJECT

public:
    explicit ExposureApplicationControlWidget(QWidget *parent = nullptr);
    ~ExposureApplicationControlWidget();

private:
    Ui::ExposureApplicationControlWidget *ui;
};

#endif // EXPOSUREAPPLICATIONCONTROLWIDGET_H
