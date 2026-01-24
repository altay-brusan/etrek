/**
 * @file WorkflowConfigurationDelegate.h
 * @brief Delegate for workflow settings configuration persistence.
 *
 * @details This delegate handles the connection between the WorkflowConfigurationWidget
 *          and the DeviceRepository, ensuring workflow configuration changes are
 *          persisted to the database.
 *
 * @author Etrek Development Team
 * @date 2026-01-11
 *
 * @copyright Copyright (c) 2024-2026 Etrek Medical Imaging
 *
 * @see WorkflowConfigurationWidget
 * @see DeviceRepository
 * @see IPageAction
 */

#ifndef WORKFLOWCONFIGURATIONDELEGATE_H
#define WORKFLOWCONFIGURATIONDELEGATE_H

#include <QWidget>
#include <QObject>
#include <memory>
#include "IDelegate.h"
#include "IPageAction.h"

namespace Etrek::Device::Repository { class DeviceRepository; }
class WorkflowConfigurationWidget;

namespace Etrek::Device::Delegate
{
	namespace rpo = Etrek::Device::Repository;

	/**
	 * @class WorkflowConfigurationDelegate
	 * @brief Delegate for examination workflow configuration.
	 *
	 * @details Implements the Delegate pattern to handle business logic for
	 *          workflow configuration. Connects WorkflowConfigurationWidget
	 *          to DeviceRepository for CRUD operations.
	 *
	 *          Key responsibilities:
	 *          - Receive workflow settings from the UI widget
	 *          - Validate configuration changes
	 *          - Persist changes to database via DeviceRepository
	 *          - Handle apply/accept/reject actions from settings dialog
	 *
	 *          Workflow configuration includes parameters like:
	 *          - Auto-advance settings
	 *          - Default examination modes
	 *          - Image processing pipeline options
	 *          - Acquisition workflow sequences
	 *
	 * @see WorkflowConfigurationWidget
	 * @see WorkflowConfigurationBuilder
	 * @see DeviceRepository
	 * @see IPageAction
	 *
	 * @ingroup Device
	 */
	class WorkflowConfigurationDelegate :
		public QObject,
		public IDelegate,
		public IPageAction
	{
		Q_OBJECT
		Q_INTERFACES(IDelegate IPageAction)

	public:
		/**
		 * @brief Constructs a WorkflowConfigurationDelegate.
		 *
		 * @param[in] widget Pointer to the associated UI widget.
		 * @param[in] repository Shared pointer to the device repository.
		 * @param[in] parent Parent QObject for memory management.
		 */
		WorkflowConfigurationDelegate(
			WorkflowConfigurationWidget* widget,
			std::shared_ptr<rpo::DeviceRepository> repository,
			QObject* parent = nullptr);

		/**
		 * @brief Returns the delegate's unique name identifier.
		 * @return QString containing "WorkflowConfigurationDelegate".
		 */
		QString name() const override;

		/**
		 * @brief Attaches related delegates for inter-delegate communication.
		 * @param[in] delegates Vector of delegate objects to attach.
		 */
		void attachDelegates(const QVector<QObject*>& delegates) override;

		/**
		 * @brief Destructor.
		 */
		~WorkflowConfigurationDelegate();

	private:
		WorkflowConfigurationWidget* m_widget = nullptr;     ///< Pointer to the associated UI widget.
		std::shared_ptr<rpo::DeviceRepository> m_repository; ///< Repository for database operations.

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
	};
}

#endif // WORKFLOWCONFIGURATIONDELEGATE_H
