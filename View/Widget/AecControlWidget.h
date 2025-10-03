#ifndef AECCONTROLWIDGET_H
#define AECCONTROLWIDGET_H

#include <QWidget>

namespace Ui {
class AecControlWidget;
}

class AecControlWidget : public QWidget
{
    Q_OBJECT

public:
    explicit AecControlWidget(QWidget *parent = nullptr);
    ~AecControlWidget();

private:
    Ui::AecControlWidget *ui;
};

#endif // AECCONTROLWIDGET_H
