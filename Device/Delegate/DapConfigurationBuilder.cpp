#include "DapConfigurationBuilder.h"

namespace Etrek::Device::Delegate
{
    DapConfigurationBuilder::DapConfigurationBuilder()
    {
    }

    DapConfigurationBuilder::~DapConfigurationBuilder()
    {
    }

    std::pair<DapConfigurationWidget*, DapConfigurationDelegate*>
        DapConfigurationBuilder::build(const DelegateParameter& params,
            QWidget* parentWidget,
            QObject* parentDelegate)
    {
        auto widget = new DapConfigurationWidget(parentWidget);
        auto delegate = new DapConfigurationDelegate(widget, parentDelegate);

        // If you need to attach other delegates:
        // if (delegate) delegate->AttachDelegates(params.delegates.values());

        return { widget, delegate };
    }
}
