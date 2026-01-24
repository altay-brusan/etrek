/**
 * @file TechniqueConfigurationDelegate.h
 * @brief Delegate for X-ray technique parameter configuration persistence.
 *
 * @details This delegate handles the connection between the TechniqueConfigurationWidget
 *          and the ScanProtocolRepository, ensuring technique parameter changes
 *          are persisted to the database.
 *
 * @author Etrek Development Team
 * @date 2026-01-11
 *
 * @copyright Copyright (c) 2024-2026 Etrek Medical Imaging
 *
 * @see TechniqueConfigurationWidget
 * @see ScanProtocolRepository
 * @see TechniqueParameter
 * @see IPageAction
 */

#ifndef TECHNIQUECONFIGURATIONDELEGATE_H
#define TECHNIQUECONFIGURATIONDELEGATE_H

#include <QHash>
#include <memory>

#include "IDelegate.h"
#include "IPageAction.h"

#include "TechniqueParameter.h"
#include "TechniqueConfigurationWidget.h"
#include "ScanProtocolRepository.h"

namespace Etrek::ScanProtocol::Delegate {

    namespace ent = Etrek::ScanProtocol::Data::Entity;
    namespace repo = Etrek::ScanProtocol::Repository;

    /**
     * @class TechniqueConfigurationDelegate
     * @brief Delegate for X-ray technique parameter configuration.
     *
     * @details Implements the Delegate pattern to handle business logic for
     *          technique parameter configuration. Connects TechniqueConfigurationWidget
     *          to ScanProtocolRepository for CRUD operations.
     *
     *          Key responsibilities:
     *          - Receive technique parameters from the UI widget
     *          - Track changes using a cache for efficient diffing
     *          - Handle insert, update, and delete operations
     *          - Persist changes to database via ScanProtocolRepository
     *          - Handle apply/accept/reject actions from settings dialog
     *
     *          Technique parameters include:
     *          - KVP (kilovolt peak)
     *          - mA (milliampere)
     *          - Exposure time
     *          - SID (source-to-image distance)
     *          - Parameters vary by body part, technique profile, and patient size
     *
     * @see TechniqueConfigurationWidget
     * @see TechniqueConfigurationBuilder
     * @see ScanProtocolRepository
     * @see TechniqueParameter
     * @see IPageAction
     *
     * @ingroup ScanProtocol
     */
    class TechniqueConfigurationDelegate :
        public QObject,
        public IDelegate,
        public IPageAction
    {
        Q_OBJECT
            Q_INTERFACES(IDelegate IPageAction)

    public:
        /**
         * @brief Constructs a TechniqueConfigurationDelegate.
         *
         * @param[in] widget Pointer to the associated UI widget.
         * @param[in] repo Shared pointer to the scan protocol repository.
         * @param[in] parent Parent QObject for memory management.
         */
        TechniqueConfigurationDelegate(TechniqueConfigurationWidget* widget,
            std::shared_ptr<repo::ScanProtocolRepository> repo,
            QObject* parent = nullptr);

        /**
         * @brief Destructor.
         */
        ~TechniqueConfigurationDelegate() override;

        /**
         * @brief Returns the delegate's unique name identifier.
         * @return QString containing "TechniqueConfigurationDelegate".
         */
        QString name() const override;

        /**
         * @brief Attaches related delegates for inter-delegate communication.
         * @param[in] delegates Vector of delegate objects to attach.
         */
        void attachDelegates(const QVector<QObject*>&) override;

    private:
        /**
         * @brief Persists current widget state to the database.
         */
        void apply() override;

        /**
         * @brief Persists changes and signals acceptance.
         */
        void accept() override;

        /**
         * @brief Discards changes without saving.
         */
        void reject() override;

        TechniqueConfigurationWidget* m_widget = nullptr;  ///< Pointer to the associated UI widget.
        std::shared_ptr<repo::ScanProtocolRepository> m_repo; ///< Repository for database operations.

        /**
         * @brief Cache of last persisted parameters for change detection.
         * @details Used to determine which parameters need insert/update/delete.
         */
        QHash<QString, Etrek::ScanProtocol::Data::Entity::TechniqueParameter> m_lastPersistedByKey;

        /**
         * @brief Creates a unique cache key for a technique parameter.
         *
         * @param[in] bodyPartId The body part identifier.
         * @param[in] p The technique profile.
         * @param[in] s The body size category.
         * @return QString key combining all parameters.
         */
        static QString makeKey(int bodyPartId,
            Etrek::ScanProtocol::TechniqueProfile p,
            Etrek::ScanProtocol::BodySize s);

        /**
         * @brief Refreshes the persisted cache for a specific body part.
         * @param[in] bodyPartId The body part to refresh cache for.
         */
        void refreshPersistedCacheFor(int bodyPartId);

        /**
         * @brief Saves technique parameters for the currently selected body part.
         * @return True if save was successful, false otherwise.
         */
        bool saveCurrentBodyPart();
    };

} // namespace Etrek::ScanProtocol::Delegate


#endif // !TECHNIQUECONFIGURATIONDELEGATE_H

