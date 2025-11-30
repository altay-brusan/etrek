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
        // Builder creates repository from dbConnection
        auto repository = std::make_shared<Etrek::ScanProtocol::Repository::ScanProtocolRepository>(params.dbConnection, nullptr);
        
        auto views = repository->getAllViews();
        auto* widget = new ViewConfigurationWidget(views.value, parentWidget);
        
        // Delegate receives repository if it needs to perform CRUD operations
        // In this case, delegate doesn't need repository (read-only widget)
        auto* delegate = new ViewConfigurationDelegate(widget, parentDelegate);

        // If you need to attach other delegates:
        // if (delegate) delegate->AttachDelegates(params.delegates.values());

        return { widget, delegate };
    }
}
