#include "WorkListPageBuilder.h"
#include "IWorklistRepository.h"

namespace Etrek::Application::Delegate {
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
        // Use repositories from params instead of creating them
        std::shared_ptr<Etrek::Worklist::Repository::IWorklistRepository> irepository =
            std::static_pointer_cast<Etrek::Worklist::Repository::IWorklistRepository>(params.worklistRepository);

        auto* widget = new WorkListPage(irepository, parentWidget);

        auto* delegate = new WorkListPageDelegate(
            widget,
            params.worklistRepository,
            params.scanRepository,
            params.dicomRepository,
            params.dicomTagRepository,
            params.contextManager,
            parentDelegate);

        // If you need to attach other delegates:
        // if (delegate) delegate->AttachDelegates(params.delegates.values());

        return { widget, delegate };
    }

}
