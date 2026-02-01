#include "RisProcedureMappingConfigurationBuilder.h"
#include "RisProcedureMappingRepository.h"
#include <QSet>

namespace Etrek::Worklist::Delegate
{
    using Etrek::Worklist::Repository::RisProcedureMappingRepository;

    RisProcedureMappingConfigurationBuilder::RisProcedureMappingConfigurationBuilder()
    {
    }

    RisProcedureMappingConfigurationBuilder::~RisProcedureMappingConfigurationBuilder()
    {
    }

    std::pair<RisProcedureMappingConfigurationWidget*, RisProcedureMappingConfigurationDelegate*>
        RisProcedureMappingConfigurationBuilder::build(
            const DelegateParameter& params,
            QWidget* parentWidget,
            QObject* parentDelegate)
    {
        // Builder creates repository from dbConnection
        auto repository = std::make_shared<RisProcedureMappingRepository>(params.dbConnection);

        // Fetch all mappings
        auto mappings = repository->getAllActive();

        // Extract unique connection names from mappings
        QSet<QString> connectionSet;
        for (const auto& mapping : mappings) {
            if (!mapping.ConnectionName.isEmpty()) {
                connectionSet.insert(mapping.ConnectionName);
            }
        }
        QStringList connectionNames = connectionSet.values();
        connectionNames.sort();

        // Create widget with data
        auto* widget = new RisProcedureMappingConfigurationWidget(
            mappings, connectionNames, parentWidget);

        // Delegate receives repository to perform CRUD operations on apply/accept
        auto* delegate = new RisProcedureMappingConfigurationDelegate(
            widget, repository, params.contextManager, parentDelegate);

        return { widget, delegate };
    }
}
