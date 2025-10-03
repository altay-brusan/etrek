#ifndef GENERATORCONTROLWIDGET_H
#define GENERATORCONTROLWIDGET_H

#include <QWidget>

namespace Ui {
class GeneratorControlWidget;
}

class GeneratorControlWidget : public QWidget
{
    Q_OBJECT

public:
    explicit GeneratorControlWidget(QWidget *parent = nullptr);
    ~GeneratorControlWidget();

private:
    Ui::GeneratorControlWidget *ui;
};

#endif // GENERATORCONTROLWIDGET_H
