#ifndef TECHNIQUECONFIGURATIONBUILDER_H
#define TECHNIQUECONFIGURATIONBUILDER_H

#include "IWidgetDelegateBuilder.h"
#include "TechniqueConfigurationDelegate.h"
#include "TechniqueConfigurationWidget.h"
#include "DelegateParameter.h"
#include <memory>

namespace Etrek::ScanProtocol::Delegate
{
    class TechniqueConfigurationBuilder :
        public IWidgetDelegateBuilder<TechniqueConfigurationWidget, TechniqueConfigurationDelegate>

    {
    public:
        TechniqueConfigurationBuilder();
        ~TechniqueConfigurationBuilder();

        std::pair<TechniqueConfigurationWidget*, TechniqueConfigurationDelegate*>
            build(const DelegateParameter& params,
                QWidget* parentWidget = nullptr,
                QObject* parentDelegate = nullptr) override;
    };
}

#endif // TECHNIQUECONFIGURATIONBUILDER_H
