/**
 * @file WorkListConfigurationDelegate.h
 * @brief Delegate for WorkList configuration settings persistence.
 *
 * @details This delegate handles the connection between the WorkListConfigurationWidget
 *          and the WorklistRepository, ensuring configuration changes are persisted
 *          to the database when the user applies or accepts settings.
 *
 * @author Etrek Development Team
 * @date 2026-01-11
 *
 * @copyright Copyright (c) 2024-2026 Etrek Medical Imaging
 *
 * @see WorkListConfigurationWidget
 * @see WorklistRepository
 * @see IPageAction
 */

#pragma once

#include <QObject>
#include <QWidget>
#include <memory>
#include "IDelegate.h"
#include "IPageAction.h"

namespace Etrek::Worklist::Repository { class WorklistRepository; }
class WorkListConfigurationWidget;

/**
 * @namespace Etrek::Worklist::Delegate
 * @brief Contains delegate classes for worklist-related UI components.
 */
namespace Etrek::Worklist::Delegate
{
	namespace rpo = Etrek::Worklist::Repository;

	/**
	 * @class WorkListConfigurationDelegate
	 * @brief Delegate for worklist configuration persistence.
	 *
	 * @details Implements the Delegate pattern to handle business logic for
	 *          worklist tag configuration. Connects WorkListConfigurationWidget
	 *          to WorklistRepository for CRUD operations on worklist tags.
	 *
	 *          Key responsibilities:
	 *          - Receive worklist tag settings from the UI widget
	 *          - Persist changes to database via WorklistRepository
	 *          - Handle apply/accept/reject actions from settings dialog
	 *
	 * @note This class is part of the Model-View-Delegate (MVD) pattern used
	 *       throughout the configuration system.
	 *
	 * @see WorkListConfigurationWidget
	 * @see WorkListConfigurationBuilder
	 * @see WorklistRepository
	 * @see IPageAction
	 *
	 * @ingroup Worklist
	 */
	class WorkListConfigurationDelegate :
	public QObject,
	public IDelegate,
	public IPageAction
	{
		Q_OBJECT
		Q_INTERFACES(IDelegate IPageAction)

	public:
		/**
		 * @brief Constructs a WorkListConfigurationDelegate.
		 *
		 * @param[in] widget Pointer to the associated UI widget.
		 * @param[in] repository Shared pointer to the worklist repository for database operations.
		 * @param[in] parent Parent QObject for memory management.
		 */
		WorkListConfigurationDelegate(
			WorkListConfigurationWidget* widget,
			std::shared_ptr<rpo::WorklistRepository> repository,
			QObject* parent);

		/**
		 * @brief Returns the delegate's unique name identifier.
		 * @return QString containing "WorkListConfigurationDelegate".
		 */
		QString name() const override;

		/**
		 * @brief Attaches related delegates for inter-delegate communication.
		 * @param[in] delegates Vector of delegate objects to attach.
		 * @note Currently not used by this delegate.
		 */
		void attachDelegates(const QVector<QObject*>& delegates) override;

		/**
		 * @brief Destructor.
		 */
		~WorkListConfigurationDelegate();

	private:
		WorkListConfigurationWidget* m_widget = nullptr;     ///< Pointer to the associated UI widget.
		std::shared_ptr<rpo::WorklistRepository> m_repository; ///< Repository for database operations.

		/**
		 * @brief Persists current widget state to the database.
		 * @details Called when user clicks "Apply" in settings dialog.
		 * @pre Widget must be valid and contain current user edits.
		 * @post Configuration changes are persisted to database.
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
	};
}

