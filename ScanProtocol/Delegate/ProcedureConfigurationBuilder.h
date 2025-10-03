#ifndef PROCEDURECONFIGURATIONBUILDER_H
#define PROCEDURECONFIGURATIONBUILDER_H

#include "IWidgetDelegateBuilder.h"
#include "ProcedureConfigurationDelegate.h"
#include "ProcedureConfigurationWidget.h"
#include "DelegateParameter.h"
#include <memory>

namespace Etrek::ScanProtocol::Delegate
{
    class ProcedureConfigurationBuilder :
        public IWidgetDelegateBuilder<ProcedureConfigurationWidget, ProcedureConfigurationDelegate>
    {
    public:
        ProcedureConfigurationBuilder();
        ~ProcedureConfigurationBuilder();

        std::pair<ProcedureConfigurationWidget*, ProcedureConfigurationDelegate*>
            build(const DelegateParameter& params,
                QWidget* parentWidget = nullptr,
                QObject* parentDelegate = nullptr) override;
    };
}
#endif // PROCEDURECONFIGURATIONBUILDER_H
