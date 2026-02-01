#include "ScanProtocolRepository.h"
#include "TechniqueConfigurationWidget.h"
#include "TechniqueConfigurationBuilder.h"
#include "TechniqueConfigurationDelegate.h"
#include "Result.h"


namespace Etrek::ScanProtocol::Delegate
{
    TechniqueConfigurationBuilder::TechniqueConfigurationBuilder()
    {
    }

    TechniqueConfigurationBuilder::~TechniqueConfigurationBuilder()
    {
    }

std::pair<TechniqueConfigurationWidget*, TechniqueConfigurationDelegate*>
TechniqueConfigurationBuilder::build(const DelegateParameter& params,
                                     QWidget* parentWidget,
                                     QObject* parentDelegate)
{
    // Builder creates repository from dbConnection
    auto repository = std::make_shared<Etrek::ScanProtocol::Repository::ScanProtocolRepository>(params.dbConnection, nullptr);
    
    auto techniqueParameters = repository->getAllTechniqueParameters();
    auto* widget   = new TechniqueConfigurationWidget(techniqueParameters.value, parentWidget);
    
    // Delegate receives repository because it needs to perform CRUD operations
    auto* delegate = new Etrek::ScanProtocol::Delegate::TechniqueConfigurationDelegate(widget, repository, params.contextManager, parentDelegate);

    return std::make_pair(widget, delegate); // MSVC: prefer make_pair
}

}
