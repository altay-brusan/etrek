#include "DetectorConfigurationBuilder.h"
#include "DeviceRepository.h"
#include "Result.h"


using namespace Etrek::Device::Repository;
using namespace Etrek::Device::Data::Entity;

namespace Etrek::Device::Delegate
{
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
        auto repository = std::make_shared<DeviceRepository>(params.dbConnection);
		const auto nodes = repository->getDetectorList();
        auto widget = new DetectorConfigurationWidget(nodes.value, parentWidget);
        auto delegate = new DetectorConfigurationDelegate(widget, parentDelegate);

        // If you need to attach other delegates:
        // if (delegate) delegate->AttachDelegates(params.delegates.values());

        return { widget, delegate };
    }
}
