#ifndef ETREK_DICOM_REPOSITORY_MWLTASKMAPPINGREPOSITORY_H
#define ETREK_DICOM_REPOSITORY_MWLTASKMAPPINGREPOSITORY_H

#include <QObject>
#include <QVector>
#include <memory>
#include <optional>
#include <QSqlDatabase>
#include "Result.h"
#include "DatabaseConnectionSetting.h"
#include "MwlTaskMapping.h"
#include "AppLogger.h"

namespace Etrek::Dicom::Repository {

    namespace ent = Etrek::Dicom::Data::Entity;

    /**
     * @class MwlTaskMappingRepository
     * @brief Provides data access for MWL to DICOM task mapping records.
     *
     * This repository manages the mwl_task_mapping table which links
     * worklist entries to all DICOM objects created during an examination.
     */
    class MwlTaskMappingRepository : public QObject {
        Q_OBJECT

    public:
        explicit MwlTaskMappingRepository(
            std::shared_ptr<Etrek::Core::Data::Model::DatabaseConnectionSetting> connectionSetting,
            QObject* parent = nullptr);

        virtual ~MwlTaskMappingRepository() = default;

        /**
         * @brief Creates a new MWL task mapping record.
         * @param mapping The mapping to create.
         * @return Result indicating success or failure.
         */
        Etrek::Specification::Result<bool> createMapping(const ent::MwlTaskMapping& mapping);

        /**
         * @brief Gets all mappings for a specific MWL entry.
         * @param mwlEntryId The MWL entry ID.
         * @return Vector of mappings for the entry.
         */
        QVector<ent::MwlTaskMapping> getMappingsByMwlEntry(int mwlEntryId) const;

        /**
         * @brief Gets the mapping for a specific study.
         * @param studyId The study ID.
         * @return Optional containing the mapping if found.
         */
        std::optional<ent::MwlTaskMapping> getMappingByStudy(int studyId) const;

        /**
         * @brief Gets the mapping by primary key.
         * @param mwlEntryId The MWL entry ID.
         * @param procedureId The procedure ID.
         * @return Optional containing the mapping if found.
         */
        std::optional<ent::MwlTaskMapping> getMappingByKey(int mwlEntryId, int procedureId) const;

        /**
         * @brief Gets the full DICOM chain for a worklist entry and procedure.
         * @param mwlEntryId The MWL entry ID.
         * @param procedureId The procedure ID.
         * @return Optional containing the full chain if found.
         */
        std::optional<ent::DicomChain> getFullChainByMwl(int mwlEntryId, int procedureId) const;

        /**
         * @brief Updates an existing mapping.
         * @param mapping The mapping to update.
         * @return Result indicating success or failure.
         */
        Etrek::Specification::Result<bool> updateMapping(const ent::MwlTaskMapping& mapping);

        /**
         * @brief Deletes a mapping by primary key.
         * @param mwlEntryId The MWL entry ID.
         * @param procedureId The procedure ID.
         * @return Result indicating success or failure.
         */
        Etrek::Specification::Result<bool> deleteMapping(int mwlEntryId, int procedureId);

    private:
        QSqlDatabase createConnection(const QString& connectionName) const;
        ent::MwlTaskMapping mapRowToEntity(const QSqlQuery& query) const;

        std::shared_ptr<Etrek::Core::Data::Model::DatabaseConnectionSetting> m_connectionSetting;
        std::shared_ptr<Etrek::Core::Log::AppLogger> m_logger;
    };

} // namespace Etrek::Dicom::Repository

#endif // ETREK_DICOM_REPOSITORY_MWLTASKMAPPINGREPOSITORY_H
