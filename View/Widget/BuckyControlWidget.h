#ifndef BUCKYCONTROLWIDGET_H
#define BUCKYCONTROLWIDGET_H

#include <QWidget>

namespace Ui {
class BuckyControlWidget;
}

class BuckyControlWidget : public QWidget
{
    Q_OBJECT

public:
    explicit BuckyControlWidget(QWidget *parent = nullptr);
    ~BuckyControlWidget();

private:
    Ui::BuckyControlWidget *ui;
};

#endif // BUCKYCONTROLWIDGET_H
