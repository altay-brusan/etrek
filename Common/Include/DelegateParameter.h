#ifndef DELEGATEPARAMETERS_H
#define DELEGATEPARAMETERS_H

#include <memory>
#include <QMap>
#include <QString>
#include "DatabaseConnectionSetting.h"
#include "IDelegate.h"

namespace Etrek::Context {
    class IContextManager;
    class ISessionContext;
    class IWorkflowContext;
}

/**
 * @struct DelegateParameter
 * @brief Parameters passed to delegates during construction.
 *
 * This structure carries configuration and context information that delegates
 * need to operate. Builders are responsible for populating these parameters
 * and passing them to delegate constructors.
 *
 * The structure includes:
 * - Database connection settings
 * - References to other delegates (for inter-delegate communication)
 * - Context manager for accessing session and workflow contexts
 * - Pre-fetched contexts for immediate use
 */
struct DelegateParameter
{
    /// Database connection settings
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

