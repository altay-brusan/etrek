#include <QApplication>
#include "MainWindowDelegate.h"
#include "SystemSettingPageBuilder.h"

using namespace Etrek::Application::Delegate;

MainWindowDelegate::MainWindowDelegate(const DelegateParameter& params,MainWindow* widget, QObject *parent)
	: QObject(parent),
	  m_params(params),
      m_mainWindow(widget)
{

    Q_ASSERT(m_mainWindow);

    // Connect MainWindow's custom signals to delegate slots
    connect(m_mainWindow, &MainWindow::LoadSystemPageAction,
        this, &MainWindowDelegate::onLoadSystemPageAction);
    connect(m_mainWindow, &MainWindow::aboutToClose,
        this, &MainWindowDelegate::aboutToClose);


    // If you later add more:
    // connect(m_mainWindow, &MainWindow::LoadExamPage,   this, &MainWindowDelegate::onLoadExamPage);
    // connect(m_mainWindow, &MainWindow::LoadViewPage,   this, &MainWindowDelegate::onLoadViewPage);
    // connect(m_mainWindow, &MainWindow::LoadOutputPage, this, &MainWindowDelegate::onLoadOutputPage);

}

void MainWindowDelegate::onLoadSystemPageAction()
{

    m_mainWindow->prepareLoadingPage();

    SystemSettingPageBuilder builder;
    auto result = builder.build(m_params, nullptr, this);
    auto* page = result.first;
    m_systemSettingPageDelegate = result.second;

    connect(m_systemSettingPageDelegate, &SystemSettingPageDelegate::closeSettings,
        this, [page, this]()
        {
            m_mainWindow->closePage();			
        });


    m_mainWindow->loadPage(page);
	
    m_mainWindow->finishLoadingPage();    
}

void MainWindowDelegate::show()
{
    m_mainWindow->show();
}

MainWindowDelegate::~MainWindowDelegate()
{}

