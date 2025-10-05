#include "MainWindowDelegate.h"
#include "SystemSettingPageBuilder.h"
#include <QApplication>
#include <QDebug>

namespace Etrek::Application::Delegate {
MainWindowDelegate::MainWindowDelegate(const DelegateParameter &params,
                                       MainWindow *widget, QObject *parent)
    : QObject(parent), m_params(params), m_mainWindow(widget) {
  Q_ASSERT(m_mainWindow);

  // Connect MainWindow's custom signals to delegate slots
  connect(m_mainWindow, &MainWindow::LoadSystemPageAction, this,
          &MainWindowDelegate::onLoadSystemPageAction);
  connect(m_mainWindow, &MainWindow::aboutToClose, this,
          &MainWindowDelegate::aboutToClose);
  connect(m_mainWindow, &QObject::destroyed, this,
          [this]() { m_mainWindow = nullptr; });

  // If you later add more:
  // connect(m_mainWindow, &MainWindow::LoadExamPage,   this,
  // &MainWindowDelegate::onLoadExamPage); connect(m_mainWindow,
  // &MainWindow::LoadViewPage,   this, &MainWindowDelegate::onLoadViewPage);
  // connect(m_mainWindow, &MainWindow::LoadOutputPage, this,
  // &MainWindowDelegate::onLoadOutputPage);
}

void MainWindowDelegate::onLoadSystemPageAction() {
  if (!m_mainWindow) {
    qWarning() << "MainWindowDelegate::onLoadSystemPageAction invoked without "
                  "a MainWindow instance.";
    return;
  }

  m_mainWindow->prepareLoadingPage();

  if (m_systemSettingPageDelegate) {
    m_systemSettingPageDelegate->deleteLater();
    m_systemSettingPageDelegate = nullptr;
  }

  SystemSettingPageBuilder builder;
  auto result = builder.build(m_params, nullptr, this);
  auto *page = result.first;
  m_systemSettingPageDelegate = result.second;

  if (m_systemSettingPageDelegate) {
    connect(m_systemSettingPageDelegate,
            &SystemSettingPageDelegate::closeSettings, this, [page, this]() {
              if (m_mainWindow) {
                m_mainWindow->closePage();
              }
            });
  }

  if (m_mainWindow) {
    m_mainWindow->loadPage(page);
    m_mainWindow->finishLoadingPage();
  }
}

void MainWindowDelegate::show() {
  if (!m_mainWindow) {
    qWarning()
        << "MainWindowDelegate::show called without a MainWindow instance.";
    return;
  }

  m_mainWindow->show();
}

MainWindowDelegate::~MainWindowDelegate() = default;
} // namespace Etrek::Application::Delegate
