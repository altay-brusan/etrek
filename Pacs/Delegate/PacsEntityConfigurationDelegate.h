/**
 * @file PacsEntityConfigurationDelegate.h
 * @brief Delegate for PACS node configuration persistence.
 *
 * @details This delegate handles the connection between the PacsEntityConfigurationWidget
 *          and the PacsNodeRepository, ensuring PACS server configuration changes
 *          are persisted to the database.
 *
 * @author Etrek Development Team
 * @date 2026-01-11
 *
 * @copyright Copyright (c) 2024-2026 Etrek Medical Imaging
 *
 * @see PacsEntityConfigurationWidget
 * @see PacsNodeRepository
 * @see IPageAction
 */

#pragma once

#include <QObject>
#include <QWidget>
#include <memory>
#include "IDelegate.h"
#include "IPageAction.h"

namespace Etrek::Pacs::Repository { class PacsNodeRepository; }
namespace Etrek::Context { class IContextManager; }
class PacsEntityConfigurationWidget;

/**
 * @namespace Etrek::Pacs::Delegate
 * @brief Contains delegate classes for PACS-related UI components.
 */
namespace Etrek::Pacs::Delegate
{
	namespace rpo = Etrek::Pacs::Repository;

	/**
	 * @class PacsEntityConfigurationDelegate
	 * @brief Delegate for PACS server node configuration.
	 *
	 * @details Implements the Delegate pattern to handle business logic for
	 *          PACS node configuration. Connects PacsEntityConfigurationWidget
	 *          to PacsNodeRepository for CRUD operations.
	 *
	 *          Key responsibilities:
	 *          - Receive PACS node settings from the UI widget
	 *          - Validate DICOM connection parameters
	 *          - Persist changes to database via PacsNodeRepository
	 *          - Handle apply/accept/reject actions from settings dialog
	 *
	 *          PACS configuration includes:
	 *          - Server AE Title, hostname, port
	 *          - Connection timeout settings
	 *          - Transfer syntax preferences
	 *          - Storage commitment options
	 *          - Query/Retrieve settings
	 *
	 * @see PacsEntityConfigurationWidget
	 * @see PacsEntityConfigurationBuilder
	 * @see PacsNodeRepository
	 * @see IPageAction
	 *
	 * @ingroup Pacs
	 */
	class PacsEntityConfigurationDelegate :
	public QObject,
	public IDelegate,
	public IPageAction
	{
		Q_OBJECT
		Q_INTERFACES(IDelegate IPageAction)

	public:
		/**
		 * @brief Constructs a PacsEntityConfigurationDelegate.
		 *
		 * @param[in] widget Pointer to the associated UI widget.
		 * @param[in] repository Shared pointer to the PACS node repository.
		 * @param[in] contextManager Weak pointer to the context manager for audit tracking.
		 * @param[in] parent Parent QObject for memory management.
		 */
		PacsEntityConfigurationDelegate(
			PacsEntityConfigurationWidget* widget,
			std::shared_ptr<rpo::PacsNodeRepository> repository,
			std::weak_ptr<Etrek::Context::IContextManager> contextManager,
			QObject* parent);

		/**
		 * @brief Returns the delegate's unique name identifier.
		 * @return QString containing "PacsEntityConfigurationDelegate".
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
		~PacsEntityConfigurationDelegate();

	private:
		PacsEntityConfigurationWidget* m_widget = nullptr;       ///< Pointer to the associated UI widget.
		std::shared_ptr<rpo::PacsNodeRepository> m_repository;   ///< Repository for database operations.
		std::weak_ptr<Etrek::Context::IContextManager> m_contextManager; ///< Context manager for audit tracking.

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

