#ifndef CONTEXTAUDITREPOSITORY_H
#define CONTEXTAUDITREPOSITORY_H

#include <QString>
#include <QVector>
#include <QDateTime>
#include <QSqlDatabase>
#include <memory>
#include <optional>

#include "IContextAuditService.h"
#include "Result.h"

namespace Etrek::Core::Data::Model {
    class DatabaseConnectionSetting;
}

namespace Etrek::Core::Repository {

/**
 * @class ContextAuditRepository
 * @brief Repository for context audit log database operations.
 *
 * This class handles all database operations for the context_audit_log table,
 * following the repository pattern used throughout the codebase.
 *
 * Responsibilities:
 * - Insert audit entries (login, logout, workflow events)
 * - Query audit history by user, context type, date range
 * - Query login history for compliance reporting
 */
class ContextAuditRepository {
public:
    /**
     * @brief Constructs a ContextAuditRepository.
     * @param connectionSetting Shared pointer to the database connection settings.
     */
    explicit ContextAuditRepository(
        std::shared_ptr<Etrek::Core::Data::Model::DatabaseConnectionSetting> connectionSetting);

    ~ContextAuditRepository() = default;

    // --- Write Operations ---

    /**
     * @brief Inserts an audit entry into the database.
     * @param entry The audit entry to insert.
     * @return Result containing true on success, or an error message.
     */
    Etrek::Specification::Result<bool> insert(const Etrek::Context::ContextAuditEntry& entry);

    // --- Read Operations ---

    /**
     * @brief Retrieves audit entries for a specific user.
     * @param userId The user ID to query.
     * @param limit Maximum number of entries to return.
     * @return Result containing vector of entries, or an error message.
     */
    Etrek::Specification::Result<QVector<Etrek::Context::ContextAuditEntry>> getByUserId(
        int userId, int limit = 100);

    /**
     * @brief Retrieves audit entries by context type.
     * @param contextType The context type to filter by.
     * @param limit Maximum number of entries to return.
     * @return Result containing vector of entries, or an error message.
     */
    Etrek::Specification::Result<QVector<Etrek::Context::ContextAuditEntry>> getByContextType(
        Etrek::Context::ContextType contextType, int limit = 100);

    /**
     * @brief Retrieves audit entries within a date range.
     * @param from Start of date range (inclusive).
     * @param to End of date range (inclusive).
     * @param contextType Optional filter by context type.
     * @return Result containing vector of entries, or an error message.
     */
    Etrek::Specification::Result<QVector<Etrek::Context::ContextAuditEntry>> getByDateRange(
        const QDateTime& from,
        const QDateTime& to,
        std::optional<Etrek::Context::ContextType> contextType = std::nullopt);

    /**
     * @brief Retrieves login history for a specific user.
     * @param userId The user ID to query.
     * @param days Number of days to look back.
     * @return Result containing vector of login entries, or an error message.
     */
    Etrek::Specification::Result<QVector<Etrek::Context::ContextAuditEntry>> getLoginHistory(
        int userId, int days = 30);

private:
    /**
     * @brief Creates a database connection with a unique name.
     * @param connectionName The name for the connection.
     * @return QSqlDatabase instance (not yet opened).
     */
    QSqlDatabase createConnection(const QString& connectionName) const;

    /**
     * @brief Parses a query result row into a ContextAuditEntry.
     * @param query The executed query positioned at a valid row.
     * @return Parsed ContextAuditEntry.
     */
    Etrek::Context::ContextAuditEntry parseEntry(const QSqlQuery& query) const;

    std::shared_ptr<Etrek::Core::Data::Model::DatabaseConnectionSetting> m_connectionSetting;
};

} // namespace Etrek::Core::Repository

#endif // CONTEXTAUDITREPOSITORY_H
