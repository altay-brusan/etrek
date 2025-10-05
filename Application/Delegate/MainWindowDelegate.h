#ifndef MAINWINDOWDELEGATE_H
#define MAINWINDOWDELEGATE_H

#include "DelegateParameter.h"
#include "MainWindow.h"
#include "SystemSettingPageDelegate.h"
#include <QObject>
#include <memory>

namespace Etrek::Application::Delegate {
class MainWindowDelegate : public QObject {
  Q_OBJECT

public:
  MainWindowDelegate(const DelegateParameter &params, MainWindow *widget,
                     QObject *parent);

  void show();

  ~MainWindowDelegate();

signals:
  void aboutToClose();

private slots:
  void onLoadSystemPageAction();

private:
  SystemSettingPageDelegate *m_systemSettingPageDelegate{nullptr};
  MainWindow *m_mainWindow{nullptr};
  DelegateParameter m_params;
};
} // namespace Etrek::Application::Delegate

#endif // MAINWINDOWDELEGATE_H
