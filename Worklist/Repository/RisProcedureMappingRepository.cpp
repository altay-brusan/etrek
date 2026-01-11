#include "RisProcedureMappingRepository.h"
#include "AppLoggerFactory.h"
#include "TranslationProvider.h"
#include <QSqlQuery>
#include <QSqlError>
#include <QRandomGenerator>

namespace Etrek::Worklist::Repository {

    using Etrek::Core::Log::AppLoggerFactory;
    using Etrek::Core::Log::LoggerProvider;
    using Etrek::Core::Globalization::TranslationProvider;

    RisProcedureMappingRepository::RisProcedureMappingRepository(
        std::shared_ptr<Etrek::Core::Data::Model::DatabaseConnectionSetting> connectionSetting,
        QObject* parent)
        : QObject(parent)
        , m_connectionSetting(std::move(connectionSetting))
    {
        AppLoggerFactory factory(LoggerProvider::Instance(), &TranslationProvider::Instance());
        m_logger = factory.CreateLogger("RisProcedureMappingRepository");
    }

    QSqlDatabase RisProcedureMappingRepository::createConnection(const QString& connectionName) const
    {
        QSqlDatabase db = QSqlDatabase::addDatabase("QMYSQL", connectionName);
        db.setHostName(m_connectionSetting->getHostName());
        db.setPort(m_connectionSetting->getPort());
        db.setDatabaseName(m_connectionSetting->getDatabaseName());
        db.setUserName(m_connectionSetting->getEtrekUserName());
        db.setPassword(m_connectionSetting->getPassword());
        return db;
    }

    ent::RisProcedureMapping RisProcedureMappingRepository::mapRowToEntity(const QSqlQuery& query) const
    {
        ent::RisProcedureMapping mapping;
        mapping.Id = query.value("id").toInt();
        mapping.ConnectionName = query.value("connection_name").toString();
        mapping.ExternalCode = query.value("external_code").toString();
        mapping.ExternalCodingScheme = query.value("external_coding_scheme").toString();
        mapping.ExternalCodeMeaning = query.value("external_code_meaning").toString();
        mapping.InternalViewId = query.value("internal_view_id").toInt();
        mapping.IsActive = query.value("is_active").toBool();
        mapping.Notes = query.value("notes").toString();
        mapping.CreatedAt = query.value("created_at").toDateTime();
        mapping.UpdatedAt = query.value("updated_at").toDateTime();
        return mapping;
    }

    Etrek::Specification::Result<ent::RisProcedureMapping> RisProcedureMappingRepository::insert(
        ent::RisProcedureMapping& mapping)
    {
        const QString cx = "ris_mapping_insert_" + QString::number(QRandomGenerator::global()->generate());
        {
            QSqlDatabase db = createConnection(cx);
            if (!db.open()) {
                const auto err = QString("Failed to open database: %1").arg(db.lastError().text());
                m_logger->LogError(err);
                return Etrek::Specification::Result<ent::RisProcedureMapping>::Failure(err);
            }

            QSqlQuery q(db);
            q.prepare(R"(
                INSERT INTO ris_procedure_mapping (
                    connection_name, external_code, external_coding_scheme,
                    external_code_meaning, internal_view_id, is_active, notes
                ) VALUES (
                    :connection_name, :external_code, :external_coding_scheme,
                    :external_code_meaning, :internal_view_id, :is_active, :notes
                )
            )");

            q.bindValue(":connection_name", mapping.ConnectionName);
            q.bindValue(":external_code", mapping.ExternalCode);
            q.bindValue(":external_coding_scheme", mapping.ExternalCodingScheme.isEmpty()
                ? QVariant(QVariant::String) : mapping.ExternalCodingScheme);
            q.bindValue(":external_code_meaning", mapping.ExternalCodeMeaning.isEmpty()
                ? QVariant(QVariant::String) : mapping.ExternalCodeMeaning);
            q.bindValue(":internal_view_id", mapping.InternalViewId);
            q.bindValue(":is_active", mapping.IsActive);
            q.bindValue(":notes", mapping.Notes.isEmpty() ? QVariant(QVariant::String) : mapping.Notes);

            if (!q.exec()) {
                const auto err = QString("Failed to insert RIS procedure mapping: %1").arg(q.lastError().text());
                m_logger->LogError(err);
                return Etrek::Specification::Result<ent::RisProcedureMapping>::Failure(err);
            }

            mapping.Id = q.lastInsertId().toInt();
            db.close();
        }
        QSqlDatabase::removeDatabase(cx);
        return Etrek::Specification::Result<ent::RisProcedureMapping>::Success(mapping);
    }

    Etrek::Specification::Result<bool> RisProcedureMappingRepository::update(
        const ent::RisProcedureMapping& mapping)
    {
        const QString cx = "ris_mapping_update_" + QString::number(QRandomGenerator::global()->generate());
        {
            QSqlDatabase db = createConnection(cx);
            if (!db.open()) {
                const auto err = QString("Failed to open database: %1").arg(db.lastError().text());
                m_logger->LogError(err);
                return Etrek::Specification::Result<bool>::Failure(err);
            }

            QSqlQuery q(db);
            q.prepare(R"(
                UPDATE ris_procedure_mapping SET
                    connection_name = :connection_name,
                    external_code = :external_code,
                    external_coding_scheme = :external_coding_scheme,
                    external_code_meaning = :external_code_meaning,
                    internal_view_id = :internal_view_id,
                    is_active = :is_active,
                    notes = :notes
                WHERE id = :id
            )");

            q.bindValue(":id", mapping.Id);
            q.bindValue(":connection_name", mapping.ConnectionName);
            q.bindValue(":external_code", mapping.ExternalCode);
            q.bindValue(":external_coding_scheme", mapping.ExternalCodingScheme.isEmpty()
                ? QVariant(QVariant::String) : mapping.ExternalCodingScheme);
            q.bindValue(":external_code_meaning", mapping.ExternalCodeMeaning.isEmpty()
                ? QVariant(QVariant::String) : mapping.ExternalCodeMeaning);
            q.bindValue(":internal_view_id", mapping.InternalViewId);
            q.bindValue(":is_active", mapping.IsActive);
            q.bindValue(":notes", mapping.Notes.isEmpty() ? QVariant(QVariant::String) : mapping.Notes);

            if (!q.exec()) {
                const auto err = QString("Failed to update RIS procedure mapping: %1").arg(q.lastError().text());
                m_logger->LogError(err);
                return Etrek::Specification::Result<bool>::Failure(err);
            }

            db.close();
        }
        QSqlDatabase::removeDatabase(cx);
        return Etrek::Specification::Result<bool>::Success(true);
    }

    Etrek::Specification::Result<bool> RisProcedureMappingRepository::remove(int id)
    {
        const QString cx = "ris_mapping_remove_" + QString::number(QRandomGenerator::global()->generate());
        {
            QSqlDatabase db = createConnection(cx);
            if (!db.open()) {
                const auto err = QString("Failed to open database: %1").arg(db.lastError().text());
                m_logger->LogError(err);
                return Etrek::Specification::Result<bool>::Failure(err);
            }

            QSqlQuery q(db);
            q.prepare("DELETE FROM ris_procedure_mapping WHERE id = :id");
            q.bindValue(":id", id);

            if (!q.exec()) {
                const auto err = QString("Failed to delete RIS procedure mapping: %1").arg(q.lastError().text());
                m_logger->LogError(err);
                return Etrek::Specification::Result<bool>::Failure(err);
            }

            db.close();
        }
        QSqlDatabase::removeDatabase(cx);
        return Etrek::Specification::Result<bool>::Success(true);
    }

    std::optional<ent::RisProcedureMapping> RisProcedureMappingRepository::getById(int id) const
    {
        const QString cx = "ris_mapping_getbyid_" + QString::number(QRandomGenerator::global()->generate());
        std::optional<ent::RisProcedureMapping> result;
        {
            QSqlDatabase db = createConnection(cx);
            if (!db.open()) {
                m_logger->LogError(QString("Failed to open database: %1").arg(db.lastError().text()));
                return result;
            }

            QSqlQuery q(db);
            q.prepare("SELECT * FROM ris_procedure_mapping WHERE id = :id");
            q.bindValue(":id", id);

            if (q.exec() && q.next()) {
                result = mapRowToEntity(q);
            }

            db.close();
        }
        QSqlDatabase::removeDatabase(cx);
        return result;
    }

    QVector<ent::RisProcedureMapping> RisProcedureMappingRepository::getByConnectionName(
        const QString& connectionName) const
    {
        QVector<ent::RisProcedureMapping> mappings;
        const QString cx = "ris_mapping_getbyconn_" + QString::number(QRandomGenerator::global()->generate());
        {
            QSqlDatabase db = createConnection(cx);
            if (!db.open()) {
                m_logger->LogError(QString("Failed to open database: %1").arg(db.lastError().text()));
                return mappings;
            }

            QSqlQuery q(db);
            q.prepare("SELECT * FROM ris_procedure_mapping WHERE connection_name = :connection_name ORDER BY external_code");
            q.bindValue(":connection_name", connectionName);

            if (q.exec()) {
                while (q.next()) {
                    mappings.push_back(mapRowToEntity(q));
                }
            }

            db.close();
        }
        QSqlDatabase::removeDatabase(cx);
        return mappings;
    }

    QVector<ent::RisProcedureMapping> RisProcedureMappingRepository::getAllActive() const
    {
        QVector<ent::RisProcedureMapping> mappings;
        const QString cx = "ris_mapping_getallactive_" + QString::number(QRandomGenerator::global()->generate());
        {
            QSqlDatabase db = createConnection(cx);
            if (!db.open()) {
                m_logger->LogError(QString("Failed to open database: %1").arg(db.lastError().text()));
                return mappings;
            }

            QSqlQuery q(db);
            q.prepare("SELECT * FROM ris_procedure_mapping WHERE is_active = TRUE ORDER BY connection_name, external_code");

            if (q.exec()) {
                while (q.next()) {
                    mappings.push_back(mapRowToEntity(q));
                }
            }

            db.close();
        }
        QSqlDatabase::removeDatabase(cx);
        return mappings;
    }

    std::optional<int> RisProcedureMappingRepository::findViewIdByExternalCode(
        const QString& connectionName,
        const QString& externalCode) const
    {
        auto mapping = findByExternalCode(connectionName, externalCode);
        if (mapping.has_value()) {
            return mapping->InternalViewId;
        }
        return std::nullopt;
    }

    std::optional<ent::RisProcedureMapping> RisProcedureMappingRepository::findByExternalCode(
        const QString& connectionName,
        const QString& externalCode) const
    {
        const QString cx = "ris_mapping_findbycode_" + QString::number(QRandomGenerator::global()->generate());
        std::optional<ent::RisProcedureMapping> result;
        {
            QSqlDatabase db = createConnection(cx);
            if (!db.open()) {
                m_logger->LogError(QString("Failed to open database: %1").arg(db.lastError().text()));
                return result;
            }

            QSqlQuery q(db);
            q.prepare(R"(
                SELECT * FROM ris_procedure_mapping
                WHERE connection_name = :connection_name
                  AND external_code = :external_code
                  AND is_active = TRUE
                LIMIT 1
            )");
            q.bindValue(":connection_name", connectionName);
            q.bindValue(":external_code", externalCode);

            if (q.exec() && q.next()) {
                result = mapRowToEntity(q);
            }

            db.close();
        }
        QSqlDatabase::removeDatabase(cx);
        return result;
    }

} // namespace Etrek::Worklist::Repository
