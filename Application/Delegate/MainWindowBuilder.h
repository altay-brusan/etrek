#ifndef MAINWINDOWBUILDER_H
#define MAINWINDOWBUILDER_H

#include <memory>
#include "IWidgetDelegateBuilder.h"
#include "MainWindowDelegate.h"
#include "SystemSettingPageBuilder.h"
#include "DelegateParameter.h"
#include "MainWindow.h"


namespace Etrek::Application::Delegate
{

    class MainWindowBuilder :
        public IWidgetDelegateBuilder<MainWindow, MainWindowDelegate>
    {
    public:
        MainWindowBuilder();
        ~MainWindowBuilder();

        std::pair<MainWindow*, MainWindowDelegate*>
            build(const DelegateParameter& params,
                QWidget* parentWidget = nullptr,
                QObject* parentDelegate = nullptr) override;

    private:
        // Add any member variables if needed (e.g., configuration)
    };
}
#endif // MAINWINDOWBUILDER_H
