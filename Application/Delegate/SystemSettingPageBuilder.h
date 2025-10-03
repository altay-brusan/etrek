#ifndef SYSTEMSETTINGPAGEBUILDER_H
#define SYSTEMSETTINGPAGEBUILDER_H

#include "IWidgetDelegateBuilder.h"
#include "SystemSettingPageDelegate.h"
#include "SystemSettingPage.h"
#include "DelegateParameter.h"
#include <memory>

namespace Etrek::Application::Delegate
{
    class SystemSettingPageBuilder :
        public IWidgetDelegateBuilder<SystemSettingPage, SystemSettingPageDelegate>
    {
    public:
        SystemSettingPageBuilder();
        ~SystemSettingPageBuilder();

        std::pair<SystemSettingPage*, SystemSettingPageDelegate*>
            build(const DelegateParameter& params,
                QWidget* parentWidget = nullptr,
                QObject* parentDelegate = nullptr) override;
    };
}

#endif // SYSTEMSETTINGPAGEBUILDER_H
