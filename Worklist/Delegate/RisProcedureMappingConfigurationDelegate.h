/**
 * @file RisProcedureMappingConfigurationDelegate.h
 * @brief Delegate for RIS procedure mapping configuration persistence.
 *
 * @details This delegate handles the connection between the RisProcedureMappingConfigurationWidget
 *          and the RisProcedureMappingRepository, ensuring RIS-to-internal-view mappings
 *          are persisted to the database.
 *
 * @author Etrek Development Team
 * @date 2026-01-11
 *
 * @copyright Copyright (c) 2024-2026 Etrek Medical Imaging
 *
 * @see RisProcedureMappingConfigurationWidget
 * @see RisProcedureMappingRepository
 * @see IPageAction
 */

#ifndef RISPROCEDUREMAPPINGCONFIGURATIONDELEGATE_H
#define RISPROCEDUREMAPPINGCONFIGURATIONDELEGATE_H

#include <QObject>
#include <memory>
#include "IDelegate.h"
#include "IPageAction.h"

namespace Etrek::Worklist::Repository { class RisProcedureMappingRepository; }
class RisProcedureMappingConfigurationWidget;

namespace Etrek::Worklist::Delegate
{
    namespace rpo = Etrek::Worklist::Repository;

    /**
     * @class RisProcedureMappingConfigurationDelegate
     * @brief Delegate for RIS procedure mapping configuration.
     *
     * @details Implements the Delegate pattern to handle business logic for
     *          RIS procedure code to internal view mappings. Connects
     *          RisProcedureMappingConfigurationWidget to RisProcedureMappingRepository.
     *
     *          Key responsibilities:
     *          - Receive mapping changes from the UI widget
     *          - Handle insert, update, and delete operations
     *          - Persist changes to database via repository
     *          - Handle apply/accept/reject actions from settings dialog
     *
     *          The widget tracks modified and deleted mappings separately,
     *          and this delegate processes them in the correct order:
     *          deletions first, then inserts/updates.
     *
     * @see RisProcedureMappingConfigurationWidget
     * @see RisProcedureMappingConfigurationBuilder
     * @see RisProcedureMappingRepository
     * @see RisProcedureMapping
     *
     * @ingroup Worklist
     */
    class RisProcedureMappingConfigurationDelegate :
        public QObject,
        public IDelegate,
        public IPageAction
    {
        Q_OBJECT
        Q_INTERFACES(IDelegate IPageAction)

    public:
        /**
         * @brief Constructs a RisProcedureMappingConfigurationDelegate.
         *
         * @param[in] widget Pointer to the associated UI widget.
         * @param[in] repository Shared pointer to the RIS mapping repository.
         * @param[in] parent Parent QObject for memory management.
         */
        RisProcedureMappingConfigurationDelegate(
            RisProcedureMappingConfigurationWidget* widget,
            std::shared_ptr<rpo::RisProcedureMappingRepository> repository,
            QObject* parent = nullptr);

        /**
         * @brief Destructor.
         */
        ~RisProcedureMappingConfigurationDelegate() override;

        /**
         * @brief Returns the delegate's unique name identifier.
         * @return QString containing "RisProcedureMappingConfigurationDelegate".
         */
        QString name() const override;

        /**
         * @brief Attaches related delegates for inter-delegate communication.
         * @param[in] delegates Vector of delegate objects to attach.
         * @note Currently not used by this delegate.
         */
        void attachDelegates(const QVector<QObject*>& delegates) override;

    private:
        RisProcedureMappingConfigurationWidget* m_widget = nullptr; ///< Pointer to the associated UI widget.
        std::shared_ptr<rpo::RisProcedureMappingRepository> m_repository; ///< Repository for database operations.

        /**
         * @brief Persists current widget state to the database.
         * @details Called when user clicks "Apply" in settings dialog.
         */
        void apply() override;

        /**
         * @brief Persists changes and signals acceptance.
         * @details Called when user clicks "OK" in settings dialog.
         */
        void accept() override;

        /**
         * @brief Discards changes without saving.
         * @details Called when user clicks "Cancel" in settings dialog.
         */
        void reject() override;

        /**
         * @brief Saves all modified and deleted mappings to the database.
         *
         * @details Processes changes in the following order:
         *          1. Delete mappings marked for deletion
         *          2. Insert new mappings (Id < 0)
         *          3. Update existing mappings
         *
         * @return True if all operations succeeded, false if any failed.
         *
         * @note Partial failures are logged but do not roll back successful operations.
         */
        bool saveAllMappings();
    };
}

#endif // RISPROCEDUREMAPPINGCONFIGURATIONDELEGATE_H
