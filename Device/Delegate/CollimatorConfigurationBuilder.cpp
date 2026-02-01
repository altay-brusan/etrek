#include "CollimatorConfigurationBuilder.h"
#include "DeviceRepository.h"

namespace Etrek::Device::Delegate
{
    using Etrek::Device::Repository::DeviceRepository;

    CollimatorConfigurationBuilder::CollimatorConfigurationBuilder()
    {
    }

    CollimatorConfigurationBuilder::~CollimatorConfigurationBuilder()
    {
    }

    std::pair<CollimatorConfigurationWidget*, CollimatorConfigurationDelegate*>
        CollimatorConfigurationBuilder::build(const DelegateParameter& params,
            QWidget* parentWidget,
            QObject* parentDelegate)
    {
        // Builder creates repository from dbConnection
        auto repository = std::make_shared<DeviceRepository>(params.dbConnection);

        auto widget = new CollimatorConfigurationWidget(parentWidget);

        // Delegate receives repository to perform CRUD operations on apply/accept
        auto delegate = new CollimatorConfigurationDelegate(widget, repository, params.contextManager, parentDelegate);

        return { widget, delegate };
    }
}
