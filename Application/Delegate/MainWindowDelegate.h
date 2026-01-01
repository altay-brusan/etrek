#ifndef MAINWINDOWDELEGATE_H
#define MAINWINDOWDELEGATE_H
#include <QObject>
#include <memory>

#include "DelegateParameter.h"
#include "MainWindow.h"
#include "SystemSettingPageDelegate.h"
#include "WorkListPageDelegate.h"
#include "ExamPageDelegate.h"
#include "ImageViewerPageDelegate.h"
#include "ImageViewerTypes.h"


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
    void onLoadWorklistPageAction();
    void onLoadExamPageAction();
    void onLoadViewPageAction();
    void onStartExamination(int worklistEntryId);
    void onOpenImageViewer(const QByteArray& imageData, const Etrek::ImageViewer::DicomMetadata& metadata);

private:
    SystemSettingPageDelegate* m_systemSettingPageDelegate{ nullptr };
    WorkListPageDelegate *m_worklistPageDelegate{nullptr};
    ExamPageDelegate *m_examPageDelegate{nullptr};
    ImageViewerPageDelegate *m_imageViewerPageDelegate{nullptr};
  MainWindow *m_mainWindow{nullptr};
  DelegateParameter m_params;
};
} // namespace Etrek::Application::Delegate

#endif // MAINWINDOWDELEGATE_H
