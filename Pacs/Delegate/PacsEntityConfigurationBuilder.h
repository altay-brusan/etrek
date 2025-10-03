#ifndef PACSENTITYCONFIGURATIONBUILDER_H
#define PACSENTITYCONFIGURATIONBUILDER_H

#include "IWidgetDelegateBuilder.h"
#include "PacsEntityConfigurationDelegate.h"
#include "PacsEntityConfigurationWidget.h"
#include "DelegateParameter.h"

#include <memory>

class PacsNodeRepository;

namespace Etrek::Pacs::Delegate
{
    class PacsEntityConfigurationBuilder :
        public IWidgetDelegateBuilder<PacsEntityConfigurationWidget, PacsEntityConfigurationDelegate>
    {
    public:
        PacsEntityConfigurationBuilder();
        ~PacsEntityConfigurationBuilder();

        std::pair<PacsEntityConfigurationWidget*, PacsEntityConfigurationDelegate*>
            build(const DelegateParameter& params,
                QWidget* parentWidget = nullptr,
                QObject* parentDelegate = nullptr) override;

    };
}
#endif // PACSENTITYCONFIGURATIONBUILDER_H
