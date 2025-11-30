#include "MainWindowBuilder.h"

namespace Etrek::Application::Delegate
{

    MainWindowBuilder::MainWindowBuilder()
    {
    }

    MainWindowBuilder::~MainWindowBuilder()
    {
    }

    std::pair<MainWindow*, MainWindowDelegate*>
        MainWindowBuilder::build(const DelegateParameter& params,
            QWidget* parentWidget,
            QObject* parentDelegate)
    {
        auto mainWindow = new MainWindow(parentWidget);

        // Build pages inside the MainWindow
        //SystemSettingPageBuilder systemSettingPageBuilder;
        //auto result = systemSettingPageBuilder.build(params, nullptr, parentDelegate);
        //auto* systemSettingPage = result.first;
        //auto& systemSettingPageDelegate = result.second;

        //mainWindow->setSystemSettingPage(systemSettingPage);

        auto delegate = new MainWindowDelegate(
            params,
            mainWindow,
            parentDelegate
        );

        return { mainWindow, delegate };
    }
}