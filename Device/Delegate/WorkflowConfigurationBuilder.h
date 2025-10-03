#ifndef WORKFLOWCONFIGURATIONBUILDER_H
#define WORKFLOWCONFIGURATIONBUILDER_H

#include "IWidgetDelegateBuilder.h"
#include "WorkflowConfigurationDelegate.h"
#include "WorkflowConfigurationWidget.h"
#include "DelegateParameter.h"
#include <memory>

class WorkflowConfigurationBuilder :
    public IWidgetDelegateBuilder<WorkflowConfigurationWidget, WorkflowConfigurationDelegate>
{
public:
    WorkflowConfigurationBuilder();
    ~WorkflowConfigurationBuilder();

    std::pair<WorkflowConfigurationWidget*, WorkflowConfigurationDelegate*>
        build(const DelegateParameter& params,
            QWidget* parentWidget = nullptr,
            QObject* parentDelegate = nullptr) override;
};

#endif // WORKFLOWCONFIGURATIONBUILDER_H
