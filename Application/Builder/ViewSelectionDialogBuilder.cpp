#include "ViewSelectionDialogBuilder.h"

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

    // Use the scan protocol repository from params
    // Create the delegate with dependencies
    auto* delegate = new ViewSelectionDialogDelegate(
        dialog,
        params.scanRepository,
        params.contextManager,
        parentDelegate
    );

    // Initialize the delegate (loads patient info and procedures)
    delegate->initialize();

    return { dialog, delegate };
}

} // namespace Etrek::Application::Delegate
