/**
 * @file ProcedureConfigurationDelegate.h
 * @brief Delegate for radiographic procedure configuration persistence.
 *
 * @details This delegate handles the connection between the ProcedureConfigurationWidget
 *          and the ScanProtocolRepository, ensuring procedure configuration changes
 *          are persisted to the database.
 *
 * @author Etrek Development Team
 * @date 2026-01-11
 *
 * @copyright Copyright (c) 2024-2026 Etrek Medical Imaging
 *
 * @see ProcedureConfigurationWidget
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
class ProcedureConfigurationWidget;

namespace Etrek::ScanProtocol::Delegate
{
	/**
	 * @class ProcedureConfigurationDelegate
	 * @brief Delegate for radiographic procedure configuration.
	 *
	 * @details Implements the Delegate pattern to handle business logic for
	 *          procedure configuration. Connects ProcedureConfigurationWidget
	 *          to ScanProtocolRepository for CRUD operations.
	 *
	 *          Key responsibilities:
	 *          - Receive procedure settings from the UI widget
	 *          - Validate procedure configuration
	 *          - Persist changes to database via ScanProtocolRepository
	 *          - Handle apply/accept/reject actions from settings dialog
	 *
	 *          Procedure configuration includes:
	 *          - Procedure name and code
	 *          - Associated views (multi-view procedures)
	 *          - Body part mappings
	 *          - RIS procedure code mappings
	 *          - Default technique presets
	 *
	 * @see ProcedureConfigurationWidget
	 * @see ProcedureConfigurationBuilder
	 * @see ScanProtocolRepository
	 * @see IPageAction
	 *
	 * @ingroup ScanProtocol
	 */
	class ProcedureConfigurationDelegate :
	public QObject,
	public IDelegate,
	public IPageAction
	{
		Q_OBJECT
		Q_INTERFACES(IDelegate IPageAction)

	public:
		/**
		 * @brief Constructs a ProcedureConfigurationDelegate.
		 *
		 * @param[in] widget Pointer to the associated UI widget.
		 * @param[in] repository Shared pointer to the scan protocol repository.
		 * @param[in] parent Parent QObject for memory management.
		 */
		ProcedureConfigurationDelegate(
			ProcedureConfigurationWidget* widget,
			std::shared_ptr<Etrek::ScanProtocol::Repository::ScanProtocolRepository> repository,
			QObject* parent);

		/**
		 * @brief Returns the delegate's unique name identifier.
		 * @return QString containing "ProcedureConfigurationDelegate".
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
		~ProcedureConfigurationDelegate();

	private:
		ProcedureConfigurationWidget* m_widget;  ///< Pointer to the associated UI widget.
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
		 * @brief Saves all procedures from the widget to the database.
		 * @return True if all procedures were saved successfully, false otherwise.
		 */
		bool saveAllProcedures();
	};

}