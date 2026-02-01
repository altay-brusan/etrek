#include "ContextAuditRepository.h"
#include "DatabaseConnectionSetting.h"

#include <QSqlQuery>
#include <QSqlError>
#include <QJsonDocument>
#include <QJsonObject>
#include <QUuid>
#include <QDebug>

namespace Etrek::Core::Repository {

using Etrek::Specification::Result;
using Etrek::Context::ContextType;
using Etrek::Context::AuditEventType;
using Etrek::Context::ContextAuditEntry;
using Etrek::Context::contextTypeToString;
using Etrek::Context::contextTypeFromString;
using Etrek::Context::auditEventTypeToString;
using Etrek::Context::auditEventTypeFromString;
using Etrek::Core::Data::Model::DatabaseConnectionSetting;

ContextAuditRepository::ContextAuditRepository(
    std::shared_ptr<DatabaseConnectionSetting> connectionSetting)
    : m_connectionSetting(std::move(connectionSetting))
{
}

QSqlDatabase ContextAuditRepository::createConnection(const QString& connectionName) const
{
    QSqlDatabase db = QSqlDatabase::addDatabase("QMYSQL", connectionName);
    db.setHostName(m_connectionSetting->getHostName());
    db.setDatabaseName(m_connectionSetting->getDatabaseName());
    db.setUserName(m_connectionSetting->getEtrekUserName());
    db.setPassword(m_connectionSetting->getPassword());
    db.setPort(m_connectionSetting->getPort());
    return db;
}

ContextAuditEntry ContextAuditRepository::parseEntry(const QSqlQuery& query) const
{
    ContextAuditEntry entry;

    entry.id = query.value("id").toInt();
    entry.contextType = contextTypeFromString(query.value("context_type").toString());
    entry.contextKey = query.value("context_key").toString();
    entry.eventType = auditEventTypeFromString(query.value("event_type").toString());

    QVariant userIdVar = query.value("user_id");
    if (!userIdVar.isNull()) {
        entry.userId = userIdVar.toInt();
    }

    entry.userName = query.value("user_name").toString();
    entry.workstationName = query.value("workstation_name").toString();
    entry.eventTimestamp = query.value("event_timestamp").toDateTime();

    // Parse JSON details
    QString detailsStr = query.value("details").toString();
    if (!detailsStr.isEmpty()) {
        QJsonDocument doc = QJsonDocument::fromJson(detailsStr.toUtf8());
        if (!doc.isNull() && doc.isObject()) {
            entry.details = doc.object();
        }
    }

    return entry;
}

Result<bool> ContextAuditRepository::insert(const ContextAuditEntry& entry)
{
    QString connectionName = QString("audit_insert_%1").arg(
        QUuid::createUuid().toString(QUuid::WithoutBraces));

    {
        QSqlDatabase db = createConnection(connectionName);
        if (!db.open()) {
            QString error = QString("Failed to open database: %1").arg(db.lastError().text());
            qWarning() << "ContextAuditRepository:" << error;
            return Result<bool>::Failure(error);
        }

        QSqlQuery query(db);
        query.prepare(R"(
            INSERT INTO context_audit_log
            (context_type, context_key, event_type, user_id, user_name, workstation_name, details)
            VALUES (?, ?, ?, ?, ?, ?, ?)
        )");

        query.addBindValue(contextTypeToString(entry.contextType));
        query.addBindValue(entry.contextKey.isEmpty() ? QVariant() : entry.contextKey);
        query.addBindValue(auditEventTypeToString(entry.eventType));
        query.addBindValue(entry.userId.has_value() ? QVariant(entry.userId.value()) : QVariant());
        query.addBindValue(entry.userName.isEmpty() ? QVariant() : entry.userName);
        query.addBindValue(entry.workstationName.isEmpty() ? QVariant() : entry.workstationName);

        // Convert JSON to string for storage
        QJsonDocument doc(entry.details);
        query.addBindValue(entry.details.isEmpty() ? QVariant() : doc.toJson(QJsonDocument::Compact));

        if (!query.exec()) {
            QString error = QString("Failed to insert audit entry: %1").arg(query.lastError().text());
            qWarning() << "ContextAuditRepository:" << error;
            return Result<bool>::Failure(error);
        }
    }

    QSqlDatabase::removeDatabase(connectionName);
    return Result<bool>::Success(true);
}

Result<QVector<ContextAuditEntry>> ContextAuditRepository::getByUserId(int userId, int limit)
{
    QVector<ContextAuditEntry> entries;
    QString connectionName = QString("audit_query_%1").arg(
        QUuid::createUuid().toString(QUuid::WithoutBraces));

    {
        QSqlDatabase db = createConnection(connectionName);
        if (!db.open()) {
            QString error = QString("Failed to open database: %1").arg(db.lastError().text());
            qWarning() << "ContextAuditRepository:" << error;
            return Result<QVector<ContextAuditEntry>>::Failure(error);
        }

        QSqlQuery query(db);
        query.prepare(R"(
            SELECT id, context_type, context_key, event_type, user_id, user_name,
                   workstation_name, details, event_timestamp
            FROM context_audit_log
            WHERE user_id = ?
            ORDER BY event_timestamp DESC
            LIMIT ?
        )");
        query.addBindValue(userId);
        query.addBindValue(limit);

        if (!query.exec()) {
            QString error = QString("Query failed: %1").arg(query.lastError().text());
            qWarning() << "ContextAuditRepository:" << error;
            return Result<QVector<ContextAuditEntry>>::Failure(error);
        }

        while (query.next()) {
            entries.append(parseEntry(query));
        }
    }

    QSqlDatabase::removeDatabase(connectionName);
    return Result<QVector<ContextAuditEntry>>::Success(entries);
}

Result<QVector<ContextAuditEntry>> ContextAuditRepository::getByContextType(
    ContextType contextType, int limit)
{
    QVector<ContextAuditEntry> entries;
    QString connectionName = QString("audit_query_%1").arg(
        QUuid::createUuid().toString(QUuid::WithoutBraces));

    {
        QSqlDatabase db = createConnection(connectionName);
        if (!db.open()) {
            QString error = QString("Failed to open database: %1").arg(db.lastError().text());
            qWarning() << "ContextAuditRepository:" << error;
            return Result<QVector<ContextAuditEntry>>::Failure(error);
        }

        QSqlQuery query(db);
        query.prepare(R"(
            SELECT id, context_type, context_key, event_type, user_id, user_name,
                   workstation_name, details, event_timestamp
            FROM context_audit_log
            WHERE context_type = ?
            ORDER BY event_timestamp DESC
            LIMIT ?
        )");
        query.addBindValue(contextTypeToString(contextType));
        query.addBindValue(limit);

        if (!query.exec()) {
            QString error = QString("Query failed: %1").arg(query.lastError().text());
            qWarning() << "ContextAuditRepository:" << error;
            return Result<QVector<ContextAuditEntry>>::Failure(error);
        }

        while (query.next()) {
            entries.append(parseEntry(query));
        }
    }

    QSqlDatabase::removeDatabase(connectionName);
    return Result<QVector<ContextAuditEntry>>::Success(entries);
}

Result<QVector<ContextAuditEntry>> ContextAuditRepository::getByDateRange(
    const QDateTime& from,
    const QDateTime& to,
    std::optional<ContextType> contextType)
{
    QVector<ContextAuditEntry> entries;
    QString connectionName = QString("audit_query_%1").arg(
        QUuid::createUuid().toString(QUuid::WithoutBraces));

    {
        QSqlDatabase db = createConnection(connectionName);
        if (!db.open()) {
            QString error = QString("Failed to open database: %1").arg(db.lastError().text());
            qWarning() << "ContextAuditRepository:" << error;
            return Result<QVector<ContextAuditEntry>>::Failure(error);
        }

        QString sql = R"(
            SELECT id, context_type, context_key, event_type, user_id, user_name,
                   workstation_name, details, event_timestamp
            FROM context_audit_log
            WHERE event_timestamp >= ? AND event_timestamp <= ?
        )";

        if (contextType.has_value()) {
            sql += " AND context_type = ?";
        }

        sql += " ORDER BY event_timestamp DESC";

        QSqlQuery query(db);
        query.prepare(sql);
        query.addBindValue(from);
        query.addBindValue(to);

        if (contextType.has_value()) {
            query.addBindValue(contextTypeToString(contextType.value()));
        }

        if (!query.exec()) {
            QString error = QString("Query failed: %1").arg(query.lastError().text());
            qWarning() << "ContextAuditRepository:" << error;
            return Result<QVector<ContextAuditEntry>>::Failure(error);
        }

        while (query.next()) {
            entries.append(parseEntry(query));
        }
    }

    QSqlDatabase::removeDatabase(connectionName);
    return Result<QVector<ContextAuditEntry>>::Success(entries);
}

Result<QVector<ContextAuditEntry>> ContextAuditRepository::getLoginHistory(int userId, int days)
{
    QVector<ContextAuditEntry> entries;
    QString connectionName = QString("audit_query_%1").arg(
        QUuid::createUuid().toString(QUuid::WithoutBraces));

    {
        QSqlDatabase db = createConnection(connectionName);
        if (!db.open()) {
            QString error = QString("Failed to open database: %1").arg(db.lastError().text());
            qWarning() << "ContextAuditRepository:" << error;
            return Result<QVector<ContextAuditEntry>>::Failure(error);
        }

        QSqlQuery query(db);
        query.prepare(R"(
            SELECT id, context_type, context_key, event_type, user_id, user_name,
                   workstation_name, details, event_timestamp
            FROM context_audit_log
            WHERE user_id = ?
              AND context_type = 'SESSION'
              AND event_type = 'CREATED'
              AND event_timestamp >= DATE_SUB(NOW(), INTERVAL ? DAY)
            ORDER BY event_timestamp DESC
        )");
        query.addBindValue(userId);
        query.addBindValue(days);

        if (!query.exec()) {
            QString error = QString("Query failed: %1").arg(query.lastError().text());
            qWarning() << "ContextAuditRepository:" << error;
            return Result<QVector<ContextAuditEntry>>::Failure(error);
        }

        while (query.next()) {
            entries.append(parseEntry(query));
        }
    }

    QSqlDatabase::removeDatabase(connectionName);
    return Result<QVector<ContextAuditEntry>>::Success(entries);
}

} // namespace Etrek::Core::Repository
