#ifndef ICONTEXTAUDITSERVICE_H
#define ICONTEXTAUDITSERVICE_H

#include <QString>
#include <QDateTime>
#include <QJsonObject>
#include <QVector>
#include <memory>
#include <optional>

namespace Etrek::Context {

class IContextManager;

/**
 * @enum ContextType
 * @brief Identifies the type of context being audited.
 */
enum class ContextType {
    SESSION,    ///< Session context (user login/logout)
    WORKFLOW    ///< Workflow context (examination, etc.)
};

/**
 * @enum AuditEventType
 * @brief Identifies the type of audit event.
 */
enum class AuditEventType {
    CREATED,    ///< Context was created
    UPDATED,    ///< Context was updated
    CLEARED     ///< Context was cleared
};

/**
 * @struct ContextAuditEntry
 * @brief Represents a single audit log entry.
 */
struct ContextAuditEntry {
    int id = 0;                         ///< Database ID
    ContextType contextType;            ///< Type of context
    QString contextKey;                 ///< Workflow key (empty for session)
    AuditEventType eventType;           ///< Type of event
    std::optional<int> userId;          ///< User ID (if available)
    QString userName;                   ///< Username at time of event
    QString workstationName;            ///< Workstation identifier
    QJsonObject details;                ///< Context-specific details
    QDateTime eventTimestamp;           ///< When the event occurred
};

/**
 * @interface IContextAuditService
 * @brief Interface for audit trail logging of context changes.
 *
 * The ContextAuditService monitors context changes via the ContextManager
 * and persists audit records to the database. This provides:
 * - Login/logout history
 * - Workflow tracking (examination starts/completions)
 * - Compliance audit trails
 *
 * Usage:
 * 1. Create service after authentication
 * 2. Call connectToContextManager() to start monitoring
 * 3. Service automatically logs all context changes
 * 4. Query methods provide access to audit history
 */
class IContextAuditService {
public:
    virtual ~IContextAuditService() = default;

    // --- Connection Management ---

    /**
     * @brief Connects to a ContextManager to monitor context changes.
     * @param manager The context manager to monitor.
     * @return true if connection succeeded; false otherwise.
     */
    virtual bool connectToContextManager(IContextManager* manager) = 0;

    /**
     * @brief Disconnects from the current ContextManager.
     */
    virtual void disconnectFromContextManager() = 0;

    /**
     * @brief Checks if the service is connected to a ContextManager.
     * @return true if connected; false otherwise.
     */
    virtual bool isConnected() const = 0;

    // --- Query Methods ---

    /**
     * @brief Retrieves audit history for a specific user.
     * @param userId The user ID to query.
     * @param limit Maximum number of entries to return (0 for all).
     * @return Vector of audit entries, most recent first.
     */
    virtual QVector<ContextAuditEntry> getAuditHistoryForUser(int userId, int limit = 100) = 0;

    /**
     * @brief Retrieves recent audit history by context type.
     * @param contextType The type of context to filter by.
     * @param limit Maximum number of entries to return.
     * @return Vector of audit entries, most recent first.
     */
    virtual QVector<ContextAuditEntry> getRecentAuditHistory(ContextType contextType, int limit = 100) = 0;

    /**
     * @brief Retrieves audit history within a date range.
     * @param from Start of date range (inclusive).
     * @param to End of date range (inclusive).
     * @param contextType Optional filter by context type.
     * @return Vector of audit entries, most recent first.
     */
    virtual QVector<ContextAuditEntry> getAuditHistoryByDateRange(
        const QDateTime& from,
        const QDateTime& to,
        std::optional<ContextType> contextType = std::nullopt) = 0;

    /**
     * @brief Retrieves login history for a specific user.
     *
     * Convenience method that filters for SESSION/CREATED events.
     *
     * @param userId The user ID to query.
     * @param days Number of days to look back (default 30).
     * @return Vector of login audit entries, most recent first.
     */
    virtual QVector<ContextAuditEntry> getLoginHistory(int userId, int days = 30) = 0;
};

/**
 * @brief Converts ContextType to string for database storage.
 */
inline QString contextTypeToString(ContextType type) {
    switch (type) {
        case ContextType::SESSION:  return QStringLiteral("SESSION");
        case ContextType::WORKFLOW: return QStringLiteral("WORKFLOW");
    }
    return QStringLiteral("UNKNOWN");
}

/**
 * @brief Converts string to ContextType.
 */
inline ContextType contextTypeFromString(const QString& str) {
    if (str == QStringLiteral("SESSION"))  return ContextType::SESSION;
    if (str == QStringLiteral("WORKFLOW")) return ContextType::WORKFLOW;
    return ContextType::SESSION; // Default
}

/**
 * @brief Converts AuditEventType to string for database storage.
 */
inline QString auditEventTypeToString(AuditEventType type) {
    switch (type) {
        case AuditEventType::CREATED: return QStringLiteral("CREATED");
        case AuditEventType::UPDATED: return QStringLiteral("UPDATED");
        case AuditEventType::CLEARED: return QStringLiteral("CLEARED");
    }
    return QStringLiteral("UNKNOWN");
}

/**
 * @brief Converts string to AuditEventType.
 */
inline AuditEventType auditEventTypeFromString(const QString& str) {
    if (str == QStringLiteral("CREATED")) return AuditEventType::CREATED;
    if (str == QStringLiteral("UPDATED")) return AuditEventType::UPDATED;
    if (str == QStringLiteral("CLEARED")) return AuditEventType::CLEARED;
    return AuditEventType::CREATED; // Default
}

} // namespace Etrek::Context

#endif // ICONTEXTAUDITSERVICE_H
