/**
 * @file PacsNodeRepository.h
 * @brief Repository for PACS node configuration data access.
 *
 * @details Provides CRUD operations for PACS server node configurations
 *          including archive servers and MPPS (Modality Performed Procedure Step) servers.
 *
 * @author Etrek Development Team
 * @date 2026-01-11
 *
 * @copyright Copyright (c) 2024-2026 Etrek Medical Imaging
 *
 * @see PacsNode
 * @see PacsEntityType
 */

#ifndef PACSNODEREPOSITORY_H
#define PACSNODEREPOSITORY_H

#include <QSqlDatabase>
#include <QVector>
#include <QSqlQuery>
#include <QRandomGenerator>
#include <QSqlError>
#include <memory>

#include "DatabaseConnectionSetting.h"
#include "TranslationProvider.h"
#include "Result.h"
#include "AppLogger.h"
#include "PacsNode.h"  // contains PacsNode + PacsEntityType
#include "PacsEntityType.h"

/**
 * @namespace Etrek::Pacs::Repository
 * @brief Contains repository classes for PACS data access.
 */
namespace Etrek::Pacs::Repository {

    /**
     * @class PacsNodeRepository
     * @brief Repository for PACS server node configuration.
     *
     * @details Implements the Repository pattern for PACS node data access.
     *          Provides CRUD operations for PACS server configurations.
     *
     *          Supported entity types:
     *          - Archive: DICOM storage/query servers
     *          - MPPS: Modality Performed Procedure Step servers
     *
     *          Each PACS node includes:
     *          - AE Title (Application Entity Title)
     *          - Hostname/IP address
     *          - Port number
     *          - Entity type
     *          - Connection settings
     *
     * @see PacsNode
     * @see PacsEntityType
     * @see PacsEntityConfigurationDelegate
     *
     * @ingroup Pacs
     */
    class PacsNodeRepository
    {
    public:
        /**
         * @brief Constructs a PacsNodeRepository.
         * @param[in] connectionSetting Shared pointer to database connection settings.
         */
        explicit PacsNodeRepository(std::shared_ptr<Etrek::Core::Data::Model::DatabaseConnectionSetting> connectionSetting);

        /**
         * @brief Adds a new PACS node to the database.
         * @param[in] node The PacsNode to add.
         * @return Result containing the created PacsNode with assigned ID.
         */
        Etrek::Specification::Result<Etrek::Pacs::Data::Entity::PacsNode> addPacsNode(const Etrek::Pacs::Data::Entity::PacsNode& node) const;

        /**
         * @brief Removes a PACS node from the database.
         * @param[in] node The PacsNode to remove.
         * @return Result indicating success or failure.
         */
        Etrek::Specification::Result<bool> removePacsNode(const Etrek::Pacs::Data::Entity::PacsNode& node) const;

        /**
         * @brief Updates an existing PACS node.
         * @param[in] node The PacsNode with updated values.
         * @return Result containing the updated PacsNode.
         */
        Etrek::Specification::Result<Etrek::Pacs::Data::Entity::PacsNode> updatePacsNode(const Etrek::Pacs::Data::Entity::PacsNode& node) const;

        /**
         * @brief Retrieves all PACS nodes from the database.
         * @return Vector of all configured PacsNode entities.
         */
        QVector<Etrek::Pacs::Data::Entity::PacsNode> getPacsNodes() const;

        /**
         * @brief Destructor.
         */
        ~PacsNodeRepository() = default;

    private:
        /**
         * @brief Creates a new database connection.
         * @param[in] connectionName Unique name for the connection.
         * @return QSqlDatabase instance.
         */
        QSqlDatabase createConnection(const QString& connectionName) const;

        /**
         * @brief Converts PacsEntityType to database string.
         * @param[in] t The entity type.
         * @return String representation ("Archive" or "MPPS").
         */
        static QString toEntityTypeString(PacsEntityType t);

        /**
         * @brief Parses entity type string from database.
         * @param[in] s The string to parse.
         * @return Corresponding PacsEntityType enum value.
         */
        static PacsEntityType parseEntityTypeString(const QString& s);

        std::shared_ptr<Etrek::Core::Data::Model::DatabaseConnectionSetting> m_connectionSetting; ///< Database connection settings.
        Etrek::Core::Globalization::TranslationProvider* translator = nullptr; ///< Translation provider (non-owning).
        std::shared_ptr<Etrek::Core::Log::AppLogger> logger; ///< Logger instance.
    };

} // namespace Etrek::Pacs::Repository

#endif // PACSNODEREPOSITORY_H
