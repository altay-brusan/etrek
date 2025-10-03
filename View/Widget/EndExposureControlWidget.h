#ifndef ENDEXPOSURECONTROLWIDGET_H
#define ENDEXPOSURECONTROLWIDGET_H

#include <QWidget>

namespace Ui {
class EndExposureControlWidget;
}

class EndExposureControlWidget : public QWidget
{
    Q_OBJECT

public:
    explicit EndExposureControlWidget(QWidget *parent = nullptr);
    ~EndExposureControlWidget();

private:
    Ui::EndExposureControlWidget *ui;
};

#endif // ENDEXPOSURECONTROLWIDGET_H
