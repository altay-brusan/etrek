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

    // Setter methods for patient demographics
    void setPatientName(const QString& name);
    void setPatientId(const QString& id);
    void setGender(const QString& gender);
    void setAge(int age);
    void setAccessionNumber(const QString& accessionNumber);

private:
    Ui::ExamTitleWidget *ui;
};

#endif // EXAMTITLEWIDGET_H
