#include <QApplication>
#include <QDebug>

#include "MainWindowDelegate.h"
#include "SystemSettingPageBuilder.h"
#include "WorkListPageBuilder.h"
#include "ExamPageBuilder.h"

namespace Etrek::Application::Delegate
{

    MainWindowDelegate::MainWindowDelegate(const DelegateParameter &params,
                                           MainWindow *widget, QObject *parent)
        : QObject(parent), m_params(params), m_mainWindow(widget) {
      Q_ASSERT(m_mainWindow);

      // Connect MainWindow's custom signals to delegate slots
      connect(m_mainWindow, &MainWindow::LoadSystemPageAction, this,
              &MainWindowDelegate::onLoadSystemPageAction);
      connect(m_mainWindow, &MainWindow::LoadWorklistPageAction, this,
          &MainWindowDelegate::onLoadWorklistPageAction);

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

    void MainWindowDelegate::onLoadWorklistPageAction() {
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

        WorkListPageBuilder builder;
        auto result = builder.build(m_params, nullptr, this);
        auto* page = result.first;
        m_worklistPageDelegate = result.second;

        if (m_worklistPageDelegate) {
            connect(m_worklistPageDelegate,
                &WorkListPageDelegate::closeWorklist, this, [page, this]() {
                    if (m_mainWindow) {
                        m_mainWindow->closePage();
                    }
                });

            // Connect examination start signal
            connect(m_worklistPageDelegate,
                &WorkListPageDelegate::startExamination, this,
                &MainWindowDelegate::onStartExamination);
        }

        if (m_mainWindow) {
            m_mainWindow->loadPage(page);
            m_mainWindow->finishLoadingPage();
        }
    }

    void MainWindowDelegate::onStartExamination(int worklistEntryId) {
        if (!m_mainWindow) {
            qWarning() << "MainWindowDelegate::onStartExamination invoked without "
                "a MainWindow instance.";
            return;
        }

        qDebug() << "MainWindowDelegate: Starting examination for worklist entry" << worklistEntryId;

        m_mainWindow->prepareLoadingPage();

        // Clean up existing exam page delegate if any
        if (m_examPageDelegate) {
            m_examPageDelegate->deleteLater();
            m_examPageDelegate = nullptr;
        }

        // Build examination page
        ExamPageBuilder builder;
        auto result = builder.build(m_params, nullptr, this);
        auto* page = result.first;
        m_examPageDelegate = result.second;

        if (m_examPageDelegate) {
            // Connect close examination signal
            connect(m_examPageDelegate,
                &ExamPageDelegate::closeExamination, this, [page, this]() {
                    if (m_mainWindow) {
                        m_mainWindow->closePage();
                    }
                });

            // Connect examination completed signal
            connect(m_examPageDelegate,
                &ExamPageDelegate::examinationCompleted, this,
                [this](int studyId) {
                    qDebug() << "Examination completed, study ID:" << studyId;
                    // Optionally refresh worklist or perform other actions
                    if (m_mainWindow) {
                        m_mainWindow->closePage();
                    }
                });

            // Connect error signal
            connect(m_examPageDelegate,
                &ExamPageDelegate::errorOccurred, this,
                [](const QString& message) {
                    qWarning() << "Examination error:" << message;
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
