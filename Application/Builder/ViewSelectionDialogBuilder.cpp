#include "ViewSelectionDialogBuilder.h"
#include "ScanProtocolRepository.h"

namespace Etrek::Application::Delegate {

ViewSelectionDialogBuilder::ViewSelectionDialogBuilder()
{
}

ViewSelectionDialogBuilder::~ViewSelectionDialogBuilder()
{
}

std::pair<ViewSelectionDialog*, ViewSelectionDialogDelegate*>
ViewSelectionDialogBuilder::build(const DelegateParameter& params,
    QWidget* parentWidget,
    QObject* parentDelegate)
{
    // Create the dialog widget
    auto* dialog = new ViewSelectionDialog(parentWidget);

    // Builder creates repository from dbConnection
    auto scanRepository = std::make_shared<Etrek::ScanProtocol::Repository::ScanProtocolRepository>(params.dbConnection, nullptr);

    // Create the delegate with dependencies (delegate receives repository)
    auto* delegate = new ViewSelectionDialogDelegate(
        dialog,
        scanRepository,
        params.contextManager,
        parentDelegate
    );

    // Initialize the delegate (loads patient info and procedures)
    delegate->initialize();

    return { dialog, delegate };
}

} // namespace Etrek::Application::Delegate
