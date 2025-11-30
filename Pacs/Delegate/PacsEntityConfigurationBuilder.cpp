#include "PacsNodeRepository.h"
#include "PacsNode.h"
#include "PacsEntityConfigurationWidget.h"
#include "PacsEntityConfigurationBuilder.h"

namespace Etrek::Pacs::Delegate
{
    using Etrek::Pacs::Repository::PacsNodeRepository;

    PacsEntityConfigurationBuilder::PacsEntityConfigurationBuilder()
    {
    }

    PacsEntityConfigurationBuilder::~PacsEntityConfigurationBuilder()
    {
    }

    std::pair<PacsEntityConfigurationWidget*,PacsEntityConfigurationDelegate*>
        PacsEntityConfigurationBuilder::build(const DelegateParameter& params,
            QWidget* parentWidget,
            QObject* parentDelegate)
    {
        // Builder creates repository from dbConnection
        auto repository = std::make_shared<PacsNodeRepository>(params.dbConnection);
        
        const auto nodesResult = repository->getPacsNodes();
        auto widget = new PacsEntityConfigurationWidget(nodesResult, parentWidget);
        
        // Delegate receives repository if it needs to perform CRUD operations
        // In this case, delegate doesn't need repository (read-only widget)
        auto delegate = new PacsEntityConfigurationDelegate(widget, parentDelegate);

        // If you need to attach other delegates:
        // if (delegate) delegate->AttachDelegates(params.delegates.values());

        return { widget, delegate };
    }
}
