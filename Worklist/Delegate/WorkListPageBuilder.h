#ifndef WORKLISTPAGEBUILDER_H
#define WORKLISTPAGEBUILDER_H

#include "IWidgetDelegateBuilder.h"
#include "WorkListPageDelegate.h"
#include "WorkListPage.h"
#include "DelegateParameter.h"
#include <memory>

namespace Etrek::Worklist::Delegate {

    class WorkListPageBuilder
        : public IWidgetDelegateBuilder<WorkListPage, WorkListPageDelegate>
    {
    public:
        WorkListPageBuilder();
        ~WorkListPageBuilder();

        std::pair<WorkListPage*, WorkListPageDelegate*>
            build(const DelegateParameter& params,
                QWidget* parentWidget = nullptr,
                QObject* parentDelegate = nullptr) override;
    };
}
#endif // WORKLISTPAGEBUILDER_H
