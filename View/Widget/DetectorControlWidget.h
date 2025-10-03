#ifndef DETECTORCONTROLWIDGET_H
#define DETECTORCONTROLWIDGET_H

#include <QWidget>

namespace Ui {
class DetectorControlWidget;
}

class DetectorControlWidget : public QWidget
{
    Q_OBJECT

public:
    explicit DetectorControlWidget(QWidget *parent = nullptr);
    ~DetectorControlWidget();

private:
    Ui::DetectorControlWidget *ui;
};

#endif // DETECTORCONTROLWIDGET_H
