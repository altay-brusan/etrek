#ifndef ETREK_WORKLIST_SERVICE_RISPROCEDUREMAPPINGSERVICE_H
#define ETREK_WORKLIST_SERVICE_RISPROCEDUREMAPPINGSERVICE_H

#include <QObject>
#include <memory>
#include <optional>
#include "Result.h"
#include "RisProcedureMapping.h"
#include "RisProcedureMappingRepository.h"

namespace Etrek::ScanProtocol::Repository {
    class ScanProtocolRepository;
}

namespace Etrek::Worklist::Service {

    namespace ent = Etrek::Worklist::Data::Entity;

    /**
     * @brief Result of a procedure code mapping lookup
     */
    struct MappingResult {
        bool success = false;
        int viewId = -1;
        QString viewName;
        QString warningMessage;
    };

    /**
     * @brief Suggested mapping based on code meaning similarity
     */
    struct SuggestedMapping {
        int viewId;
        QString viewName;
        double confidenceScore;  // 0.0 to 1.0
    };

    /**
     * @brief Service for managing RIS procedure code to view mappings.
     *
     * This service provides:
     * - Core mapping lookup for MWL import flow
     * - Unmapped code tracking and notification
     * - Mapping suggestions based on code meaning
     */
    class RisProcedureMappingService : public QObject {
        Q_OBJECT

    public:
        explicit RisProcedureMappingService(
            std::shared_ptr<Etrek::Worklist::Repository::RisProcedureMappingRepository> mappingRepo,
            std::shared_ptr<Etrek::ScanProtocol::Repository::ScanProtocolRepository> viewRepo,
            QObject* parent = nullptr);

        virtual ~RisProcedureMappingService() = default;

        /**
         * @brief Maps an external procedure code to an internal view.
         *
         * This is the core function used during MWL import to translate
         * RIS procedure codes to internal view IDs.
         *
         * @param connectionName The RIS connection name.
         * @param externalCode The procedure code from RIS.
         * @param codingScheme Optional coding scheme (e.g., "SRT").
         * @param codeMeaning Optional human-readable code meaning.
         * @return MappingResult with view info if found, or warning if unmapped.
         */
        MappingResult mapProcedureCode(
            const QString& connectionName,
            const QString& externalCode,
            const QString& codingScheme = QString(),
            const QString& codeMeaning = QString());

        /**
         * @brief Suggests possible mappings based on code meaning.
         *
         * Uses fuzzy matching on view names to suggest possible mappings
         * for unmapped codes.
         *
         * @param externalCodeMeaning The human-readable code meaning from RIS.
         * @return Vector of suggested mappings sorted by confidence.
         */
        QVector<SuggestedMapping> suggestMappings(const QString& externalCodeMeaning);

        /**
         * @brief Creates a new mapping.
         * @param mapping The mapping to create.
         * @return Result with created mapping or error.
         */
        Etrek::Specification::Result<ent::RisProcedureMapping> createMapping(
            ent::RisProcedureMapping& mapping);

        /**
         * @brief Updates an existing mapping.
         * @param mapping The mapping to update.
         * @return Result indicating success or failure.
         */
        Etrek::Specification::Result<bool> updateMapping(
            const ent::RisProcedureMapping& mapping);

        /**
         * @brief Deletes a mapping.
         * @param mappingId The mapping ID to delete.
         * @return Result indicating success or failure.
         */
        Etrek::Specification::Result<bool> deleteMapping(int mappingId);

        /**
         * @brief Gets all mappings for a connection.
         * @param connectionName The RIS connection name.
         * @return Vector of mappings.
         */
        QVector<ent::RisProcedureMapping> getMappingsForConnection(
            const QString& connectionName);

        /**
         * @brief Gets all active mappings.
         * @return Vector of all active mappings.
         */
        QVector<ent::RisProcedureMapping> getAllMappings();

    signals:
        /**
         * @brief Emitted when an unmapped procedure code is encountered.
         *
         * Connect to this signal to track unmapped codes for later
         * configuration by administrators.
         *
         * @param connectionName The RIS connection name.
         * @param externalCode The unmapped procedure code.
         * @param codeMeaning The code meaning if available.
         */
        void unmappedCodeEncountered(
            const QString& connectionName,
            const QString& externalCode,
            const QString& codeMeaning);

    private:
        std::shared_ptr<Etrek::Worklist::Repository::RisProcedureMappingRepository> m_mappingRepo;
        std::shared_ptr<Etrek::ScanProtocol::Repository::ScanProtocolRepository> m_viewRepo;
    };

} // namespace Etrek::Worklist::Service

#endif // ETREK_WORKLIST_SERVICE_RISPROCEDUREMAPPINGSERVICE_H
