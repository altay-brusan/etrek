#include "ViewConfigurationBuilder.h"
#include "ScanProtocolRepository.h"
#include "Result.h"

namespace Etrek::ScanProtocol::Delegate
{
    ViewConfigurationBuilder::ViewConfigurationBuilder()
    {
    }

    ViewConfigurationBuilder::~ViewConfigurationBuilder()
    {
    }

    std::pair<ViewConfigurationWidget*,ViewConfigurationDelegate*>
        ViewConfigurationBuilder::build(const DelegateParameter& params,
            QWidget* parentWidget,
            QObject* parentDelegate)
    {
        auto repo = std::make_shared<Etrek::ScanProtocol::Repository::ScanProtocolRepository>(params.dbConnection, nullptr);
        auto views = repo->getAllViews();
        auto* widget = new ViewConfigurationWidget(views.value,parentWidget);
        auto* delegate = new ViewConfigurationDelegate(widget, parentDelegate);

        // If you need to attach other delegates:
        // if (delegate) delegate->AttachDelegates(params.delegates.values());

        return { widget, delegate };
    }
}
