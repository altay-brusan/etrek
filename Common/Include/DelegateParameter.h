#ifndef DELEGATEPARAMETERS_H
#define DELEGATEPARAMETERS_H

#include <memory>
#include <QMap>
#include <QString>
#include "IDelegate.h"

namespace Etrek::Context {
    class IContextManager;
    class ISessionContext;
    class IWorkflowContext;
}

namespace Etrek::Core::Data::Model {
    class DatabaseConnectionSetting;
}

/**
 * @struct DelegateParameter
 * @brief Parameters passed to builders during construction.
 *
 * This structure carries configuration and context information that builders
 * need to operate. Builders are responsible for creating repositories from
 * dbConnection and passing them to delegates if needed.
 *
 * The structure includes:
 * - Database connection for builders to create repositories
 * - References to other delegates (for inter-delegate communication)
 * - Context manager for accessing session and workflow contexts
 * - Pre-fetched contexts for immediate use
 *
 * IMPORTANT ARCHITECTURAL PATTERN:
 * - DelegateParameter ONLY contains dbConnection (NO repositories)
 * - Builders receive DelegateParameter and create repositories from dbConnection
 * - Builders pass repository instances to delegates if delegates need them
 * - Delegates NEVER receive dbConnection, only repositories
 * - Each builder is independent and creates its own repository instances
 */
struct DelegateParameter
{
    /// Database connection for builders to create repositories
    /// NOTE: Only builders use this, delegates receive repository instances from builders
    std::shared_ptr<Etrek::Core::Data::Model::DatabaseConnectionSetting> dbConnection;

    /// Map of delegate names to delegate instances for inter-delegate dependencies
    QMap<QString, QWeakPointer<IDelegate>> delegates;

    /// Context manager for accessing session and workflow contexts
    std::weak_ptr<Etrek::Context::IContextManager> contextManager;

    /// Pre-fetched session context (optional, for convenience)
    std::shared_ptr<Etrek::Context::ISessionContext> sessionContext;

    /// Pre-fetched workflow context relevant to the delegate (optional)
    std::shared_ptr<Etrek::Context::IWorkflowContext> workflowContext;
};

#endif // DELEGATEPARAMETERS_H

