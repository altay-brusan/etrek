#include "DetectorConfigurationBuilder.h"
#include "DeviceRepository.h"
#include "Result.h"


namespace Etrek::Device::Delegate
{
    using Etrek::Device::Repository::DeviceRepository;

    DetectorConfigurationBuilder::DetectorConfigurationBuilder()
    {
    }

    DetectorConfigurationBuilder::~DetectorConfigurationBuilder()
    {
    }

    std::pair<DetectorConfigurationWidget*, DetectorConfigurationDelegate*>
        DetectorConfigurationBuilder::build(const DelegateParameter& params,
            QWidget* parentWidget,
            QObject* parentDelegate)
    {
        // Builder creates repository from dbConnection
        auto repository = std::make_shared<DeviceRepository>(params.dbConnection);
        
        const auto nodes = repository->getDetectorList();
        auto widget = new DetectorConfigurationWidget(nodes.value, parentWidget);
        
        // Delegate receives repository to perform CRUD operations on apply/accept
        auto delegate = new DetectorConfigurationDelegate(widget, repository, params.contextManager, parentDelegate);

        // If you need to attach other delegates:
        // if (delegate) delegate->AttachDelegates(params.delegates.values());

        return { widget, delegate };
    }
}
