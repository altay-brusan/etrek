#ifndef GENERATORCONFIGURATIONBUILDER_H
#define GENERATORCONFIGURATIONBUILDER_H

#include "IWidgetDelegateBuilder.h"
#include "GeneratorConfigurationDelegate.h"
#include "GeneratorConfigurationWidget.h"
#include "DelegateParameter.h"
#include "DeviceRepository.h"
#include <memory>

namespace Etrek::Device::Delegate
{
    class GeneratorConfigurationBuilder :
        public IWidgetDelegateBuilder<GeneratorConfigurationWidget, GeneratorConfigurationDelegate>
    {
    public:
        GeneratorConfigurationBuilder();
        ~GeneratorConfigurationBuilder();

        std::pair<GeneratorConfigurationWidget*, GeneratorConfigurationDelegate*>
            build(const DelegateParameter& params,
                QWidget* parentWidget = nullptr,
                QObject* parentDelegate = nullptr) override;
    };
}

#endif // GENERATORCONFIGURATIONBUILDER_H
