#include "CollimatorConfigurationBuilder.h"

namespace Etrek::Device::Delegate
{
    CollimatorConfigurationBuilder::CollimatorConfigurationBuilder()
    {
    }

    CollimatorConfigurationBuilder::~CollimatorConfigurationBuilder()
    {
    }

    std::pair<CollimatorConfigurationWidget*, CollimatorConfigurationDelegate*>
        CollimatorConfigurationBuilder::build(const DelegateParameter& params,
            QWidget* parentWidget,
            QObject* parentDelegate)
    {
        auto widget = new CollimatorConfigurationWidget(parentWidget);
        auto delegate = new CollimatorConfigurationDelegate(widget, parentDelegate);

        // If attach other delegates are needed:
        // if (delegate) delegate->AttachDelegates(params.delegates.values());

        return { widget, delegate };
    }
}
