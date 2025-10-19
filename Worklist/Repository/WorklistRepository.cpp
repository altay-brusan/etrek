#include "WorklistRepository.h"
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include <QRandomGenerator>
#include <QDebug>

#include "WorklistEnum.h"
#include "AppLoggerFactory.h"
#include "MessageKey.h"
#include "DatabaseConnectionSetting.h"
#include "TranslationProvider.h"
#include "WorklistProfile.h"
#include "Result.h"
#include "DicomTag.h"
#include "WorklistEntry.h"

namespace Etrek::Worklist::Repository {

    using Etrek::Specification::Result;
    using namespace Etrek::Core::Data::Model;
	using namespace Etrek::Worklist::Data::Entity;
	using namespace Etrek::Core::Globalization;
    using namespace Etrek::Core::Log;
    using namespace Etrek::Specification;

    WorklistRepository::WorklistRepository(std::shared_ptr<DatabaseConnectionSetting> connectionSetting, QObject* parent)
        : QObject(parent), m_connectionSetting(connectionSetting), translator(nullptr), logger(nullptr)
    {

        translator = &TranslationProvider::Instance();
        AppLoggerFactory factory(LoggerProvider::Instance(), translator);
        logger = factory.CreateLogger("WorklistRepository");
    }

    QSqlDatabase WorklistRepository::createConnection(const QString& connectionName) const {
        QSqlDatabase db = QSqlDatabase::addDatabase("QMYSQL", connectionName);
        db.setHostName(m_connectionSetting->getHostName());
        db.setDatabaseName(m_connectionSetting->getDatabaseName());
        db.setUserName(m_connectionSetting->getEtrekUserName());
        db.setPassword(m_connectionSetting->getPassword());
        db.setPort(m_connectionSetting->getPort());
        return db;
    }

    Result<QList<WorklistProfile>> WorklistRepository::getProfiles() const {
        QList<WorklistProfile> profiles;

        QString connName = "conn_get_profiles_" + QString::number(QRandomGenerator::global()->generate());
        {
            QSqlDatabase db = createConnection(connName);

            if (!db.open()) {
                QString error = translator->getErrorMessage(FAILED_TO_OPEN_DB_MSG).arg(db.lastError().text());
                logger->LogError(error);
                qDebug()<<error;
                return Result<QList<WorklistProfile>>::Failure(error);
            }

            // Fetch profiles along with associated presentation context and tags
            QSqlQuery query(db);
            query.prepare(R"(
                SELECT p.id AS profile_id, p.name AS profile_name,
                       pc.id AS context_id, pc.transfer_syntax_uid,
                       t.id AS tag_id, t.name AS tag_name  -- Added tag_id and tag_name
                FROM mwl_profiles p
                LEFT JOIN mwl_presentation_contexts pc ON p.id = pc.profile_id
                LEFT JOIN profile_tag_association pta ON p.id = pta.profile_id
                LEFT JOIN dicom_tags t ON pta.tag_id = t.id
            )");

            if (!query.exec()) {
                QString error = translator->getErrorMessage(MWL_QUERY_FAILED_MSG).arg(query.lastError().text());
                logger->LogError(error);
                qDebug()<<error;
                return Result<QList<WorklistProfile>>::Failure(error);
            }

            // Create a map to track which profiles we've already processed
            QMap<int, WorklistProfile> profilesMap;

            while (query.next()) {
                int profileId = query.value("profile_id").toInt();

                // Check if this profile is already in the map
                if (!profilesMap.contains(profileId)) {
                    WorklistProfile profile;
                    profile.Id = profileId;
                    profile.Name = query.value("profile_name").toString();

                    // Set the context for the profile
                    WorklistPresentationContext context;
                    context.Id = query.value("context_id").toInt();
                    context.TransferSyntaxUid = query.value("transfer_syntax_uid").toString();

                    // Assign the context to the profile
                    profile.Context = context;

                    // Add the profile to the map
                    profilesMap[profileId] = profile;
                }

                // Add associated tags for the profile (we can do this in a separate loop)
                if (query.value("tag_id").isValid()) {
                    ProfileTagAssociation association;
                    association.Tag.Id = query.value("tag_id").toInt();
                    association.Tag.Name = query.value("tag_name").toString();

                    // Add the tag to the corresponding profile in the map
                    profilesMap[profileId].Tags.append(association);
                }
            }

            // Now convert the map of profiles to a list
            profiles = profilesMap.values();
        }
        QSqlDatabase::removeDatabase(connName); // ✅ SAFE — all references destroyed
        return Result<QList<WorklistProfile>>::Success(profiles);
    }

    Result<QList<DicomTag>> WorklistRepository::getTagsByProfile(int profileId) const {
        QList<DicomTag> tags;
        QString connName = "conn_get_tags_" + QString::number(QRandomGenerator::global()->generate());
        {
            QSqlDatabase db = createConnection(connName);

            if (!db.open()) {
                QString error = translator->getErrorMessage(FAILED_TO_OPEN_DB_MSG).arg(db.lastError().text());
                logger->LogError(error);
                qDebug()<<error;
                return Result<QList<DicomTag>>::Failure(error);
            }

            QSqlQuery query(db);
            query.prepare(R"(
                SELECT t.id, t.name, t.display_name, t.group_hex, t.element_hex, t.pgroup_hex, t.pelement_hex, t.is_active, t.is_retired
                FROM dicom_tags t
                JOIN profile_tag_association pta ON t.id = pta.tag_id
                WHERE pta.profile_id = :profileId
            )");
            query.bindValue(":profileId", profileId);

            if (!query.exec()) {
                QString error = translator->getErrorMessage(MWL_QUERY_FAILED_MSG).arg(query.lastError().text());
                logger->LogError(error);
                qDebug()<<error;
                return Result<QList<DicomTag>>::Failure(error);
            }

            while (query.next()) {
                DicomTag tag;
                tag.Id = query.value("id").toInt();
                tag.Name = query.value("name").toString();
                tag.DisplayName = query.value("display_name").toString();
                tag.GroupHex = query.value("group_hex").toUInt();
                tag.ElementHex = query.value("element_hex").toUInt();
                tag.PgroupHex = query.value("pgroup_hex").toUInt();
                tag.PelementHex = query.value("pelement_hex").toUInt();
                tag.IsActive = query.value("is_active").toBool();
                tag.IsRetired = query.value("is_retired").toBool();
                tags.append(tag);
            }
        }
        QSqlDatabase::removeDatabase(connName);
        return Result<QList<DicomTag>>::Success(tags);
    }

    Result<QList<DicomTag>> WorklistRepository::getIdentifiersByProfile(int profileId) const {
        QList<DicomTag> tags;
        QString connName = "conn_get_tags_" + QString::number(QRandomGenerator::global()->generate());
        {
            QSqlDatabase db = createConnection(connName);

            if (!db.open()) {
                QString error = translator->getErrorMessage(FAILED_TO_OPEN_DB_MSG).arg(db.lastError().text());
                logger->LogError(error);
                qDebug()<<error;
                return Result<QList<DicomTag>>::Failure(error);
            }

            QSqlQuery query(db);
            query.prepare(R"(
                SELECT t.id, t.name, t.display_name, t.group_hex, t.element_hex, t.pgroup_hex, t.pelement_hex, t.is_active, t.is_retired
                FROM dicom_tags t
                JOIN profile_tag_association pta ON t.id = pta.tag_id
                WHERE pta.profile_id = :profileId AND  pta.is_identifier =1
            )");
            query.bindValue(":profileId", profileId);

            if (!query.exec()) {
                QString error = translator->getErrorMessage(MWL_QUERY_FAILED_MSG).arg(query.lastError().text());
                logger->LogError(error);
                qDebug()<<error;
                return Result<QList<DicomTag>>::Failure(error);
            }

            while (query.next()) {
                DicomTag tag;
                tag.Id = query.value("id").toInt();
                tag.Name = query.value("name").toString();
                tag.DisplayName = query.value("display_name").toString();
                tag.GroupHex = query.value("group_hex").toUInt();
                tag.ElementHex = query.value("element_hex").toUInt();
                tag.PgroupHex = query.value("pgroup_hex").toUInt();
                tag.PelementHex = query.value("pelement_hex").toUInt();
                tag.IsActive = query.value("is_active").toBool();
                tag.IsRetired = query.value("is_retired").toBool();
                tags.append(tag);
            }
        }
        QSqlDatabase::removeDatabase(connName);
        return Result<QList<DicomTag>>::Success(tags);
    }

    Result<WorklistEntry> WorklistRepository::getWorklistEntryById(int entryId) const {
        QString connName = "conn_get_worklist_" + QString::number(QRandomGenerator::global()->generate());
        WorklistEntry entry;
        {
            QSqlDatabase db = createConnection(connName);

            if (!db.open()) {
                QString error = translator->getErrorMessage(FAILED_TO_OPEN_DB_MSG).arg(db.lastError().text());
                logger->LogError(error);
                qDebug()<<error;
                return Result<WorklistEntry>::Failure(error);
            }

            QSqlQuery query(db);
            query.prepare(R"(
                SELECT id, source, profile_id, status, created_at, updated_at
                FROM mwl_entries
                WHERE id = :entryId
            )");
            query.bindValue(":entryId", entryId);

            if (!query.exec() || !query.next()) {
                QString error = translator->getErrorMessage(MWL_ENTRY_NOT_FOUND_OR_QUERY_FAILED_MSG);
                logger->LogError(error);
                qDebug()<<error;
                return Result<WorklistEntry>::Failure(error);
            }

            entry.Id = query.value("id").toInt();
            QString sourceString = query.value("source").toString();
            entry.Source = QStringToSource(sourceString);
            entry.Profile.Id = query.value("profile_id").toInt();
            QString statusString = query.value("status").toString();
            entry.Status = QStringToStatus(statusString);
            entry.CreatedAt = query.value("created_at").toDateTime();
            entry.UpdatedAt = query.value("updated_at").toDateTime();

            QSqlQuery attrQuery(db);
            attrQuery.prepare(R"(
                SELECT wa.id, wa.tag_value, t.id AS tag_id, t.name, t.display_name, t.group_hex, t.element_hex, t.pgroup_hex, t.pelement_hex, t.is_active, t.is_retired
                FROM mwl_attributes wa
                JOIN dicom_tags t ON wa.dicom_tag_id = t.id
                WHERE wa.mwl_entry_id = :entryId
            )");
            attrQuery.bindValue(":entryId", entryId);

            if (!attrQuery.exec()) {
                QString error = translator->getErrorMessage(MWL_FAILED_TO_LOAD_ATTRIBUTES_MSG);
                logger->LogError(error);
                qDebug()<<error;
                return Result<WorklistEntry>::Failure(error);
            }

            while (attrQuery.next()) {
                WorklistAttribute attr;
                attr.id = attrQuery.value("id").toInt();
                attr.EntryId = entryId;
                attr.Tag.Id = attrQuery.value("tag_id").toInt();
                attr.Tag.Name = attrQuery.value("name").toString();
                attr.Tag.DisplayName = attrQuery.value("display_name").toString();
                attr.Tag.GroupHex = attrQuery.value("group_hex").toUInt();
                attr.Tag.ElementHex = attrQuery.value("element_hex").toUInt();
                attr.Tag.PgroupHex = attrQuery.value("pgroup_hex").toUInt();
                attr.Tag.PelementHex = attrQuery.value("pelement_hex").toUInt();
                attr.Tag.IsActive = attrQuery.value("is_active").toBool();
                attr.Tag.IsRetired = attrQuery.value("is_retired").toBool();
                attr.TagValue = attrQuery.value("tag_value").toString();
                entry.Attributes.append(attr);
            }
        }

        QSqlDatabase::removeDatabase(connName);
        return Result<WorklistEntry>::Success(entry);
    }

    Result<QList<WorklistEntry>> WorklistRepository::getWorklistEntries(const QDateTime* from, const QDateTime* to) const {
        QList<WorklistEntry> entries;
        const QString connName = "conn_get_entries_" + QString::number(QRandomGenerator::global()->generate());

        {
            // This block ensures everything is destroyed before removeDatabase
            QSqlDatabase db = createConnection(connName);
            if (!db.open()) {
                QString error = translator->getErrorMessage(FAILED_TO_OPEN_DB_MSG).arg(db.lastError().text());
                logger->LogError(error);
                qDebug()<<error;
                return Result<QList<WorklistEntry>>::Failure(error);
            }

            QString sql = R"(SELECT id, source, profile_id, status, created_at, updated_at FROM mwl_entries WHERE 1=1 )";

            if (from && from->isValid()) {
                sql += " AND created_at >= :from ";
            }
            if (to && to->isValid()) {
                sql += " AND created_at <= :to ";
            }

            QSqlQuery query(db);
            query.prepare(sql);

            if (from && from->isValid()) query.bindValue(":from", *from);
            if (to && to->isValid()) query.bindValue(":to", *to);

            if (!query.exec()) {
                QString error = translator->getErrorMessage(MWL_QUERY_FAILED_MSG).arg(query.lastError().text());
                logger->LogError(error);
                qDebug()<<error;
                return Result<QList<WorklistEntry>>::Failure(error);
            }

            QList<int> entryIds;
            while (query.next()) {
                WorklistEntry entry;
                entry.Id = query.value("id").toInt();
                entry.Source = static_cast<Source>(query.value("source").toInt());
                entry.Profile.Id = query.value("profile_id").toInt();
                entry.Status = static_cast<ProcedureStepStatus>(query.value("status").toInt());
                entry.CreatedAt = query.value("created_at").toDateTime();
                entry.UpdatedAt = query.value("updated_at").toDateTime();

                entries.append(entry);
                entryIds.append(entry.Id);
            }

            auto attributesMap = loadAttributesForEntries(entryIds, db);

            for (auto& entry : entries) {
                if (attributesMap.contains(entry.Id)) {
                    entry.Attributes = attributesMap[entry.Id];
                }
            }
        }

        QSqlDatabase::removeDatabase(connName);  // ✅ safe: all objects referencing the db are destroyed

        return Result<QList<WorklistEntry>>::Success(entries);
    }

    Result<QList<WorklistEntry>> WorklistRepository::getWorklistEntries(Source source) const {
        QList<WorklistEntry> entries;
        QString connName = "conn_get_entries_src_" + QString::number(QRandomGenerator::global()->generate());
        {
            QSqlDatabase db = createConnection(connName);

            if (!db.open()) {
                QString error = translator->getErrorMessage(FAILED_TO_OPEN_DB_MSG).arg(db.lastError().text());
                logger->LogError(error);
                qDebug()<<error;
                return Result<QList< WorklistEntry>>::Failure(error);
            }

            QSqlQuery query(db);
            query.prepare(R"(
                SELECT id, source, profile_id, status, created_at, updated_at
                FROM mwl_entries
                WHERE source = :source
            )");
            query.bindValue(":source", static_cast<int>(source));

            if (!query.exec()) {
                QString error = translator->getErrorMessage(MWL_QUERY_FAILED_MSG).arg(query.lastError().text());
                logger->LogError(error);
                qDebug()<<error;
                return Result<QList< WorklistEntry>>::Failure(error);
            }

            QList<int> entryIds;
            while (query.next()) {
                 WorklistEntry entry;
                entry.Id = query.value("id").toInt();
                entry.Source = static_cast<Source>(query.value("source").toInt());
                entry.Profile.Id = query.value("profile_id").toInt();
                entry.Status = static_cast<ProcedureStepStatus>(query.value("status").toInt());
                entry.CreatedAt = query.value("created_at").toDateTime();
                entry.UpdatedAt = query.value("updated_at").toDateTime();

                entries.append(entry);
                entryIds.append(entry.Id);
            }

            // Batch load attributes for all entries
            auto attributesMap = loadAttributesForEntries(entryIds, db);

            // Assign attributes to each entry
            for (auto& entry : entries) {
                if (attributesMap.contains(entry.Id)) {
                    entry.Attributes = attributesMap[entry.Id];
                }
            }
        }
        QSqlDatabase::removeDatabase(connName);
        return Result<QList< WorklistEntry>>::Success(entries);
    }

    Result<QList< WorklistEntry>> WorklistRepository::getWorklistEntries(ProcedureStepStatus status) const {
        QList< WorklistEntry> entries;
        QString connName = "conn_get_entries_status_" + QString::number(QRandomGenerator::global()->generate());
        {
            QSqlDatabase db = createConnection(connName);

            if (!db.open()) {
                QString error = translator->getErrorMessage(FAILED_TO_OPEN_DB_MSG).arg(db.lastError().text());
                logger->LogError(error);
                qDebug()<<error;
                return Result<QList< WorklistEntry>>::Failure(error);
            }

            QSqlQuery query(db);
            query.prepare(R"(
                SELECT id, source, profile_id, status, created_at, updated_at
                FROM mwl_entries
                WHERE status = :status
            )");
            query.bindValue(":status", static_cast<int>(status));

            if (!query.exec()) {
                QString error = translator->getErrorMessage(MWL_QUERY_FAILED_MSG).arg(query.lastError().text());
                logger->LogError(error);
                qDebug()<<error;
                return Result<QList< WorklistEntry>>::Failure(error);
            }

            QList<int> entryIds;
            while (query.next()) {
                 WorklistEntry entry;
                entry.Id = query.value("id").toInt();
                entry.Source = static_cast<Source>(query.value("source").toInt());
                entry.Profile.Id = query.value("profile_id").toInt();
                entry.Status = static_cast<ProcedureStepStatus>(query.value("status").toInt());
                entry.CreatedAt = query.value("created_at").toDateTime();
                entry.UpdatedAt = query.value("updated_at").toDateTime();

                entries.append(entry);
                entryIds.append(entry.Id);
            }

            // Batch load attributes for all entries
            auto attributesMap = loadAttributesForEntries(entryIds, db);

            // Assign attributes to each entry
            for (auto& entry : entries) {
                if (attributesMap.contains(entry.Id)) {
                    entry.Attributes = attributesMap[entry.Id];
            }
        }

        }
        QSqlDatabase::removeDatabase(connName);
        return Result<QList< WorklistEntry>>::Success(entries);
    }

    Result<bool> WorklistRepository::deleteWorklistEntries(const QDateTime& beforeDate) {
        QString connName = "conn_delete_before_date_" + QString::number(QRandomGenerator::global()->generate());

        QList<int> entryIds;

        {
            QSqlDatabase db = createConnection(connName);

            if (!db.open()) {
                QString error = translator->getErrorMessage(FAILED_TO_OPEN_DB_MSG).arg(db.lastError().text());
                logger->LogError(error);
                qDebug() << error;
                return Result<bool>::Failure(error);
            }

            if (!db.transaction()) {
                QString error = translator->getErrorMessage(MWL_FAILED_TO_START_TRANSACTION_MSG).arg(db.lastError().text());
                logger->LogError(error);
                qDebug() << error;
                return Result<bool>::Failure(error);
            }

            // Step 1: Collect IDs of entries to be deleted
            QSqlQuery selectIds(db);
            selectIds.prepare("SELECT id FROM mwl_entries WHERE created_at <= :beforeDate");
            selectIds.bindValue(":beforeDate", beforeDate);

            if (!selectIds.exec()) {
                QString error = translator->getErrorMessage(DB_FETCH_IDS_BEFORE_DELETE_FAILED_ERROR).arg(selectIds.lastError().text());
                logger->LogError(error);
                qDebug() << error;
                return Result<bool>::Failure(error);
            }

            while (selectIds.next()) {
                entryIds << selectIds.value(0).toInt();
            }

            // Step 2: Delete associated attributes
            QSqlQuery attrDelete(db);
            attrDelete.prepare(R"(
                DELETE wa FROM mwl_attributes wa
                JOIN mwl_entries e ON wa.mwl_entry_id = e.id
                WHERE e.created_at <= :beforeDate
            )");
            attrDelete.bindValue(":beforeDate", beforeDate);

            if (!attrDelete.exec()) {
                QString error = translator->getErrorMessage(MWL_FAILED_TO_DELETE_ATTRIBUTES_MSG).arg(attrDelete.lastError().text());
                logger->LogError(error);
                qDebug() << error;
                return Result<bool>::Failure(error);
            }

            // Step 3: Delete the entries themselves
            QSqlQuery entryDelete(db);
            entryDelete.prepare("DELETE FROM mwl_entries WHERE created_at <= :beforeDate");
            entryDelete.bindValue(":beforeDate", beforeDate);

            if (!entryDelete.exec()) {
                QString error = translator->getErrorMessage(MWL_FAILED_TO_DELETE_ENTRIES_MSG).arg(entryDelete.lastError().text());
                logger->LogError(error);
                qDebug() << error;
                return Result<bool>::Failure(error);
            }

            if (!db.commit()) {
                QString error = translator->getErrorMessage(DB_COMMIT_TRANSACTION_FAILED_ERROR).arg(db.lastError().text());
                logger->LogError(error);
                qDebug() << error;
                return Result<bool>::Failure(error);
            }
        }

        // Step 4: Emit deleted signal for each ID
        for (const int& id  : entryIds)
            emit worklistEntryDeleted(id);

        QSqlDatabase::removeDatabase(connName);
        return Result<bool>::Success(true);
    }


    Result<bool> WorklistRepository::deleteWorklistEntries(const QList<int>& entryIds) {

        if (entryIds.isEmpty()) {
            return Result<bool>::Success(true); // Nothing to delete
        }

        QString connName = "conn_delete_entries_" + QString::number(QRandomGenerator::global()->generate());

        {
            QSqlDatabase db = createConnection(connName);

            if (!db.open()) {
                QString error = translator->getErrorMessage(FAILED_TO_OPEN_DB_MSG).arg(db.lastError().text());
                logger->LogError(error);
                qDebug()<<error;
                return Result<bool>::Failure(error);
            }

            if (!db.transaction()) {
                QString error = translator->getErrorMessage(MWL_FAILED_TO_START_TRANSACTION_MSG).arg(db.lastError().text());
                logger->LogError(error);
                qDebug()<<error;
                return Result<bool>::Failure(error);
            }

            // Prepare placeholders for IN clause
            QStringList placeholders;
            for (int i = 0; i < entryIds.size(); ++i) {
                placeholders << "?";
            }
            QString inClause = placeholders.join(",");

            // Delete attributes linked to specified entries
            QSqlQuery attrDelete(db);
            QString attrSql = QString("DELETE FROM mwl_attributes WHERE mwl_entry_id IN (%1)").arg(inClause);
            attrDelete.prepare(attrSql);

            for (int i = 0; i < entryIds.size(); ++i) {
                attrDelete.bindValue(i, entryIds[i]);
            }

            if (!attrDelete.exec()) {
                QString error = translator->getErrorMessage(MWL_FAILED_TO_DELETE_ATTRIBUTES_MSG).arg(attrDelete.lastError().text());
                logger->LogError(error);
                qDebug()<<error;
                return Result<bool>::Failure(error);
            }

            // Delete entries themselves
            QSqlQuery entryDelete(db);
            QString entrySql = QString("DELETE FROM mwl_entries WHERE id IN (%1)").arg(inClause);
            entryDelete.prepare(entrySql);

            for (int i = 0; i < entryIds.size(); ++i) {
                entryDelete.bindValue(i, entryIds[i]);
            }

            if (!entryDelete.exec()) {
                QString error = translator->getErrorMessage(MWL_FAILED_TO_DELETE_ENTRIES_MSG).arg(entryDelete.lastError().text());
                logger->LogError(error);
                qDebug()<<error;
                return Result<bool>::Failure(error);
            }

            if (!db.commit()) {
                QString error = translator->getErrorMessage(MWL_FAILED_TO_COMMIT_TRANSACTION_MSG).arg(db.lastError().text());
                logger->LogError(error);
                qDebug()<<error;
                return Result<bool>::Failure(error);
            }

            for (const int& id  : entryIds)
                emit worklistEntryDeleted(id);
        }

        QSqlDatabase::removeDatabase(connName);
        return Result<bool>::Success(true);
    }

    Result<int> WorklistRepository::addDicomTag(const  DicomTag& tag) {
        int newId = -1;
        QString connName = "conn_add_dicom_tag_" + QString::number(QRandomGenerator::global()->generate());

        {
            QSqlDatabase db = createConnection(connName);

            if (!db.open()) {
                QString error = translator->getErrorMessage(FAILED_TO_OPEN_DB_MSG).arg(db.lastError().text());
                logger->LogError(error);
                qDebug()<<error;
                return Result<int>::Failure(error);
            }

            QSqlQuery query(db);

            query.prepare(R"(
                INSERT INTO dicom_tags
                    (name, display_name, group_hex, element_hex, pgroup_hex, pelement_hex, is_active, is_retired)
                VALUES
                    (:name,:display_name , :groupHex, :elementHex, :pgroupHex, :pelementHex, :isActive, :isRetired)
            )");

            query.bindValue(":name", tag.Name);
            query.bindValue(":display_name", tag.DisplayName);
            query.bindValue(":groupHex", static_cast<quint32>(tag.GroupHex));
            query.bindValue(":elementHex", static_cast<quint32>(tag.ElementHex));
            query.bindValue(":pgroupHex", static_cast<quint32>(tag.PgroupHex));
            query.bindValue(":pelementHex", static_cast<quint32>(tag.PelementHex));
            query.bindValue(":isActive", tag.IsActive);
            query.bindValue(":isRetired", tag.IsRetired);

            if (!query.exec()) {
                QString error = translator->getErrorMessage(MWL_FAILED_TO_INSERT_TAG_MSG).arg(query.lastError().text());
                logger->LogError(error);
                qDebug()<<error;
                return Result<int>::Failure(error);
            }

            newId = query.lastInsertId().toInt();
        }
        QSqlDatabase::removeDatabase(connName);
        return Result<int>::Success(newId);
    }

    Result<bool> WorklistRepository::updateDicomTagActiveStatus(int tagId, bool isActive) {
        QString connName = "conn_update_active_" + QString::number(QRandomGenerator::global()->generate());

        {
            QSqlDatabase db = createConnection(connName);

            if (!db.open()) {
                QString error = translator->getErrorMessage(FAILED_TO_OPEN_DB_MSG).arg(db.lastError().text());
                logger->LogError(error);
                qDebug()<<error;
                return Result<bool>::Failure(error);
            }

            QSqlQuery query(db);
            query.prepare(R"(
                UPDATE dicom_tags
                SET is_active = :isActive
                WHERE id = :tagId
            )");

            query.bindValue(":isActive", isActive);
            query.bindValue(":tagId", tagId);

            if (!query.exec()) {
                QString error = translator->getErrorMessage(MWL_FAILED_TO_UPDATE_TAG_MSG).arg(query.lastError().text());
                logger->LogError(error);
                qDebug()<<error;
                return Result<bool>::Failure(error);
            }
        }

        QSqlDatabase::removeDatabase(connName);
        return Result<bool>::Success(true);
    }

    Result<bool> WorklistRepository::updateDicomTagRetiredStatus(int tagId, bool isRetired) {

        QString connName = "conn_update_retired_" + QString::number(QRandomGenerator::global()->generate());

        {
            QSqlDatabase db = createConnection(connName);

            if (!db.open()) {
                QString error = translator->getErrorMessage(FAILED_TO_OPEN_DB_MSG).arg(db.lastError().text());
                logger->LogError(error);
                qDebug()<<error;
                return Result<bool>::Failure(error);
            }

            QSqlQuery query(db);
            query.prepare(R"(
                UPDATE dicom_tags
                SET is_retired = :isRetired
                WHERE id = :tagId
            )");

            query.bindValue(":isRetired", isRetired);
            query.bindValue(":tagId", tagId);

            if (!query.exec()) {
                QString error = translator->getErrorMessage(MWL_FAILED_TO_UPDATE_TAG_MSG).arg(query.lastError().text());
                logger->LogError(error);
                qDebug()<<error;
                return Result<bool>::Failure(error);
            }
        }

        QSqlDatabase::removeDatabase(connName);
        return Result<bool>::Success(true);
    }

    Result<QString> WorklistRepository::updateWorklistStatus(int entryId, ProcedureStepStatus newStatus) {
        QString connName = "conn_update_status_" + QString::number(QRandomGenerator::global()->generate());

        {
            QSqlDatabase db = createConnection(connName);

            if (!db.open()) {
                QString error = translator->getErrorMessage(FAILED_TO_OPEN_DB_MSG).arg(db.lastError().text());
                logger->LogError(error);
                qDebug()<<error;
                return Result<QString>::Failure(error);
            }

            QSqlQuery query(db);
            query.prepare("UPDATE mwl_entries SET status = :status WHERE id = :id");
            query.bindValue(":status", ProcedureStepStatusToString(newStatus));
            query.bindValue(":id", entryId);

            if (!query.exec()) {
                QString error = translator->getErrorMessage(MWL_FAILED_TO_UPDATE_ENTRIES_MSG).arg(query.lastError().text());
                logger->LogError(error);
                qDebug()<<error;
                return Result<QString>::Failure(error);
            }
        }

        QSqlDatabase::removeDatabase(connName);
        QString success = translator->getInfoMessage(MWL_ENTRY_UPDATE_SUCCEED_MSG);
        return Result<QString>::Success(success);
    }

    Result<int> WorklistRepository::createWorklistEntry(const  WorklistEntry& entry) {
        int newId = -1;
         WorklistEntry result;
        QString connName = "conn_create_worklist_" + QString::number(QRandomGenerator::global()->generate());

        {
            QSqlDatabase db = createConnection(connName);

            if (!db.open()) {
                QString error = translator->getErrorMessage(FAILED_TO_OPEN_DB_MSG).arg(db.lastError().text());
                logger->LogError(error);
                qDebug()<<error;
                return Result<int>::Failure(error);
            }

            if (!db.transaction()) {
                QString error = translator->getErrorMessage(MWL_FAILED_TO_START_TRANSACTION_MSG).arg(db.lastError().text());
                logger->LogError(error);
                qDebug()<<error;
                return Result<int>::Failure(error);
            }

            // Insert worklist entry
            QSqlQuery query(db);
            query.prepare(R"(
                INSERT INTO mwl_entries (source, profile_id, status, created_at, updated_at)
                VALUES (:source, :profileId, :status, :createdAt, :updatedAt)
            )");
            query.bindValue(":source", SourceToString(entry.Source));
            query.bindValue(":profileId", entry.Profile.Id);
            query.bindValue(":status", ProcedureStepStatusToString(entry.Status));
            query.bindValue(":createdAt", entry.CreatedAt);
            query.bindValue(":updatedAt", entry.UpdatedAt);

            if (!query.exec()) {
                db.rollback();
                QString error = translator->getErrorMessage(DB_INSERT_FAILED_ERROR).arg(query.lastError().text());
                logger->LogError(error);
                qDebug()<<error;
                return Result<int>::Failure(error);
            }

            newId = query.lastInsertId().toInt();

            // Insert associated tags (only active ones)
            QSqlQuery attrQuery(db);
            attrQuery.prepare(R"(
                INSERT INTO mwl_attributes (mwl_entry_id, dicom_tag_id, tag_value)
                VALUES (:entryId, :tagId, :tagValue)
            )");

            for (const  WorklistAttribute& attr : entry.Attributes) {
                if (attr.Tag.IsActive) {
                    attrQuery.bindValue(":entryId", newId);
                    attrQuery.bindValue(":tagId", attr.Tag.Id);
                    attrQuery.bindValue(":tagValue", attr.TagValue);

                    if (!attrQuery.exec()) {
                        db.rollback();
                        QString error = translator->getErrorMessage(DB_INSERT_ATTRIBUTE_FAILED_ERROR).arg(attrQuery.lastError().text());
                        logger->LogError(error);
                        qDebug()<<error;
                        return Result<int>::Failure(error);
                    }
                }
            }

            if (!db.commit()) {
                db.rollback();            
                QString error = translator->getErrorMessage(MWL_FAILED_TO_COMMIT_TRANSACTION_MSG).arg(db.lastError().text());
                logger->LogError(error);
                qDebug()<<error;
                return Result<int>::Failure(error);
            }
            result = entry;
            result.Id = newId;
        }
        emit worklistEntryCreated(static_cast<const  WorklistEntry&>(result));  // after commit
        QSqlDatabase::removeDatabase(connName);
        return Result<int>::Success(newId);
    }

    Result<int> WorklistRepository::updateWorklistEntry(const  WorklistEntry& entry) {
        QString connName = "conn_update_worklist_" + QString::number(QRandomGenerator::global()->generate());
        {
            QSqlDatabase db = createConnection(connName);

            if (!db.open()) {
                QString error = translator->getErrorMessage(FAILED_TO_OPEN_DB_MSG).arg(db.lastError().text());
                logger->LogError(error);
                qDebug()<<error;
                return Result<int>::Failure(error);
            }

            if (!db.transaction()) {
                QString error = translator->getErrorMessage(MWL_FAILED_TO_START_TRANSACTION_MSG).arg(db.lastError().text());
                logger->LogError(error);
                qDebug()<<error;
                return Result<int>::Failure(error);
            }

            // Update worklist entry
            QSqlQuery query(db);
            query.prepare(R"(
                UPDATE mwl_entries
                SET source = :source, profile_id = :profileId, status = :status, updated_at = :updatedAt
                WHERE id = :id
            )");
            query.bindValue(":source", SourceToString(entry.Source));
            query.bindValue(":profileId", entry.Profile.Id);
            query.bindValue(":status", ProcedureStepStatusToString(entry.Status));
            query.bindValue(":updatedAt", entry.UpdatedAt);
            query.bindValue(":id", entry.Id);

            if (!query.exec()) {
                db.rollback();
                QString error = translator->getErrorMessage(MWL_FAILED_TO_UPDATE_ENTRIES_MSG).arg(query.lastError().text());
                logger->LogError(error);
                qDebug()<<error;
                return Result<int>::Failure(error);
            }

            // Delete existing attributes
            QSqlQuery deleteQuery(db);
            deleteQuery.prepare("DELETE FROM mwl_attributes WHERE mwl_entry_id = :entryId");
            deleteQuery.bindValue(":entryId", entry.Id);

            if (!deleteQuery.exec()) {
                db.rollback();
                QString error = translator->getErrorMessage(MWL_FAILED_TO_DELETE_ATTRIBUTES_MSG).arg(deleteQuery.lastError().text());
                logger->LogError(error);
                qDebug()<<error;
                return Result<int>::Failure(error);
            }

            // Re-insert only active tags
            QSqlQuery attrQuery(db);
            attrQuery.prepare(R"(
                INSERT INTO mwl_attributes (mwl_entry_id, dicom_tag_id, tag_value)
                VALUES (:entryId, :tagId, :tagValue)
            )");

            for (const  WorklistAttribute& attr : entry.Attributes) {
                if (attr.Tag.IsActive) {
                    attrQuery.bindValue(":entryId", entry.Id);
                    attrQuery.bindValue(":tagId", attr.Tag.Id);
                    attrQuery.bindValue(":tagValue", attr.TagValue);

                    if (!attrQuery.exec()) {
                        db.rollback();
                        QString error = translator->getErrorMessage(DB_INSERT_ATTRIBUTE_FAILED_ERROR).arg(attrQuery.lastError().text());
                        logger->LogError(error);
                        qDebug()<<error;
                        return Result<int>::Failure(error);
                    }
                }
            }

            if (!db.commit()) {
                db.rollback();
                QString error = translator->getErrorMessage(MWL_FAILED_TO_COMMIT_TRANSACTION_MSG).arg(db.lastError().text());
                logger->LogError(error);
                qDebug()<<error;
                return Result<int>::Failure(error);
            }

        }

        QSqlDatabase::removeDatabase(connName);

        return Result<int>::Success(entry.Id);
    }

    Result< WorklistEntry> WorklistRepository::getWorklistEntry(const  WorklistEntry& entry) const {
        // Get active identifiers tags for profile
        auto activeTagsResult = getActiveIdentifierTags(entry.Profile.Id);
        if (!activeTagsResult.isSuccess) {
            QString error = translator->getWarningMessage(MWL_FAILED_TO_GET_ACTIVE_IDENTIFIERS_MSG).arg(activeTagsResult.message);
            logger->LogError(error);
            qDebug()<<error;
            return Result< WorklistEntry>::Failure(error);
        }

        // Build tagId to value map for provided entry only for active identifier tags
        QMap<int, QString> tagIdToValue;
        for (const auto& attr : entry.Attributes) {
            if (std::any_of(activeTagsResult.value.begin(), activeTagsResult.value.end(),
                            [&](const  DicomTag& tag) { return tag.Id == attr.Tag.Id; })) {
                tagIdToValue[attr.Tag.Id] = attr.TagValue;
            }
        }

        // If no active identifier tags were found for the entry, return empty result
        if (tagIdToValue.isEmpty()) {
            QString warning = translator->getWarningMessage(MWL_NO_ACTIVE_IDENTIFIER_FOUND_FOR_ENTRY_MSG);
            logger->LogError(warning);
            qDebug()<<warning;
            return Result< WorklistEntry>::Failure(warning);
        }

        // Search existing entry by identifiers
        auto findResult = findWorklistEntryByIdentifiers(entry.Profile.Id, tagIdToValue);
        if (!findResult.isSuccess) {
            QString error = translator->getErrorMessage(MWL_FAILED_TO_FIND_ENTRY_MSG).arg(findResult.message);
            logger->LogError(error);
            qDebug()<<error;
            return Result< WorklistEntry>::Failure(error);
        }

        // Now fetch the entry details (tags and attributes)
         WorklistEntry worklistEntry;
        worklistEntry.Id = findResult.value;

        // Fetch attributes and tags for the found entry
        auto entryWithDetails = getWorklistEntryDetails(worklistEntry.Id);
        if (!entryWithDetails.isSuccess) {
            return entryWithDetails;  // Return the error if fetching details failed
        }

        return Result< WorklistEntry>::Success(entryWithDetails.value);
    }

    Result< WorklistEntry> WorklistRepository::getWorklistEntry(const  WorklistEntry& entry, const  WorklistProfile& profile) const {
        // Get active identifiers tags for profile
        auto activeTagsResult = getActiveIdentifierTags(profile.Id);
        if (!activeTagsResult.isSuccess) {
            QString error = translator->getErrorMessage(MWL_FAILED_TO_GET_ACTIVE_IDENTIFIERS_MSG).arg(activeTagsResult.message);
            logger->LogError(error);
            qDebug()<<error;
            return Result< WorklistEntry>::Failure(error);
        }

        // Build tagId to value map for provided entry only for active identifier tags
        QMap<int, QString> tagIdToValue;
        for (const auto& attr : entry.Attributes) {
            if (std::any_of(activeTagsResult.value.begin(), activeTagsResult.value.end(),
                            [&](const  DicomTag& tag) { return tag.Id == attr.Tag.Id; })) {
                tagIdToValue[attr.Tag.Id] = attr.TagValue;
            }
        }

        // If no active identifier tags were found for the entry, return empty result
        if (tagIdToValue.isEmpty()) {
            QString warning = translator->getWarningMessage(MWL_NO_ACTIVE_IDENTIFIER_FOUND_FOR_ENTRY_MSG);
            logger->LogError(warning);
            qDebug()<<warning;
            return Result< WorklistEntry>::Failure(warning);
        }

        // Search existing entry by identifiers
        auto findResult = findWorklistEntryByIdentifiers(profile.Id, tagIdToValue);
        if (!findResult.isSuccess || findResult.value == -1) {
            QString error = translator->getErrorMessage(MWL_FAILED_TO_FIND_ENTRY_MSG).arg(findResult.message);
            logger->LogError(error);
            qDebug()<<error;
            return Result< WorklistEntry>::Failure(error);
        }

        // Now fetch the entry details (tags and attributes)
         WorklistEntry worklistEntry;
        worklistEntry.Id = findResult.value;

        // Fetch attributes and tags for the found entry
        auto entryWithDetails = getWorklistEntryDetails(worklistEntry.Id);
        if (!entryWithDetails.isSuccess) {
            return entryWithDetails;  // Return the error if fetching details failed
        }

        return Result< WorklistEntry>::Success(entryWithDetails.value);
    }

    Result<QList<DicomTag>> WorklistRepository::getActiveIdentifierTags(int profileId) const {
        QList<DicomTag> tags;
        QString connName = "conn_get_active_ids_" + QString::number(QRandomGenerator::global()->generate());

        {
            QSqlDatabase db = createConnection(connName);

            if (!db.open()) {
                QString error = translator->getErrorMessage(FAILED_TO_OPEN_DB_MSG).arg(db.lastError().text());
                logger->LogError(error);
                qDebug()<<error;
                return Result<QList<DicomTag>>::Failure(error);
            }

            QSqlQuery query(db);
            query.prepare(R"(
                SELECT t.id, t.name, t.display_name, t.group_hex, t.element_hex, t.pgroup_hex, t.pelement_hex, t.is_active, t.is_retired
                FROM dicom_tags t
                JOIN profile_tag_association pta ON t.id = pta.tag_id
                WHERE pta.profile_id = :profileId AND t.is_active = TRUE AND pta.is_identifier = TRUE
            )");
            query.bindValue(":profileId", profileId);

            if (!query.exec()) {
                QString error = translator->getErrorMessage(MWL_QUERY_FAILED_MSG).arg(query.lastError().text());
                logger->LogError(error);
                qDebug()<<error;
                return Result<QList<DicomTag>>::Failure(error);
            }

            while (query.next()) {
            DicomTag tag;
            tag.Id = query.value("id").toInt();
            tag.Name = query.value("name").toString();
            tag.DisplayName = query.value("display_name").toString();
            tag.GroupHex = query.value("group_hex").toUInt();
            tag.ElementHex = query.value("element_hex").toUInt();
            tag.PgroupHex = query.value("pgroup_hex").toUInt();
            tag.PelementHex = query.value("pelement_hex").toUInt();
            tag.IsActive = query.value("is_active").toBool();
            tag.IsRetired = query.value("is_retired").toBool();
            tags.append(tag);
            }
        }

        QSqlDatabase::removeDatabase(connName);
        return Result<QList<DicomTag>>::Success(tags);
    }

    Result<QList<DicomTag>> WorklistRepository::getMandatoryIdentifierTags(int profileId) const {
        QList<DicomTag> tags;
        QString connName = "conn_get_mandatory_ids_" + QString::number(QRandomGenerator::global()->generate());
        {
            QSqlDatabase db = createConnection(connName);
            if (!db.open()) {
                QString error = translator->getErrorMessage(FAILED_TO_OPEN_DB_MSG).arg(db.lastError().text());
                logger->LogError(error);
                qDebug()<<error;
                return Result<QList<DicomTag>>::Failure(error);
            }

            QSqlQuery query(db);
            query.prepare(R"(
            SELECT t.id, t.name, t.display_name, t.group_hex, t.element_hex, t.pgroup_hex, t.pelement_hex, t.is_active, t.is_retired
            FROM dicom_tags t
            JOIN profile_tag_association pta ON t.id = pta.tag_id
            WHERE pta.profile_id = :profileId AND pta.is_mandatory = TRUE
            )");
            query.bindValue(":profileId", profileId);

            if (!query.exec()) {
                QString error = translator->getErrorMessage(MWL_QUERY_FAILED_MSG).arg(query.lastError().text());
                logger->LogError(error);
                qDebug()<<error;
                return Result<QList<DicomTag>>::Failure(error);
            }

            while (query.next()) {
                DicomTag tag;
                tag.Id = query.value("id").toInt();
                tag.Name = query.value("name").toString();
                tag.DisplayName = query.value("display_name").toString();
                tag.GroupHex = query.value("group_hex").toUInt();
                tag.ElementHex = query.value("element_hex").toUInt();
                tag.PgroupHex = query.value("pgroup_hex").toUInt();
                tag.PelementHex = query.value("pelement_hex").toUInt();
                tag.IsActive = query.value("is_active").toBool();
                tag.IsRetired = query.value("is_retired").toBool();
                tags.append(tag);
            }
        }
        QSqlDatabase::removeDatabase(connName);
        return Result<QList<DicomTag>>::Success(tags);
    }

    Result<bool> WorklistRepository::updateIdentifierFlags(int profileId, int tagId, bool isIdentifier, bool isMandatoryIdentifier) {
        QString connName = "conn_update_flags_" + QString::number(QRandomGenerator::global()->generate());

        {
                QSqlDatabase db = createConnection(connName);
                if (!db.open()) {
                    QString error = translator->getErrorMessage(FAILED_TO_OPEN_DB_MSG).arg(db.lastError().text());
                    logger->LogError(error);
                    qDebug()<<error;
                    return Result<bool>::Failure(error);
                }

                QSqlQuery query(db);
                query.prepare(R"(
                    UPDATE profile_tag_association
                    SET is_identifier = :isIdentifier,
                        is_mandatory = :isMandatoryIdentifier
                    WHERE profile_id = :profileId AND tag_id = :tagId
                )");
                query.bindValue(":isIdentifier", isIdentifier);
                query.bindValue(":isMandatoryIdentifier", isMandatoryIdentifier);
                query.bindValue(":profileId", profileId);
                query.bindValue(":tagId", tagId);

                if (!query.exec()) {
                    QString error = translator->getErrorMessage(MWL_QUERY_FAILED_MSG).arg(query.lastError().text());
                    logger->LogError(error);
                    qDebug()<<error;
                    return Result<bool>::Failure(error);
                }
        }

        QSqlDatabase::removeDatabase(connName);
        return Result<bool>::Success(true);
    }

    Result<int> WorklistRepository::findWorklistEntryByIdentifiers(int profileId, const QMap<int, QString>& tagIdToValue) const {
        if (tagIdToValue.isEmpty()) {
            QString error = translator->getErrorMessage(DB_NO_IDENTIFIERS_PROVIDED_ERROR);
            logger->LogError(error);
            qDebug()<<error;
            return Result<int>::Failure(error);
        }

        QString connName = "conn_find_entry_" + QString::number(QRandomGenerator::global()->generate());

        {
            QSqlDatabase db = createConnection(connName);
            if (!db.open()) {
                QString error = translator->getErrorMessage(FAILED_TO_OPEN_DB_MSG).arg(db.lastError().text());
                logger->LogError(error);
                qDebug()<<error;
                return Result<int>::Failure(error);
            }

            QString sql = R"(
                SELECT e.id
                FROM mwl_entries e
                JOIN mwl_attributes a ON e.id = a.mwl_entry_id
                WHERE e.profile_id = :profileId
                  AND (
            )";

            // Dynamically build matching conditions
            QStringList conditions;
            int idx = 1;
            for (auto it = tagIdToValue.constBegin(); it != tagIdToValue.constEnd(); ++it) {
                conditions << QString("(a.dicom_tag_id = :tagId%1 AND LOWER(a.tag_value) = LOWER(:tagVal%1))").arg(idx);
                ++idx;
            }
            sql += conditions.join(" OR ") + ") ";

            // Group and count matches
            sql += R"(
                GROUP BY e.id
                HAVING COUNT(DISTINCT a.dicom_tag_id) = :tagCount
                LIMIT 1
            )";

            QSqlQuery query(db);
            query.prepare(sql);

            idx = 1;
            for (auto it = tagIdToValue.constBegin(); it != tagIdToValue.constEnd(); ++it) {
                query.bindValue(QString(":tagId%1").arg(idx), it.key());
                query.bindValue(QString(":tagVal%1").arg(idx), it.value());
                ++idx;
            }
            query.bindValue(":profileId", profileId);
            query.bindValue(":tagCount", tagIdToValue.size());

            // for debug

            // Reconstruct the query for debugging
            // QString reconstructedQuery = sql;
            // idx = 1;
            // for (auto it = tagIdToValue.constBegin(); it != tagIdToValue.constEnd(); ++it) {
            //     QString tagIdPlaceholder = QString(":tagId%1").arg(idx);
            //     QString tagValPlaceholder = QString(":tagVal%1").arg(idx);

            //     reconstructedQuery.replace(tagIdPlaceholder, QString::number(it.key()));

            //     QString safeValue = it.value();
            //     safeValue.replace("'", "''");  // SQL-safe
            //     reconstructedQuery.replace(tagValPlaceholder, QString("'%1'").arg(safeValue));

            //     ++idx;
            // }

            // reconstructedQuery.replace(":profileId", QString::number(profileId));
            // reconstructedQuery.replace(":tagCount", QString::number(tagIdToValue.size()));

            // // Log the final SQL
            // qDebug() << "[DEBUG] Final Executed SQL:\n" << reconstructedQuery;

            // throw std::exception("break it here");

            if (!query.exec()) {
                QString error = translator->getErrorMessage(MWL_QUERY_FAILED_MSG).arg(query.lastError().text());
                logger->LogError(error);
                qDebug()<<error;
                return Result<int>::Failure(error);
            }

            if (query.next()) {
            int foundId = query.value("id").toInt();
            return Result<int>::Success(foundId);
            }
        }
        QSqlDatabase::removeDatabase(connName);
        return Result<int>::Success(-1);  // not found
    }

    QMap<int, QList<WorklistAttribute>> WorklistRepository::loadAttributesForEntries(const QList<int>& entryIds, QSqlDatabase& db) const {
        QMap<int, QList<WorklistAttribute>> attributesMap;
        if (entryIds.isEmpty()) return attributesMap;

        QStringList placeholders;
        for (int i = 0; i < entryIds.size(); ++i) {
            placeholders << "?";
        }

        QString sql = QString(R"(
            SELECT wa.id, wa.mwl_entry_id, wa.tag_value,
                   t.id AS tag_id, t.name, t.display_name, t.group_hex, t.element_hex, t.pgroup_hex, t.pelement_hex, t.is_active, t.is_retired
            FROM mwl_attributes wa
            JOIN dicom_tags t ON wa.dicom_tag_id = t.id
            WHERE wa.mwl_entry_id IN (%1) AND t.is_active = TRUE
        )").arg(placeholders.join(","));

        QSqlQuery query(db);
        query.prepare(sql);

        for (int i = 0; i < entryIds.size(); ++i) {
            query.bindValue(i, entryIds[i]);
        }

        if (!query.exec()) {
            // Log or handle error, here we just return empty map
            return attributesMap;
        }

        while (query.next()) {
            WorklistAttribute attr;
            attr.id = query.value("id").toInt();
            attr.EntryId = query.value("mwl_entry_id").toInt();
            attr.Tag.Id = query.value("tag_id").toInt();
            attr.Tag.Name = query.value("name").toString();
            attr.Tag.DisplayName = query.value("display_name").toString();
            attr.Tag.GroupHex = query.value("group_hex").toUInt();
            attr.Tag.ElementHex = query.value("element_hex").toUInt();
            attr.Tag.PgroupHex = query.value("pgroup_hex").toUInt();
            attr.Tag.PelementHex = query.value("pelement_hex").toUInt();
            attr.Tag.IsActive = query.value("is_active").toBool();
            attr.Tag.IsRetired = query.value("is_retired").toBool();
            attr.TagValue = query.value("tag_value").toString();

            attributesMap[attr.EntryId].append(attr);
        }

        return attributesMap;
    }

    Result<WorklistEntry> WorklistRepository::getWorklistEntryDetails(int entryId) const {

        WorklistEntry entry;
        QString connName = "conn_get_entry_details_" + QString::number(QRandomGenerator::global()->generate());

        {
            QSqlDatabase db = createConnection(connName);

            if (!db.open()) {
                QString error = translator->getErrorMessage(FAILED_TO_OPEN_DB_MSG).arg(db.lastError().text());
                logger->LogError(error);
                qDebug()<<error;
                return Result<WorklistEntry>::Failure(error);
            }

            QSqlQuery query(db);
            query.prepare(R"(
                SELECT id, source, profile_id, status, created_at, updated_at
                FROM mwl_entries
                WHERE id = :entryId
            )");
            query.bindValue(":entryId", entryId);

            if (!query.exec() || !query.next()) {
                QString error = translator->getErrorMessage(MWL_FAILED_TO_FIND_ENTRY_MSG).arg(query.lastError().text());
                logger->LogError(error);
                qDebug()<<error;
                return Result<WorklistEntry>::Failure(error);
            }

            entry.Id = query.value("id").toInt();
            entry.Source = static_cast<Source>(query.value("source").toInt());
            entry.Profile.Id = query.value("profile_id").toInt();
            entry.Status = static_cast<ProcedureStepStatus>(query.value("status").toInt());
            entry.CreatedAt = query.value("created_at").toDateTime();
            entry.UpdatedAt = query.value("updated_at").toDateTime();

            // Fetch associated attributes (tags)
            QSqlQuery attrQuery(db);
            attrQuery.prepare(R"(
                SELECT wa.id, wa.tag_value, t.id AS tag_id, t.name, t.display_name, t.group_hex, t.element_hex, t.pgroup_hex, t.pelement_hex, t.is_active, t.is_retired
                FROM mwl_attributes wa
                JOIN dicom_tags t ON wa.dicom_tag_id = t.id
                WHERE wa.mwl_entry_id = :entryId
            )");
            attrQuery.bindValue(":entryId", entryId);

            if (!attrQuery.exec()) {
                QString error = translator->getErrorMessage(MWL_FAILED_TO_LOAD_ATTRIBUTES_MSG);
                logger->LogError(error);
                qDebug()<<error;
                return Result<WorklistEntry>::Failure(error);
            }

            while (attrQuery.next()) {
                WorklistAttribute attr;
                attr.id = attrQuery.value("id").toInt();
                attr.EntryId = entryId;
                attr.Tag.Id = attrQuery.value("tag_id").toInt();
                attr.Tag.Name = attrQuery.value("name").toString();
                attr.Tag.DisplayName = attrQuery.value("display_name").toString();
                attr.Tag.GroupHex = attrQuery.value("group_hex").toUInt();
                attr.Tag.ElementHex = attrQuery.value("element_hex").toUInt();
                attr.Tag.PgroupHex = attrQuery.value("pgroup_hex").toUInt();
                attr.Tag.PelementHex = attrQuery.value("pelement_hex").toUInt();
                attr.Tag.IsActive = attrQuery.value("is_active").toBool();
                attr.Tag.IsRetired = attrQuery.value("is_retired").toBool();
                attr.TagValue = attrQuery.value("tag_value").toString();

                entry.Attributes.append(attr);
            }

        }
        QSqlDatabase::removeDatabase(connName);
        return Result<WorklistEntry>::Success(entry);
    }

} // namespace Etrek::Repository
