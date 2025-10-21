#ifndef MAINWINDOW_H
#define MAINWINDOW_H
#include <QCloseEvent>
#include <QMainWindow>
#include <QActionGroup>
#include <QSharedPointer>
#include <QVBoxLayout>


QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    
    MainWindow(QWidget *parent = nullptr);
    
    void prepareLoadingPage();
    void loadPage(QWidget* widget);
    void finishLoadingPage();
	void closePage();
        
    ~MainWindow();

signals:
	void LoadSystemPageAction();
	void LoadWorklistPageAction();
	void LoadExamPage();
	void LoadViewPage();
	void LoadOutputPage();
    void aboutToClose();

protected:
    void closeEvent(QCloseEvent* event) override {
        emit aboutToClose();   // Notify subscribers
        QMainWindow::closeEvent(event);
    }

private:

    void clearLoadedPage();
    void uncheckAllActions();
    void disableAllActions();
    void enableAllActions();
    void toggleAllActions();

    void setTitle(const QString& title);
    Ui::MainWindow *ui;

    QWidget* m_loadedPage = nullptr;
    QVBoxLayout* m_pageContainer = nullptr; // Layout for holding pages
    QList<QAction*> m_mainWindowActionList;
};
#endif // MAINWINDOW_H
