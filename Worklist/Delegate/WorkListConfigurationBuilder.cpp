#include "WorkListConfigurationBuilder.h"
#include "WorklistRepository.h"

using namespace Etrek::Worklist::Repository;
namespace Etrek::Worklist::Delegate {

    WorkListConfigurationBuilder::WorkListConfigurationBuilder()
    {
    }

    WorkListConfigurationBuilder::~WorkListConfigurationBuilder()
    {
    }

    std::pair<WorkListConfigurationWidget*, WorkListConfigurationDelegate*>
        WorkListConfigurationBuilder::build(const DelegateParameter& params,
            QWidget* parentWidget,
            QObject* parentDelegate)
    {
        auto repository = std::make_shared<WorklistRepository>(params.dbConnection);
		// TODO: this is hardcoded profile id, need to make it dynamic
        auto tags = repository->getTagsByProfile(1);
        auto* widget = new WorkListConfigurationWidget(tags.value,parentWidget);
        auto* delegate = new WorkListConfigurationDelegate(widget, parentDelegate);

        // If you need to attach other delegates:
        // if (delegate) delegate->AttachDelegates(params.delegates.values());

        return { widget, delegate };
    }
}
