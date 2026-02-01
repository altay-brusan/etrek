#ifndef CONTEXTAUDITSERVICE_H
#define CONTEXTAUDITSERVICE_H

#include <QObject>
#include <QMutex>
#include <memory>

#include "IContextAuditService.h"

namespace Etrek::Core::Repository {
    class ContextAuditRepository;
}

namespace Etrek::Context {
    class IContextManager;
    class ISessionContext;
    class IWorkflowContext;
}

namespace Etrek::Core::Context {

class ContextManager;

/**
 * @class ContextAuditService
 * @brief Implementation of IContextAuditService for audit trail logging.
 *
 * The ContextAuditService monitors context changes via the ContextManager
 * and persists audit records to the database via ContextAuditRepository.
 * This provides:
 * - Login/logout history tracking
 * - Workflow event logging (examination starts/completions)
 * - Compliance audit trails for regulatory requirements
 *
 * Thread-safe design. Database operations are delegated to the repository.
 *
 * Usage:
 * 1. Create service with a ContextAuditRepository
 * 2. Call connectToContextManager() to start monitoring
 * 3. Service automatically logs all context changes
 * 4. Query methods provide access to audit history
 */
class ContextAuditService : public QObject, public Etrek::Context::IContextAuditService {
    Q_OBJECT

public:
    /**
     * @brief Constructs a ContextAuditService.
     * @param repository Shared pointer to the audit repository.
     * @param parent Parent QObject for memory management.
     */
    explicit ContextAuditService(
        std::shared_ptr<Etrek::Core::Repository::ContextAuditRepository> repository,
        QObject* parent = nullptr);

    ~ContextAuditService() override;

    // --- IContextAuditService: Connection Management ---
    bool connectToContextManager(Etrek::Context::IContextManager* manager) override;
    void disconnectFromContextManager() override;
    bool isConnected() const override;

    // --- IContextAuditService: Query Methods ---
    QVector<Etrek::Context::ContextAuditEntry> getAuditHistoryForUser(int userId, int limit = 100) override;
    QVector<Etrek::Context::ContextAuditEntry> getRecentAuditHistory(Etrek::Context::ContextType contextType, int limit = 100) override;
    QVector<Etrek::Context::ContextAuditEntry> getAuditHistoryByDateRange(
        const QDateTime& from,
        const QDateTime& to,
        std::optional<Etrek::Context::ContextType> contextType = std::nullopt) override;
    QVector<Etrek::Context::ContextAuditEntry> getLoginHistory(int userId, int days = 30) override;

private slots:
    /**
     * @brief Handles session context changes.
     */
    void onSessionContextChanged();

    /**
     * @brief Handles workflow context changes.
     * @param key The workflow identifier.
     */
    void onWorkflowContextChanged(const QString& key);

    /**
     * @brief Handles workflow context cleared events.
     * @param key The workflow identifier.
     */
    void onWorkflowContextCleared(const QString& key);

private:
    /**
     * @brief Extracts JSON details from a session context.
     * @param context The session context.
     * @return JSON object with context details.
     */
    QJsonObject extractSessionDetails(const std::shared_ptr<Etrek::Context::ISessionContext>& context) const;

    /**
     * @brief Extracts JSON details from a workflow context.
     * @param context The workflow context.
     * @return JSON object with context details.
     */
    QJsonObject extractWorkflowDetails(const std::shared_ptr<Etrek::Context::IWorkflowContext>& context) const;

    mutable QMutex m_mutex;
    std::shared_ptr<Etrek::Core::Repository::ContextAuditRepository> m_repository;
    ContextManager* m_contextManager = nullptr;
    bool m_hasSessionContext = false;  // Track if we had a session to detect cleared events
};

} // namespace Etrek::Core::Context

#endif // CONTEXTAUDITSERVICE_H
