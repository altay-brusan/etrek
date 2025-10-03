#ifndef WORKLISTCONFIGURATIONBUILDER_H
#define WORKLISTCONFIGURATIONBUILDER_H

#include "IWidgetDelegateBuilder.h"
#include "WorkListConfigurationDelegate.h"
#include "WorkListConfigurationWidget.h"
#include "DelegateParameter.h"
#include <memory>

namespace Etrek::Worklist::Delegate {

    class WorkListConfigurationBuilder
        : public IWidgetDelegateBuilder<WorkListConfigurationWidget, WorkListConfigurationDelegate>
    {
    public:
        WorkListConfigurationBuilder();
        ~WorkListConfigurationBuilder();

        std::pair<WorkListConfigurationWidget*,WorkListConfigurationDelegate*>
            build(const DelegateParameter& params,
                QWidget* parentWidget = nullptr,
                QObject* parentDelegate = nullptr) override;
    };
}
#endif // WORKLISTCONFIGURATIONBUILDER_H
