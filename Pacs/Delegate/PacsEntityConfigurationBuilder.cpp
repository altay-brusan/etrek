#include "PacsNodeRepository.h"
#include "PacsEntityConfigurationBuilder.h"

namespace Etrek::Pacs::Delegate
{
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
		auto repository = std::make_shared<PacsNodeRepository>(params.dbConnection);
		const auto nodesResult = repository->getPacsNodes();
        auto widget = new PacsEntityConfigurationWidget(nodesResult, parentWidget);
        auto delegate = new PacsEntityConfigurationDelegate(widget, parentDelegate);

        // If you need to attach other delegates:
        // if (delegate) delegate->AttachDelegates(params.delegates.values());

        return { widget, delegate };
    }
}
