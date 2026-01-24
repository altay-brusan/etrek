/**
 * @file ViewConfigurationDelegate.h
 * @brief Delegate for radiographic view configuration persistence.
 *
 * @details This delegate handles the connection between the ViewConfigurationWidget
 *          and the ScanProtocolRepository, ensuring view configuration changes
 *          are persisted to the database.
 *
 * @author Etrek Development Team
 * @date 2026-01-11
 *
 * @copyright Copyright (c) 2024-2026 Etrek Medical Imaging
 *
 * @see ViewConfigurationWidget
 * @see ScanProtocolRepository
 * @see IPageAction
 */

#pragma once

#include <QObject>
#include <QWidget>
#include <memory>
#include "IDelegate.h"
#include "IPageAction.h"

namespace Etrek::ScanProtocol::Repository { class ScanProtocolRepository; }
class ViewConfigurationWidget;

/**
 * @namespace Etrek::ScanProtocol::Delegate
 * @brief Contains delegate classes for scan protocol configuration UI components.
 */
namespace Etrek::ScanProtocol::Delegate
{
	/**
	 * @class ViewConfigurationDelegate
	 * @brief Delegate for radiographic view configuration.
	 *
	 * @details Implements the Delegate pattern to handle business logic for
	 *          view configuration. Connects ViewConfigurationWidget to
	 *          ScanProtocolRepository for CRUD operations.
	 *
	 *          Key responsibilities:
	 *          - Receive view settings from the UI widget
	 *          - Validate view configuration
	 *          - Persist changes to database via ScanProtocolRepository
	 *          - Handle apply/accept/reject actions from settings dialog
	 *
	 *          View configuration includes:
	 *          - View name and description
	 *          - Associated body part
	 *          - Default technique parameters
	 *          - Positioning instructions
	 *          - Image processing presets
	 *
	 * @see ViewConfigurationWidget
	 * @see ViewConfigurationBuilder
	 * @see ScanProtocolRepository
	 * @see IPageAction
	 *
	 * @ingroup ScanProtocol
	 */
	class ViewConfigurationDelegate :
	public QObject,
	public IDelegate,
	public IPageAction
	{
		Q_OBJECT
		Q_INTERFACES(IDelegate IPageAction)

	public:
		/**
		 * @brief Constructs a ViewConfigurationDelegate.
		 *
		 * @param[in] widget Pointer to the associated UI widget.
		 * @param[in] repository Shared pointer to the scan protocol repository.
		 * @param[in] parent Parent QObject for memory management.
		 */
		ViewConfigurationDelegate(
			ViewConfigurationWidget* widget,
			std::shared_ptr<Etrek::ScanProtocol::Repository::ScanProtocolRepository> repository,
			QObject* parent);

		/**
		 * @brief Returns the delegate's unique name identifier.
		 * @return QString containing "ViewConfigurationDelegate".
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
		~ViewConfigurationDelegate();

	private:
		ViewConfigurationWidget* m_widget = nullptr;  ///< Pointer to the associated UI widget.
		std::shared_ptr<Etrek::ScanProtocol::Repository::ScanProtocolRepository> m_repository; ///< Repository for database operations.

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

		/**
		 * @brief Saves all views from the widget to the database.
		 * @return True if all views were saved successfully, false otherwise.
		 */
		bool saveAllViews();
	};

}
