#ifndef EXAMPAGE_H
#define EXAMPAGE_H

#include <QWidget>

namespace Ui {
class ExamPage;
}

class ExamPage : public QWidget
{
    Q_OBJECT

public:
    explicit ExamPage(QWidget *parent = nullptr);
    ~ExamPage();

private:
    Ui::ExamPage *ui;
};

#endif // EXAMPAGE_H
