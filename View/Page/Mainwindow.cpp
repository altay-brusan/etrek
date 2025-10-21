#include "Mainwindow.h"

#include <QActionGroup>
#include <QApplication>
#include "Mainwindow.h"
#include "ExamPage.h"
#include "OutputPage.h"
#include "SystemSettingPage.h"
#include "ViewPage.h"
#include "ui_Mainwindow.h"

MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent),
      ui(new Ui::MainWindow)
{
    ui->setupUi(this);
	m_pageContainer = new QVBoxLayout(parent);
    this->centralWidget()->setLayout(m_pageContainer);

    QActionGroup* toolbarGroup = new QActionGroup(this);
    toolbarGroup->setExclusive(true);

    toolbarGroup->addAction(ui->actionLoadSystemPage);
    toolbarGroup->addAction(ui->actionLoadWorklist);
    toolbarGroup->addAction(ui->actionLoadExamPage);
    toolbarGroup->addAction(ui->actionLoadViewPage);
    toolbarGroup->addAction(ui->actionLoadOutputPage);

    // Collect all the relevant actions
    m_mainWindowActionList = {
        ui->actionLoadSystemPage,
		ui->actionLoadWorklist,
        ui->actionLoadExamPage,
        ui->actionLoadViewPage,
        ui->actionLoadOutputPage
    };

    connect(ui->actionLoadSystemPage, &QAction::triggered, this, [this]() {
        emit LoadSystemPageAction();
        });
    connect(ui->actionLoadWorklist, &QAction::triggered, this, [this]() {
        emit LoadWorklistPageAction();
        });
    connect(ui->actionLoadExamPage, &QAction::triggered, this, [this]() {
        emit LoadExamPage();
        });
    connect(ui->actionLoadViewPage, &QAction::triggered, this, [this]() {
        emit LoadViewPage();
        });
    connect(ui->actionLoadOutputPage, &QAction::triggered, this, [this]() {
        emit LoadOutputPage();
        });
}

void MainWindow::prepareLoadingPage()
{
    clearLoadedPage();
    QApplication::setOverrideCursor(Qt::WaitCursor);  // show busy cursor
    disableAllActions();
}

void MainWindow::loadPage(QWidget* widget)
{
    // 1 Remove old page if exists
    clearLoadedPage();

    // 2️ Add new page to layout
    m_pageContainer->addWidget(widget);
    m_loadedPage = widget;
    widget->show();
}

void MainWindow::finishLoadingPage()
{
    QApplication::restoreOverrideCursor();
}

void MainWindow::closePage()
{
	if (m_loadedPage) {
		m_loadedPage->close();
		clearLoadedPage();
	}
	enableAllActions();
	uncheckAllActions();
}

void MainWindow::clearLoadedPage()
{
    if (m_loadedPage) {
        m_loadedPage->hide();
        m_pageContainer->removeWidget(m_loadedPage);
        m_loadedPage->deleteLater();
        m_loadedPage = nullptr;
    }
}

void MainWindow::uncheckAllActions()
{
    for (QAction* a : m_mainWindowActionList) {
        a->setChecked(false);
    }
}

void MainWindow::disableAllActions()
{
    for (QAction* a : m_mainWindowActionList) {
        a->setEnabled(false);
    }
}

void MainWindow::enableAllActions()
{
    for (QAction* a : m_mainWindowActionList) {
        a->setEnabled(true);
    }
}

void MainWindow::toggleAllActions()
{

    // Check if all are currently enabled
    bool allEnabled = std::all_of(m_mainWindowActionList.begin(), m_mainWindowActionList.end(),
        [](QAction* a) { return a->isEnabled(); });

    // Toggle: if all enabled -> disable all, otherwise -> enable all
    for (QAction* a : m_mainWindowActionList) {
        a->setEnabled(!allEnabled);
    }
}

void MainWindow::setTitle(const QString& title)
{
}

MainWindow::~MainWindow()
{
	delete ui;
    // Clean up pages that are not owned by Qt
    // Delete unused pages only
    if (m_loadedPage )
        delete m_loadedPage;

}