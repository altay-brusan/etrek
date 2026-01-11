#include "DapConfigurationBuilder.h"
#include "DeviceRepository.h"

namespace Etrek::Device::Delegate
{
    using Etrek::Device::Repository::DeviceRepository;

    DapConfigurationBuilder::DapConfigurationBuilder()
    {
    }

    DapConfigurationBuilder::~DapConfigurationBuilder()
    {
    }

    std::pair<DapConfigurationWidget*, DapConfigurationDelegate*>
        DapConfigurationBuilder::build(const DelegateParameter& params,
            QWidget* parentWidget,
            QObject* parentDelegate)
    {
        // Builder creates repository from dbConnection
        auto repository = std::make_shared<DeviceRepository>(params.dbConnection);

        auto widget = new DapConfigurationWidget(parentWidget);

        // Delegate receives repository to perform CRUD operations on apply/accept
        auto delegate = new DapConfigurationDelegate(widget, repository, parentDelegate);

        return { widget, delegate };
    }
}
