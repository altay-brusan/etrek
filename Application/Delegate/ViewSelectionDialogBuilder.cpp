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

    // Create the scan protocol repository
    auto scanRepository = std::make_shared<Etrek::ScanProtocol::Repository::ScanProtocolRepository>(params.dbConnection);

    // Create the delegate with dependencies
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
