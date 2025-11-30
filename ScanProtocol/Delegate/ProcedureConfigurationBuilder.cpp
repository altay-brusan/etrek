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
        // Builder creates repository from dbConnection
        auto repository = std::make_shared<Etrek::ScanProtocol::Repository::ScanProtocolRepository>(params.dbConnection, nullptr);

        auto views = repository->getAllViews();
        auto procedures = repository->getAllProcedures();
        auto bodyParts = repository->getAllBodyParts();
        auto anatomicalRegions = repository->getAllAnatomicRegions();
        auto widget = new ProcedureConfigurationWidget(procedures.value, bodyParts.value, anatomicalRegions.value, views.value, parentWidget);
        
        // Delegate receives repository if it needs to perform CRUD operations
        // In this case, delegate doesn't need repository (read-only widget)
        auto delegate = new ProcedureConfigurationDelegate(widget, parentDelegate);

        // If you need to attach other delegates:
        // if (delegate) delegate->AttachDelegates(params.delegates.values());

        return { widget, delegate };
    }
}
