#include "WorkflowConfigurationBuilder.h"
#include "DeviceRepository.h"
namespace Etrek::Device::Delegate
{   
    using Etrek::Device::Repository::DeviceRepository;

    WorkflowConfigurationBuilder::WorkflowConfigurationBuilder()
    {
    }

    WorkflowConfigurationBuilder::~WorkflowConfigurationBuilder()
    {
    }

    std::pair<WorkflowConfigurationWidget*, WorkflowConfigurationDelegate*>
        WorkflowConfigurationBuilder::build(const DelegateParameter& params,
            QWidget* parentWidget,
            QObject* parentDelegate)
    {
        auto repository = std::make_shared<DeviceRepository>(params.dbConnection);
        const auto equipments = repository->getGeneralEquipmentList();
        const auto institutes = repository->getInstitutionList();
        const auto envSettings = repository->getEnvironmentSettings();

        auto widget = new WorkflowConfigurationWidget(envSettings.value, equipments.value, institutes.value, parentWidget);
        auto delegate = new WorkflowConfigurationDelegate(widget, parentDelegate);

        // If you need to attach other delegates:
        // if (delegate) delegate->AttachDelegates(params.delegates.values());

        return { widget, delegate };
    }

}
