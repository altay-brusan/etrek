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

namespace Etrek::Worklist::Repository {
    class WorklistRepository;
}

namespace Etrek::ScanProtocol::Repository {
    class ScanProtocolRepository;
}

namespace Etrek::Dicom::Repository {
    class DicomRepository;
    class DicomTagRepository;
}

namespace Etrek::Core::Data::Model {
    class DatabaseConnectionSetting;
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
 * - Repository instances for data access (commonly used repositories)
 * - Database connection for builders to create additional repositories
 * - References to other delegates (for inter-delegate communication)
 * - Context manager for accessing session and workflow contexts
 * - Pre-fetched contexts for immediate use
 *
 * IMPORTANT: Delegates should receive repositories, never dbConnection.
 * Only builders should use dbConnection to create repositories.
 */
struct DelegateParameter
{
    /// Commonly used repository instances
    std::shared_ptr<Etrek::Worklist::Repository::WorklistRepository> worklistRepository;
    std::shared_ptr<Etrek::ScanProtocol::Repository::ScanProtocolRepository> scanRepository;
    std::shared_ptr<Etrek::Dicom::Repository::DicomRepository> dicomRepository;
    std::shared_ptr<Etrek::Dicom::Repository::DicomTagRepository> dicomTagRepository;

    /// Database connection for builders to create additional repositories
    /// NOTE: Delegates should NOT receive or store this - only use repositories!
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

