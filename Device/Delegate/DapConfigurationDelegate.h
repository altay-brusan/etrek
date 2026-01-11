/**
 * @file DapConfigurationDelegate.h
 * @brief Delegate for DAP (Dose Area Product) meter configuration persistence.
 *
 * @details This delegate handles the connection between the DapConfigurationWidget
 *          and the DeviceRepository, ensuring DAP meter configuration changes are
 *          persisted to the database.
 *
 * @author Etrek Development Team
 * @date 2026-01-11
 *
 * @copyright Copyright (c) 2024-2026 Etrek Medical Imaging
 *
 * @see DapConfigurationWidget
 * @see DeviceRepository
 * @see IPageAction
 */

#pragma once

#include <QObject>
#include <QWidget>
#include <memory>
#include "IDelegate.h"
#include "IPageAction.h"

namespace Etrek::Device::Repository { class DeviceRepository; }
class DapConfigurationWidget;

namespace Etrek::Device::Delegate
{
	namespace rpo = Etrek::Device::Repository;

	/**
	 * @class DapConfigurationDelegate
	 * @brief Delegate for DAP (Dose Area Product) meter configuration.
	 *
	 * @details Implements the Delegate pattern to handle business logic for
	 *          DAP meter configuration. Connects DapConfigurationWidget
	 *          to DeviceRepository for CRUD operations.
	 *
	 *          Key responsibilities:
	 *          - Receive DAP meter settings from the UI widget
	 *          - Validate configuration changes
	 *          - Persist changes to database via DeviceRepository
	 *          - Handle apply/accept/reject actions from settings dialog
	 *
	 *          DAP meter configuration includes parameters like:
	 *          - Calibration factors
	 *          - Communication settings
	 *          - Measurement units
	 *          - Dose tracking thresholds
	 *
	 * @see DapConfigurationWidget
	 * @see DapConfigurationBuilder
	 * @see DeviceRepository
	 * @see IPageAction
	 *
	 * @ingroup Device
	 */
	class DapConfigurationDelegate :
	public QObject,
	public IDelegate,
	public IPageAction
	{
		Q_OBJECT
		Q_INTERFACES(IDelegate IPageAction)

	public:
		/**
		 * @brief Constructs a DapConfigurationDelegate.
		 *
		 * @param[in] widget Pointer to the associated UI widget.
		 * @param[in] repository Shared pointer to the device repository.
		 * @param[in] parent Parent QObject for memory management.
		 */
		DapConfigurationDelegate(
			DapConfigurationWidget* widget,
			std::shared_ptr<rpo::DeviceRepository> repository,
			QObject* parent);

		/**
		 * @brief Returns the delegate's unique name identifier.
		 * @return QString containing "DapConfigurationDelegate".
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
		~DapConfigurationDelegate();

	private:
		DapConfigurationWidget* m_widget = nullptr;          ///< Pointer to the associated UI widget.
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

