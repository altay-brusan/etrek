#ifndef VIEWCONFIGURATIONBUILDER_H
#define VIEWCONFIGURATIONBUILDER_H

#include "IWidgetDelegateBuilder.h"
#include "ViewConfigurationDelegate.h"
#include "ViewConfigurationWidget.h"
#include "DelegateParameter.h"
#include <memory>


namespace Etrek::ScanProtocol::Delegate
{
    class ViewConfigurationBuilder :
        public IWidgetDelegateBuilder<ViewConfigurationWidget, ViewConfigurationDelegate>
    {
    public:
        ViewConfigurationBuilder();
        ~ViewConfigurationBuilder();

        std::pair<ViewConfigurationWidget*,ViewConfigurationDelegate*>
            build(const DelegateParameter& params,
                QWidget* parentWidget = nullptr,
                QObject* parentDelegate = nullptr) override;
    };
}

#endif // VIEWCONFIGURATIONBUILDER_H
