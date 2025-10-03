#include "ProcedureConfigurationBuilder.h"
#include "ScanProtocolRepository.h"

namespace Etrek::ScanProtocol::Delegate
{
    ProcedureConfigurationBuilder::ProcedureConfigurationBuilder()
    {
    }

    ProcedureConfigurationBuilder::~ProcedureConfigurationBuilder()
    {
    }

    std::pair<ProcedureConfigurationWidget*, ProcedureConfigurationDelegate*>
        ProcedureConfigurationBuilder::build(const DelegateParameter& params,
            QWidget* parentWidget,
            QObject* parentDelegate)
    {
        auto repo = std::make_shared<Etrek::ScanProtocol::Repository::ScanProtocolRepository>(params.dbConnection, nullptr);

        auto views = repo->getAllViews();
		auto procedures = repo->getAllProcedures();
        auto bodyParts = repo->getAllBodyParts();
        auto anatomicalRegions = repo->getAllAnatomicRegions();
        auto widget = new ProcedureConfigurationWidget(procedures.value, bodyParts.value, anatomicalRegions.value, views.value, parentWidget);
        auto delegate = new ProcedureConfigurationDelegate(widget, parentDelegate);

        // If you need to attach other delegates:
        // if (delegate) delegate->AttachDelegates(params.delegates.values());

        return { widget, delegate };
    }
}
