#ifndef VIEWPAGE_H
#define VIEWPAGE_H

#include <QWidget>

namespace Ui {
class ViewPage;
}

class ViewPage : public QWidget
{
    Q_OBJECT

public:
    explicit ViewPage(QWidget *parent = nullptr);
    ~ViewPage();

private:
    Ui::ViewPage *ui;
};

#endif // VIEWPAGE_H
