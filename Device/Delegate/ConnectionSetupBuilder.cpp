#include "ConnectionSetupBuilder.h"

namespace Etrek::Device::Delegate
{
    ConnectionSetupBuilder::ConnectionSetupBuilder()
    {
    }

    ConnectionSetupBuilder::~ConnectionSetupBuilder()
    {
    }

    std::pair<ConnectionSetupWidget*, ConnectionSetupDelegate*>
        ConnectionSetupBuilder::build(const DelegateParameter& params,
            QWidget* parentWidget,
            QObject* parentDelegate)
    {

        // Use params.dbConnection if needed
        auto widget = new ConnectionSetupWidget(parentWidget);
        auto delegate = new ConnectionSetupDelegate(widget, parentDelegate);
        // Optionally: delegate->AttachDelegates(params.delegates.values());
        return { widget, delegate };
    }
}
