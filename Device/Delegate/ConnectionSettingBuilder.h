#ifndef CONNECTIONSETTINGBUILDER_H
#define CONNECTIONSETTINGBUILDER_H

#include "IWidgetDelegateBuilder.h"
#include "ConnectionSettingWidget.h"
#include "DatabaseConnectionSetting.h"
#include "DelegateParameter.h"
#include <memory>
#include <QObject>

namespace Etrek::Device::Delegate
{
    class ConnectionSettingBuilder :
        public IWidgetDelegateBuilder<ConnectionSettingWidget, QObject>
    {
    public:
        ConnectionSettingBuilder();
        ~ConnectionSettingBuilder();

        std::pair<ConnectionSettingWidget*, QObject*>
            build(const DelegateParameter& params,
                QWidget* parentWidget = nullptr,
                QObject* parentDelegate = nullptr) override;
    };
}

#endif // CONNECTIONSETUPBUILDER_H
