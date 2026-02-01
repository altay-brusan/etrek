/**
 * @file DetectorConfigurationDelegate.h
 * @brief Delegate for detector device configuration persistence.
 *
 * @details This delegate handles the connection between the DetectorConfigurationWidget
 *          and the DeviceRepository, ensuring detector configuration changes are
 *          persisted to the database.
 *
 * @author Etrek Development Team
 * @date 2026-01-11
 *
 * @copyright Copyright (c) 2024-2026 Etrek Medical Imaging
 *
 * @see DetectorConfigurationWidget
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
namespace Etrek::Context { class IContextManager; }
class DetectorConfigurationWidget;

/**
 * @namespace Etrek::Device::Delegate
 * @brief Contains delegate classes for device configuration UI components.
 */
namespace Etrek::Device::Delegate
{
	namespace rpo = Etrek::Device::Repository;

	/**
	 * @class DetectorConfigurationDelegate
	 * @brief Delegate for digital detector configuration persistence.
	 *
	 * @details Implements the Delegate pattern to handle business logic for
	 *          detector configuration. Connects DetectorConfigurationWidget
	 *          to DeviceRepository for CRUD operations.
	 *
	 *          Key responsibilities:
	 *          - Receive detector settings from the UI widget
	 *          - Validate configuration changes
	 *          - Persist changes to database via DeviceRepository
	 *          - Handle apply/accept/reject actions from settings dialog
	 *
	 *          Detector configuration includes parameters like:
	 *          - Detector model and serial number
	 *          - Image matrix size
	 *          - Pixel pitch
	 *          - Communication settings
	 *
	 * @note This class is part of the Model-View-Delegate (MVD) pattern used
	 *       throughout the configuration system.
	 *
	 * @see DetectorConfigurationWidget
	 * @see DetectorConfigurationBuilder
	 * @see DeviceRepository
	 * @see IPageAction
	 *
	 * @ingroup Device
	 */
	class DetectorConfigurationDelegate :
	public QObject,
	public IDelegate,
	public IPageAction
	{
		Q_OBJECT
		Q_INTERFACES(IDelegate IPageAction)

	public:
		/**
		 * @brief Constructs a DetectorConfigurationDelegate.
		 *
		 * @param[in] widget Pointer to the associated UI widget.
		 * @param[in] repository Shared pointer to the device repository for database operations.
		 * @param[in] contextManager Weak pointer to the context manager for audit tracking.
		 * @param[in] parent Parent QObject for memory management.
		 */
		DetectorConfigurationDelegate(
			DetectorConfigurationWidget* widget,
			std::shared_ptr<rpo::DeviceRepository> repository,
			std::weak_ptr<Etrek::Context::IContextManager> contextManager,
			QObject* parent);

		/**
		 * @brief Returns the delegate's unique name identifier.
		 * @return QString containing "DetectorConfigurationDelegate".
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
		~DetectorConfigurationDelegate();

	private:
		DetectorConfigurationWidget* m_widget = nullptr;     ///< Pointer to the associated UI widget.
		std::shared_ptr<rpo::DeviceRepository> m_repository; ///< Repository for database operations.
		std::weak_ptr<Etrek::Context::IContextManager> m_contextManager; ///< Context manager for audit tracking.

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