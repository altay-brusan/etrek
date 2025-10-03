#include "EtrekSplashScreen.h"
#include "EtrekSplashScreen.h"
#include "ui_EtrekSplashScreen.h"
#include <QKeyEvent>

EtrekSplashScreen::EtrekSplashScreen(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::EtrekSplashScreen)
{

        ui->setupUi(this);

    // Set fixed size (adjust width/height as needed)
     setFixedSize(600, 350);

    // Remove title bar, close/minimize/maximize buttons
     setWindowFlags(Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint);

    // Optional: make background transparent if you want custom shape
    setAttribute(Qt::WA_TranslucentBackground);

    // Disable resizing (already handled by setFixedSize)
     setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);

    // Init UI state
    ui->progressBar->setValue(10);
    ui->messageLbl->setText("Initializing...");


}

void EtrekSplashScreen::keyPressEvent(QKeyEvent *event)
{
    if (event->key() == Qt::Key_Escape) {
        event->ignore(); // prevent closing
    } else {
        QWidget::keyPressEvent(event);
    }
}


EtrekSplashScreen::~EtrekSplashScreen()
{
    delete ui;
}

void EtrekSplashScreen::setMessage(const QString &message)
{
    ui->messageLbl->setText(message);
    qApp->processEvents(); // Ensure immediate update
}

void EtrekSplashScreen::setTitle(const QString& title)
{
    // TODO: if required added title bar to progress window
}

void EtrekSplashScreen::incrementProgress(int value)
{
    int current = ui->progressBar->value();
    int maximum = ui->progressBar->maximum();
    int newValue = current + value;
    if (newValue > maximum)
        newValue = maximum;
    if (newValue < ui->progressBar->minimum())
        newValue = ui->progressBar->minimum();
    ui->progressBar->setValue(newValue);
    qApp->processEvents();
}
