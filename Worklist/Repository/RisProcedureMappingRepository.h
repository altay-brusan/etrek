/**
 * @file RisProcedureMappingRepository.h
 * @brief Repository for RIS procedure code to internal view mappings.
 *
 * @details Provides CRUD operations for RIS procedure code mappings, enabling
 *          automatic view selection when importing worklist entries from RIS systems.
 *
 * @author Etrek Development Team
 * @date 2026-01-11
 *
 * @copyright Copyright (c) 2024-2026 Etrek Medical Imaging
 *
 * @see RisProcedureMapping
 * @see RisProcedureMappingService
 * @see RisProcedureMappingConfigurationDelegate
 */

#ifndef RISPROCEDUREMAPPINGREPOSITORY_H
#define RISPROCEDUREMAPPINGREPOSITORY_H

#include <QObject>
#include <QVector>
#include <memory>
#include <optional>
#include <QSqlDatabase>
#include "Result.h"
#include "DatabaseConnectionSetting.h"
#include "RisProcedureMapping.h"
#include "AppLogger.h"

namespace Etrek::Worklist::Repository {

    namespace ent = Etrek::Worklist::Data::Entity;

    /**
     * @class RisProcedureMappingRepository
     * @brief Provides data access for RIS procedure code to internal view mappings.
     *
     * This repository handles CRUD operations for ris_procedure_mapping table,
     * enabling the system to map external RIS procedure codes to internal view IDs.
     */
    class RisProcedureMappingRepository : public QObject {
        Q_OBJECT

    public:
        explicit RisProcedureMappingRepository(
            std::shared_ptr<Etrek::Core::Data::Model::DatabaseConnectionSetting> connectionSetting,
            QObject* parent = nullptr);

        virtual ~RisProcedureMappingRepository() = default;

        // CRUD Operations

        /**
         * @brief Inserts a new procedure mapping.
         * @param mapping The mapping to insert (Id will be set on success).
         * @return Result containing the inserted mapping with Id set.
         */
        Etrek::Specification::Result<ent::RisProcedureMapping> insert(ent::RisProcedureMapping& mapping);

        /**
         * @brief Updates an existing procedure mapping.
         * @param mapping The mapping to update.
         * @return Result indicating success or failure.
         */
        Etrek::Specification::Result<bool> update(const ent::RisProcedureMapping& mapping);

        /**
         * @brief Removes a procedure mapping by ID.
         * @param id The mapping ID to remove.
         * @return Result indicating success or failure.
         */
        Etrek::Specification::Result<bool> remove(int id);

        // Query Operations

        /**
         * @brief Retrieves a mapping by its ID.
         * @param id The mapping ID.
         * @return Optional containing the mapping if found.
         */
        std::optional<ent::RisProcedureMapping> getById(int id) const;

        /**
         * @brief Retrieves all mappings for a RIS connection.
         * @param connectionName The RIS connection name.
         * @return Vector of mappings for the connection.
         */
        QVector<ent::RisProcedureMapping> getByConnectionName(const QString& connectionName) const;

        /**
         * @brief Retrieves all active mappings.
         * @return Vector of all active mappings.
         */
        QVector<ent::RisProcedureMapping> getAllActive() const;

        // Key lookup for MWL import

        /**
         * @brief Finds the internal view ID for an external procedure code.
         * @param connectionName The RIS connection name.
         * @param externalCode The external procedure code from RIS.
         * @return Optional containing the internal view ID if mapping exists.
         */
        std::optional<int> findViewIdByExternalCode(
            const QString& connectionName,
            const QString& externalCode) const;

        /**
         * @brief Finds the full mapping for an external procedure code.
         * @param connectionName The RIS connection name.
         * @param externalCode The external procedure code from RIS.
         * @return Optional containing the full mapping if found.
         */
        std::optional<ent::RisProcedureMapping> findByExternalCode(
            const QString& connectionName,
            const QString& externalCode) const;

    signals:
        /**
         * @brief Emitted when an unmapped procedure code is encountered.
         * @param connectionName The RIS connection name.
         * @param externalCode The unmapped procedure code.
         * @param codeMeaning The code meaning if available.
         */
        void unmappedCodeEncountered(
            const QString& connectionName,
            const QString& externalCode,
            const QString& codeMeaning);

    private:
        QSqlDatabase createConnection(const QString& connectionName) const;
        ent::RisProcedureMapping mapRowToEntity(const QSqlQuery& query) const;

        std::shared_ptr<Etrek::Core::Data::Model::DatabaseConnectionSetting> m_connectionSetting;
        std::shared_ptr<Etrek::Core::Log::AppLogger> m_logger;
    };

} // namespace Etrek::Worklist::Repository

#endif // RISPROCEDUREMAPPINGREPOSITORY_H
