#include "GeneratorConfigurationBuilder.h"

namespace Etrek::Device::Delegate
{
    GeneratorConfigurationBuilder::GeneratorConfigurationBuilder()
    {
    }

    GeneratorConfigurationBuilder::~GeneratorConfigurationBuilder()
    {
    }

    std::pair<GeneratorConfigurationWidget*,GeneratorConfigurationDelegate*>
        GeneratorConfigurationBuilder::build(const DelegateParameter& params,
            QWidget* parentWidget,
            QObject* parentDelegate)
    {
		auto repository = std::make_shared<Etrek::Device::Repository::DeviceRepository>(
			params.dbConnection, nullptr);
        //auto generatorManufactures = repository->getManufacturersList(DeviceType::GENERATOR);
        //auto tubeManufactures = repository->getManufacturersList(DeviceType::TUBE);
		auto generators = repository->getGeneratorList();
		auto xRayTubes = repository->getXRayTubesList();
		//auto generatorTubeConnections = repository->getActiveGeneratorTubes();

        auto widget = new GeneratorConfigurationWidget( 
                                                       generators.value, 
                                                       
                                                       xRayTubes.value,
                                                       
                                                       parentWidget);
        auto delegate = new GeneratorConfigurationDelegate(widget, repository, parentDelegate);

        // If you need to attach other delegates:
        // if (delegate) delegate->AttachDelegates(params.delegates.values());

        return { widget, delegate };
    }
}
