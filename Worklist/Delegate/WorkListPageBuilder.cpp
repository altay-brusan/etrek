#include "WorkListPageBuilder.h"
#include "ScanProtocolRepository.h"

namespace Etrek::Worklist::Delegate {
	using namespace Etrek::Worklist::Repository;
	WorkListPageBuilder::WorkListPageBuilder()
	{
	}

	WorkListPageBuilder::~WorkListPageBuilder()
	{
	}

    std::pair<WorkListPage*, WorkListPageDelegate*>
        WorkListPageBuilder::build(const DelegateParameter& params,
            QWidget* parentWidget,
            QObject* parentDelegate)
    {
        auto repository = std::make_shared<WorklistRepository>(params.dbConnection);
        auto scanRepository = std::make_shared<Etrek::ScanProtocol::Repository::ScanProtocolRepository>(params.dbConnection);
        std::shared_ptr<IWorklistRepository> irepository = std::static_pointer_cast<IWorklistRepository>(repository);
        auto* widget = new WorkListPage(irepository, parentWidget);

        auto* delegate = new WorkListPageDelegate(widget, repository, scanRepository, parentDelegate);

        // If you need to attach other delegates:
        // if (delegate) delegate->AttachDelegates(params.delegates.values());

        return { widget, delegate };
    }

}
