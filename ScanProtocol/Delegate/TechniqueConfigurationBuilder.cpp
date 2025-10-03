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
    auto repo = std::make_shared<Etrek::ScanProtocol::Repository::ScanProtocolRepository>(params.dbConnection, nullptr);
    
    auto techniqueParameters = repo->getAllTechniqueParameters();
    auto* widget   = new TechniqueConfigurationWidget(techniqueParameters.value, parentWidget);
    auto* delegate = new Etrek::ScanProtocol::Delegate::TechniqueConfigurationDelegate(widget, repo, parentDelegate);

    return std::make_pair(widget, delegate); // MSVC: prefer make_pair
}

}
