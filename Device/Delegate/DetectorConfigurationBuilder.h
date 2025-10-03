#ifndef DETECTORCONFIGURATIONBUILDER_H
#define DETECTORCONFIGURATIONBUILDER_H

#include "IWidgetDelegateBuilder.h"
#include "DetectorConfigurationDelegate.h"
#include "DetectorConfigurationWidget.h"
#include "DelegateParameter.h"
#include <memory>

namespace Etrek::Device::Delegate
{
    class DetectorConfigurationBuilder :
		public IWidgetDelegateBuilder<DetectorConfigurationWidget, DetectorConfigurationDelegate>
    {
    public:
        DetectorConfigurationBuilder();
        ~DetectorConfigurationBuilder();

        std::pair<DetectorConfigurationWidget*, DetectorConfigurationDelegate*>
            build(const DelegateParameter& params,
                QWidget* parentWidget = nullptr,
                QObject* parentDelegate = nullptr) override;
    };
}

#endif // DETECTORCONFIGURATIONBUILDER_H
