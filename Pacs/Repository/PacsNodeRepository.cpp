#include "PacsNodeRepository.h"
#include "AppLoggerFactory.h"
#include "AppLogger.h"

#include <QVariant>

using namespace Etrek::Core::Log;
using namespace Etrek::Pacs;
using namespace Etrek::Pacs::Data::Entity;

static inline QString kRepoName() { return "PacsNodeRepository"; }
static inline QString kTable() { return "pacs_nodes"; }

static inline QString errOpen(const QSqlDatabase& db, TranslationProvider* /*tr*/) {
    return QString("Failed to open database: %1").arg(db.lastError().text());
}
static inline QString errExec(const QSqlQuery& q, TranslationProvider* /*tr*/) {
    return QString("Query failed: %1").arg(q.lastError().text());
}

PacsNodeRepository::PacsNodeRepository(std::shared_ptr<DatabaseConnectionSetting> connectionSetting)
    : m_connectionSetting(std::move(connectionSetting))   
{
    translator = &TranslationProvider::Instance();
    AppLoggerFactory factory(LoggerProvider::Instance(), translator);
    logger = factory.CreateLogger("PacsNodeRepository");
}

QSqlDatabase PacsNodeRepository::createConnection(const QString& connectionName) const
{
    QSqlDatabase db = QSqlDatabase::addDatabase("QMYSQL", connectionName);
    db.setHostName(m_connectionSetting->getHostName());
    db.setDatabaseName(m_connectionSetting->getDatabaseName());
    db.setUserName(m_connectionSetting->getEtrekUserName());
    db.setPassword(m_connectionSetting->getPassword());
    db.setPort(m_connectionSetting->getPort());
    return db;
}

// --- Enum mapping helpers ---
QString PacsNodeRepository::toEntityTypeString(PacsEntityType t)
{
    switch (t) {
    case PacsEntityType::Archive: return QStringLiteral("Archive");
    case PacsEntityType::MPPS:    return QStringLiteral("MPPS");
    default:                      return QStringLiteral("Archive");
    }
}

PacsEntityType PacsNodeRepository::parseEntityTypeString(const QString& s)
{
    const QString v = s.trimmed();
    if (v.compare("Archive", Qt::CaseInsensitive) == 0) return PacsEntityType::Archive;
    if (v.compare("MPPS", Qt::CaseInsensitive) == 0) return PacsEntityType::MPPS;
    return PacsEntityType::Archive;
}

// --- CRUD ---

QVector<PacsNode> PacsNodeRepository::getPacsNodes() const
{
    QVector<PacsNode> out;
    const QString cx = "pacs_nodes_all_" + QString::number(QRandomGenerator::global()->generate());

    {
        QSqlDatabase db = createConnection(cx);
        if (!db.open()) {
            const auto err = errOpen(db, translator);
            logger->LogError(err);
            return out;
        }

        QSqlQuery q(db);
        q.prepare(QStringLiteral(R"(
            SELECT id, entity_type, host_name, host_ip, host_port, called_aet, calling_aet
            FROM %1
            ORDER BY id ASC
        )").arg(kTable()));

        if (!q.exec()) {
            const auto err = errExec(q, translator);
            logger->LogError(err);
            return out;
        }

        while (q.next()) {
            PacsNode n;
            n.Id = q.value("id").toInt();
            n.Type = parseEntityTypeString(q.value("entity_type").toString());
            n.HostName = q.value("host_name").toString();
            n.HostIp = q.value("host_ip").toString();
            n.Port = q.value("host_port").toInt();
            n.CalledAet = q.value("called_aet").toString();
            n.CallingAet = q.value("calling_aet").toString();
            out.append(n);
        }
    }

    QSqlDatabase::removeDatabase(cx);
    return out;
}

Etrek::Specification::Result<PacsNode> PacsNodeRepository::addPacsNode(const PacsNode& node) const
{
    const QString cx = "pacs_nodes_add_" + QString::number(QRandomGenerator::global()->generate());
    PacsNode inserted = node;

    // Basic validations (tune as needed)
    if (node.HostName.trimmed().isEmpty())
        return Etrek::Specification::Result<PacsNode>::Failure("HostName is required.");
    if (node.HostIp.trimmed().isEmpty())
        return Etrek::Specification::Result<PacsNode>::Failure("HostIp is required.");
    if (node.Port <= 0 || node.Port > 65535)
        return Etrek::Specification::Result<PacsNode>::Failure("Port must be between 1 and 65535.");
    if (node.CalledAet.trimmed().isEmpty())
        return Etrek::Specification::Result<PacsNode>::Failure("CalledAET is required.");
    if (node.CallingAet.trimmed().isEmpty())
        return Etrek::Specification::Result<PacsNode>::Failure("CallingAET is required.");

    {
        QSqlDatabase db = createConnection(cx);
        if (!db.open()) {
            const auto err = errOpen(db, translator);
            logger->LogError(err);
            return Etrek::Specification::Result<PacsNode>::Failure(err);
        }

        QSqlQuery q(db);
        q.prepare(QStringLiteral(R"(
            INSERT INTO %1
                (entity_type, host_name, host_ip, host_port, called_aet, calling_aet, is_default)
            VALUES
                (?, ?, ?, ?, ?, ?, ?)
        )").arg(kTable()));

        q.addBindValue(toEntityTypeString(node.Type));  // ENUM('Archive','MPPS')
        q.addBindValue(node.HostName);
        q.addBindValue(node.HostIp);
        q.addBindValue(node.Port);
        q.addBindValue(node.CalledAet);
        q.addBindValue(node.CallingAet);
        q.addBindValue(false);                          // default flag (adjust if you add to entity)

        if (!q.exec()) {
            const auto err = errExec(q, translator);
            logger->LogError(err);
            return Etrek::Specification::Result<PacsNode>::Failure(err);
        }

        const QVariant newId = q.lastInsertId();
        if (newId.isValid()) inserted.Id = newId.toInt();
    }

    QSqlDatabase::removeDatabase(cx);
    return Etrek::Specification::Result<PacsNode>::Success(inserted, "Inserted");
}

Etrek::Specification::Result<PacsNode> PacsNodeRepository::updatePacsNode(const PacsNode& node) const
{
    if (node.Id <= 0)
        return Etrek::Specification::Result<PacsNode>::Failure("Invalid node Id.");

    if (node.HostName.trimmed().isEmpty())
        return Etrek::Specification::Result<PacsNode>::Failure("HostName is required.");
    if (node.HostIp.trimmed().isEmpty())
        return Etrek::Specification::Result<PacsNode>::Failure("HostIp is required.");
    if (node.Port <= 0 || node.Port > 65535)
        return Etrek::Specification::Result<PacsNode>::Failure("Port must be between 1 and 65535.");
    if (node.CalledAet.trimmed().isEmpty())
        return Etrek::Specification::Result<PacsNode>::Failure("CalledAET is required.");
    if (node.CallingAet.trimmed().isEmpty())
        return Etrek::Specification::Result<PacsNode>::Failure("CallingAET is required.");

    const QString cx = "pacs_nodes_upd_" + QString::number(QRandomGenerator::global()->generate());

    {
        QSqlDatabase db = createConnection(cx);
        if (!db.open()) {
            const auto err = errOpen(db, translator);
            logger->LogError(err);
            return Etrek::Specification::Result<PacsNode>::Failure(err);
        }

        QSqlQuery q(db);
        q.prepare(QStringLiteral(R"(
            UPDATE %1
            SET entity_type = ?,
                host_name   = ?,
                host_ip     = ?,
                host_port   = ?,
                called_aet  = ?,
                calling_aet = ?
            WHERE id = ?
        )").arg(kTable()));

        q.addBindValue(toEntityTypeString(node.Type));
        q.addBindValue(node.HostName);
        q.addBindValue(node.HostIp);
        q.addBindValue(node.Port);
        q.addBindValue(node.CalledAet);
        q.addBindValue(node.CallingAet);
        q.addBindValue(node.Id);

        if (!q.exec()) {
            const auto err = errExec(q, translator);
            logger->LogError(err);
            return Etrek::Specification::Result<PacsNode>::Failure(err);
        }

        if (q.numRowsAffected() == 0) {
            return Etrek::Specification::Result<PacsNode>::Failure("No rows updated (node not found?).");
        }
    }

    QSqlDatabase::removeDatabase(cx);
    return Etrek::Specification::Result<PacsNode>::Success(node, "Updated");
}

Etrek::Specification::Result<bool> PacsNodeRepository::removePacsNode(const PacsNode& node) const
{
    if (node.Id <= 0)
        return Etrek::Specification::Result<bool>::Failure("Invalid node Id.");

    const QString cx = "pacs_nodes_del_" + QString::number(QRandomGenerator::global()->generate());
    bool deleted = false;

    {
        QSqlDatabase db = createConnection(cx);
        if (!db.open()) {
            const auto err = errOpen(db, translator);
            logger->LogError(err);
            return Etrek::Specification::Result<bool>::Failure(err);
        }

        QSqlQuery q(db);
        q.prepare(QStringLiteral(R"(
            DELETE FROM %1 WHERE id = ?
        )").arg(kTable()));

        q.addBindValue(node.Id);

        if (!q.exec()) {
            const auto err = errExec(q, translator);
            logger->LogError(err);
            return Etrek::Specification::Result<bool>::Failure(err);
        }

        deleted = (q.numRowsAffected() > 0);
        if (!deleted) {
            return Etrek::Specification::Result<bool>::Failure("No rows deleted (node not found?).");
        }
    }

    QSqlDatabase::removeDatabase(cx);
    return Etrek::Specification::Result<bool>::Success(true, "Deleted");
}
