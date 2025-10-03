#ifndef ETREKSPLASHSCREEN_H
#define ETREKSPLASHSCREEN_H

#include <QWidget>

namespace Ui {
class EtrekSplashScreen;
}

class EtrekSplashScreen : public QWidget
{
    Q_OBJECT

public:
    explicit EtrekSplashScreen(QWidget *parent = nullptr);
    ~EtrekSplashScreen();

    void setMessage(const QString &message);
    void setTitle(const QString& title);
    void incrementProgress(int value); // 0–100

protected:
    void keyPressEvent(QKeyEvent *event) override;


private:
    Ui::EtrekSplashScreen *ui;

};

#endif // ETREKSPLASHSCREEN_H
