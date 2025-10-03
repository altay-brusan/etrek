#ifndef FOCALSPOTCONTROLWIDGET_H
#define FOCALSPOTCONTROLWIDGET_H

#include <QWidget>

namespace Ui {
class FocalPointControlWidget;
}

class FocalPointControlWidget : public QWidget
{
    Q_OBJECT

public:
    explicit FocalPointControlWidget(QWidget *parent = nullptr);
    ~FocalPointControlWidget();

private:
    Ui::FocalPointControlWidget *ui;
};

#endif // FOCALSPOTCONTROLWIDGET_H
