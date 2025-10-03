#ifndef EXPOSURECONTROLWIDGET_H
#define EXPOSURECONTROLWIDGET_H

#include <QWidget>

namespace Ui {
class ExposureControlWidget;
}

class ExposureControlWidget : public QWidget
{
    Q_OBJECT

public:
    explicit ExposureControlWidget(QWidget *parent = nullptr);
    ~ExposureControlWidget();

private:
    Ui::ExposureControlWidget *ui;
};

#endif // EXPOSURECONTROLWIDGET_H
