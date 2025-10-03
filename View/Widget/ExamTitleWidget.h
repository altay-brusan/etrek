#ifndef EXAMTITLEWIDGET_H
#define EXAMTITLEWIDGET_H

#include <QWidget>

namespace Ui {
class ExamTitleWidget;
}

class ExamTitleWidget : public QWidget
{
    Q_OBJECT

public:
    explicit ExamTitleWidget(QWidget *parent = nullptr);
    ~ExamTitleWidget();

private:
    Ui::ExamTitleWidget *ui;
};

#endif // EXAMTITLEWIDGET_H
