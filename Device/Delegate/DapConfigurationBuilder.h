#ifndef DAPCONFIGURATIONBUILDER_H
#define DAPCONFIGURATIONBUILDER_H

#include "IWidgetDelegateBuilder.h"
#include "DapConfigurationDelegate.h"
#include "DapConfigurationWidget.h"
#include "DelegateParameter.h"
#include <memory>

namespace Etrek::Device::Delegate
{
    class DapConfigurationBuilder :
        public IWidgetDelegateBuilder<DapConfigurationWidget, DapConfigurationDelegate>
    {
    public:
        DapConfigurationBuilder();
        ~DapConfigurationBuilder();

        std::pair<DapConfigurationWidget*, DapConfigurationDelegate*>
            build(const DelegateParameter& params,
                QWidget* parentWidget = nullptr,
                QObject* parentDelegate = nullptr) override;

    };
}

#endif // DAPCONFIGURATIONBUILDER_H
