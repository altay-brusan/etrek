#ifndef STUDYCONTROLWIDGET_H
#define STUDYCONTROLWIDGET_H

#include <QWidget>

namespace Ui {
class StudyControlWidget;
}

class StudyControlWidget : public QWidget
{
    Q_OBJECT

public:
    explicit StudyControlWidget(QWidget *parent = nullptr);
    ~StudyControlWidget();

private:
    Ui::StudyControlWidget *ui;
};

#endif // STUDYCONTROLWIDGET_H
