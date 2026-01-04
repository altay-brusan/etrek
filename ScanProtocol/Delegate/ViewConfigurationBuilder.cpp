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

        // Delegate receives repository to perform CRUD operations on apply/accept
        auto* delegate = new ViewConfigurationDelegate(widget, repository, parentDelegate);

        return { widget, delegate };
    }
}
