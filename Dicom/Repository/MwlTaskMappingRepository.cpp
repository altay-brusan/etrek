#include "MwlTaskMappingRepository.h"
#include "LoggerProvider.h"
#include <QSqlQuery>
#include <QSqlError>
#include <QRandomGenerator>

namespace Etrek::Dicom::Repository {

    using Etrek::Specification::Result;
    using Etrek::Dicom::Data::Entity::MwlTaskMapping;
    using Etrek::Dicom::Data::Entity::DicomChain;

    MwlTaskMappingRepository::MwlTaskMappingRepository(
        std::shared_ptr<Etrek::Core::Data::Model::DatabaseConnectionSetting> connectionSetting,
        QObject* parent)
        : QObject(parent)
        , m_connectionSetting(std::move(connectionSetting))
        , m_logger(Etrek::Core::Log::LoggerProvider::Instance().GetLogger("MwlTaskMappingRepository"))
    {
    }

    QSqlDatabase MwlTaskMappingRepository::createConnection(const QString& connectionName) const
    {
        QSqlDatabase db = QSqlDatabase::addDatabase("QMYSQL", connectionName);
        db.setHostName(m_connectionSetting->Host);
        db.setPort(m_connectionSetting->Port);
        db.setDatabaseName(m_connectionSetting->DatabaseName);
        db.setUserName(m_connectionSetting->Username);
        db.setPassword(m_connectionSetting->Password);
        return db;
    }

    MwlTaskMapping MwlTaskMappingRepository::mapRowToEntity(const QSqlQuery& query) const
    {
        MwlTaskMapping mapping;
        mapping.MwlEntryId = query.value("mwl_entry_id").toInt();
        mapping.ProcedureId = query.value("procedure_id").toInt();
        mapping.StudyId = query.value("study_id").toInt();
        mapping.SeriesId = query.value("series_id").toInt();
        mapping.ImagesId = query.value("images_id").toInt();
        mapping.SopCommonId = query.value("sop_common_id").toInt();
        mapping.AcquisitionId = query.value("acquisition_id").toInt();
        return mapping;
    }

    Result<bool> MwlTaskMappingRepository::createMapping(const MwlTaskMapping& mapping)
    {
        const QString cx = "mwl_task_create_" + QString::number(QRandomGenerator::global()->generate());
        {
            QSqlDatabase db = createConnection(cx);
            if (!db.open()) {
                const auto err = QString("Failed to open database: %1").arg(db.lastError().text());
                m_logger->LogError(err);
                return Result<bool>::Failure(err);
            }

            QSqlQuery q(db);
            q.prepare(R"(
                INSERT INTO mwl_task_mapping (
                    mwl_entry_id, procedure_id, study_id, series_id,
                    images_id, sop_common_id, acquisition_id
                ) VALUES (
                    :mwl_entry_id, :procedure_id, :study_id, :series_id,
                    :images_id, :sop_common_id, :acquisition_id
                )
            )");

            q.bindValue(":mwl_entry_id", mapping.MwlEntryId);
            q.bindValue(":procedure_id", mapping.ProcedureId);
            q.bindValue(":study_id", mapping.StudyId);
            q.bindValue(":series_id", mapping.SeriesId);
            q.bindValue(":images_id", mapping.ImagesId);
            q.bindValue(":sop_common_id", mapping.SopCommonId);
            q.bindValue(":acquisition_id", mapping.AcquisitionId);

            if (!q.exec()) {
                const auto err = QString("Failed to create MWL task mapping: %1").arg(q.lastError().text());
                m_logger->LogError(err);
                return Result<bool>::Failure(err);
            }

            db.close();
        }
        QSqlDatabase::removeDatabase(cx);
        return Result<bool>::Success(true);
    }

    QVector<MwlTaskMapping> MwlTaskMappingRepository::getMappingsByMwlEntry(int mwlEntryId) const
    {
        QVector<MwlTaskMapping> mappings;
        const QString cx = "mwl_task_getbymwl_" + QString::number(QRandomGenerator::global()->generate());
        {
            QSqlDatabase db = createConnection(cx);
            if (!db.open()) {
                m_logger->LogError(QString("Failed to open database: %1").arg(db.lastError().text()));
                return mappings;
            }

            QSqlQuery q(db);
            q.prepare("SELECT * FROM mwl_task_mapping WHERE mwl_entry_id = :mwl_entry_id ORDER BY procedure_id");
            q.bindValue(":mwl_entry_id", mwlEntryId);

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

    std::optional<MwlTaskMapping> MwlTaskMappingRepository::getMappingByStudy(int studyId) const
    {
        const QString cx = "mwl_task_getbystudy_" + QString::number(QRandomGenerator::global()->generate());
        std::optional<MwlTaskMapping> result;
        {
            QSqlDatabase db = createConnection(cx);
            if (!db.open()) {
                m_logger->LogError(QString("Failed to open database: %1").arg(db.lastError().text()));
                return result;
            }

            QSqlQuery q(db);
            q.prepare("SELECT * FROM mwl_task_mapping WHERE study_id = :study_id LIMIT 1");
            q.bindValue(":study_id", studyId);

            if (q.exec() && q.next()) {
                result = mapRowToEntity(q);
            }

            db.close();
        }
        QSqlDatabase::removeDatabase(cx);
        return result;
    }

    std::optional<MwlTaskMapping> MwlTaskMappingRepository::getMappingByKey(int mwlEntryId, int procedureId) const
    {
        const QString cx = "mwl_task_getbykey_" + QString::number(QRandomGenerator::global()->generate());
        std::optional<MwlTaskMapping> result;
        {
            QSqlDatabase db = createConnection(cx);
            if (!db.open()) {
                m_logger->LogError(QString("Failed to open database: %1").arg(db.lastError().text()));
                return result;
            }

            QSqlQuery q(db);
            q.prepare(R"(
                SELECT * FROM mwl_task_mapping
                WHERE mwl_entry_id = :mwl_entry_id AND procedure_id = :procedure_id
            )");
            q.bindValue(":mwl_entry_id", mwlEntryId);
            q.bindValue(":procedure_id", procedureId);

            if (q.exec() && q.next()) {
                result = mapRowToEntity(q);
            }

            db.close();
        }
        QSqlDatabase::removeDatabase(cx);
        return result;
    }

    std::optional<DicomChain> MwlTaskMappingRepository::getFullChainByMwl(int mwlEntryId, int procedureId) const
    {
        auto mapping = getMappingByKey(mwlEntryId, procedureId);
        if (!mapping.has_value()) {
            return std::nullopt;
        }

        DicomChain chain;
        chain.MwlEntryId = mapping->MwlEntryId;
        chain.ProcedureId = mapping->ProcedureId;
        chain.StudyId = mapping->StudyId;
        chain.SeriesId = mapping->SeriesId;
        chain.ImageId = mapping->ImagesId;
        chain.SopCommonId = mapping->SopCommonId;
        chain.AcquisitionId = mapping->AcquisitionId;
        return chain;
    }

    Result<bool> MwlTaskMappingRepository::updateMapping(const MwlTaskMapping& mapping)
    {
        const QString cx = "mwl_task_update_" + QString::number(QRandomGenerator::global()->generate());
        {
            QSqlDatabase db = createConnection(cx);
            if (!db.open()) {
                const auto err = QString("Failed to open database: %1").arg(db.lastError().text());
                m_logger->LogError(err);
                return Result<bool>::Failure(err);
            }

            QSqlQuery q(db);
            q.prepare(R"(
                UPDATE mwl_task_mapping SET
                    study_id = :study_id,
                    series_id = :series_id,
                    images_id = :images_id,
                    sop_common_id = :sop_common_id,
                    acquisition_id = :acquisition_id
                WHERE mwl_entry_id = :mwl_entry_id AND procedure_id = :procedure_id
            )");

            q.bindValue(":mwl_entry_id", mapping.MwlEntryId);
            q.bindValue(":procedure_id", mapping.ProcedureId);
            q.bindValue(":study_id", mapping.StudyId);
            q.bindValue(":series_id", mapping.SeriesId);
            q.bindValue(":images_id", mapping.ImagesId);
            q.bindValue(":sop_common_id", mapping.SopCommonId);
            q.bindValue(":acquisition_id", mapping.AcquisitionId);

            if (!q.exec()) {
                const auto err = QString("Failed to update MWL task mapping: %1").arg(q.lastError().text());
                m_logger->LogError(err);
                return Result<bool>::Failure(err);
            }

            db.close();
        }
        QSqlDatabase::removeDatabase(cx);
        return Result<bool>::Success(true);
    }

    Result<bool> MwlTaskMappingRepository::deleteMapping(int mwlEntryId, int procedureId)
    {
        const QString cx = "mwl_task_delete_" + QString::number(QRandomGenerator::global()->generate());
        {
            QSqlDatabase db = createConnection(cx);
            if (!db.open()) {
                const auto err = QString("Failed to open database: %1").arg(db.lastError().text());
                m_logger->LogError(err);
                return Result<bool>::Failure(err);
            }

            QSqlQuery q(db);
            q.prepare("DELETE FROM mwl_task_mapping WHERE mwl_entry_id = :mwl_entry_id AND procedure_id = :procedure_id");
            q.bindValue(":mwl_entry_id", mwlEntryId);
            q.bindValue(":procedure_id", procedureId);

            if (!q.exec()) {
                const auto err = QString("Failed to delete MWL task mapping: %1").arg(q.lastError().text());
                m_logger->LogError(err);
                return Result<bool>::Failure(err);
            }

            db.close();
        }
        QSqlDatabase::removeDatabase(cx);
        return Result<bool>::Success(true);
    }

} // namespace Etrek::Dicom::Repository
