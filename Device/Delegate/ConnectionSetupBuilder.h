#ifndef CONNECTIONSETUPBUILDER_H
#define CONNECTIONSETUPBUILDER_H

#include "IWidgetDelegateBuilder.h"
#include "ConnectionSetupDelegate.h"
#include "ConnectionSetupWidget.h"
#include "DatabaseConnectionSetting.h"
#include "DelegateParameter.h"
#include "ConnectionSettingBuilder.h"
#include <memory>

namespace Etrek::Device::Delegate
{
    class ConnectionSetupBuilder :
        public IWidgetDelegateBuilder<ConnectionSetupWidget, ConnectionSetupDelegate>
    {
    public:
        ConnectionSetupBuilder();
        ~ConnectionSetupBuilder();

        std::pair<ConnectionSetupWidget*, ConnectionSetupDelegate*>
            build(const DelegateParameter& params,
                QWidget* parentWidget = nullptr,
                QObject* parentDelegate = nullptr) override;      
    };
}

#endif // CONNECTIONSETUPBUILDER_H
