#ifndef BODYSIZEWIDGET_H
#define BODYSIZEWIDGET_H

#include <QWidget>

namespace Ui {
class BodySizeWidget;
}

class BodySizeWidget : public QWidget
{
    Q_OBJECT

public:
    explicit BodySizeWidget(QWidget *parent = nullptr);
    ~BodySizeWidget();

private:
    Ui::BodySizeWidget *ui;
};

#endif // BODYSIZEWIDGET_H
