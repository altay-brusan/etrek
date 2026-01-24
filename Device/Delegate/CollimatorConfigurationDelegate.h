/**
 * @file CollimatorConfigurationDelegate.h
 * @brief Delegate for collimator device configuration persistence.
 *
 * @details This delegate handles the connection between the CollimatorConfigurationWidget
 *          and the DeviceRepository, ensuring collimator configuration changes are
 *          persisted to the database.
 *
 * @author Etrek Development Team
 * @date 2026-01-11
 *
 * @copyright Copyright (c) 2024-2026 Etrek Medical Imaging
 *
 * @see CollimatorConfigurationWidget
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
class CollimatorConfigurationWidget;

namespace Etrek::Device::Delegate
{
	namespace rpo = Etrek::Device::Repository;

	/**
	 * @class CollimatorConfigurationDelegate
	 * @brief Delegate for X-ray collimator configuration persistence.
	 *
	 * @details Implements the Delegate pattern to handle business logic for
	 *          collimator configuration. Connects CollimatorConfigurationWidget
	 *          to DeviceRepository for CRUD operations.
	 *
	 *          Key responsibilities:
	 *          - Receive collimator settings from the UI widget
	 *          - Validate configuration changes
	 *          - Persist changes to database via DeviceRepository
	 *          - Handle apply/accept/reject actions from settings dialog
	 *
	 *          Collimator configuration includes parameters like:
	 *          - Blade positions and ranges
	 *          - Field size presets
	 *          - Communication protocol settings
	 *
	 * @see CollimatorConfigurationWidget
	 * @see CollimatorConfigurationBuilder
	 * @see DeviceRepository
	 * @see IPageAction
	 *
	 * @ingroup Device
	 */
	class CollimatorConfigurationDelegate :
		public QObject,
		public IDelegate,
		public IPageAction
	{
		Q_OBJECT
		Q_INTERFACES(IDelegate IPageAction)

	public:
		/**
		 * @brief Constructs a CollimatorConfigurationDelegate.
		 *
		 * @param[in] widget Pointer to the associated UI widget.
		 * @param[in] repository Shared pointer to the device repository.
		 * @param[in] parent Parent QObject for memory management.
		 */
		CollimatorConfigurationDelegate(
			CollimatorConfigurationWidget* widget,
			std::shared_ptr<rpo::DeviceRepository> repository,
			QObject* parent);

		/**
		 * @brief Returns the delegate's unique name identifier.
		 * @return QString containing "CollimatorConfigurationDelegate".
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
		~CollimatorConfigurationDelegate();

	private:
		CollimatorConfigurationWidget* m_widget = nullptr;   ///< Pointer to the associated UI widget.
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

