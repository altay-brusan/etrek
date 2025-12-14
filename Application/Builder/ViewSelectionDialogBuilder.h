#ifndef VIEWSELECTIONDIALOGBUILDER_H
#define VIEWSELECTIONDIALOGBUILDER_H

#include "IWidgetDelegateBuilder.h"
#include "ViewSelectionDialogDelegate.h"
#include "ViewSelectionDialog.h"
#include "DelegateParameter.h"
#include <memory>

namespace Etrek::Application::Delegate {

class ViewSelectionDialogBuilder
    : public IWidgetDelegateBuilder<ViewSelectionDialog, ViewSelectionDialogDelegate>
{
public:
    ViewSelectionDialogBuilder();
    ~ViewSelectionDialogBuilder();

    std::pair<ViewSelectionDialog*, ViewSelectionDialogDelegate*>
        build(const DelegateParameter& params,
            QWidget* parentWidget = nullptr,
            QObject* parentDelegate = nullptr) override;
};

} // namespace Etrek::Application::Delegate

#endif // VIEWSELECTIONDIALOGBUILDER_H
