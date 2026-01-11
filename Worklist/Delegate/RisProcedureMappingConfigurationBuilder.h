#ifndef RISPROCEDUREMAPPINGCONFIGURATIONBUILDER_H
#define RISPROCEDUREMAPPINGCONFIGURATIONBUILDER_H

#include "IWidgetDelegateBuilder.h"
#include "RisProcedureMappingConfigurationDelegate.h"
#include "RisProcedureMappingConfigurationWidget.h"
#include "DelegateParameter.h"
#include <memory>

namespace Etrek::Worklist::Delegate
{
    class RisProcedureMappingConfigurationBuilder :
        public IWidgetDelegateBuilder<RisProcedureMappingConfigurationWidget, RisProcedureMappingConfigurationDelegate>
    {
    public:
        RisProcedureMappingConfigurationBuilder();
        ~RisProcedureMappingConfigurationBuilder();

        std::pair<RisProcedureMappingConfigurationWidget*, RisProcedureMappingConfigurationDelegate*>
            build(const DelegateParameter& params,
                QWidget* parentWidget = nullptr,
                QObject* parentDelegate = nullptr) override;
    };
}

#endif // RISPROCEDUREMAPPINGCONFIGURATIONBUILDER_H
