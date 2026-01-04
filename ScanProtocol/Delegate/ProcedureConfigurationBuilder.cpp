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

        // Delegate receives repository to perform CRUD operations on apply/accept
        auto delegate = new ProcedureConfigurationDelegate(widget, repository, parentDelegate);

        return { widget, delegate };
    }
}
