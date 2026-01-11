/**
 * @file GeneratorConfigurationDelegate.h
 * @brief Delegate for X-ray generator configuration persistence.
 *
 * @details This delegate handles the connection between the GeneratorConfigurationWidget
 *          and the DeviceRepository, ensuring generator configuration changes are
 *          persisted to the database.
 *
 * @author Etrek Development Team
 * @date 2026-01-11
 *
 * @copyright Copyright (c) 2024-2026 Etrek Medical Imaging
 *
 * @see GeneratorConfigurationWidget
 * @see DeviceRepository
 * @see IPageAction
 */

#pragma once

#include <QObject>
#include <QWidget>
#include "IDelegate.h"
#include "IPageAction.h"
#include "DeviceRepository.h"
#include "GeneratorConfigurationWidget.h"

namespace Etrek::Device::Delegate
{
	namespace rpo = Etrek::Device::Repository;

	/**
	 * @class GeneratorConfigurationDelegate
	 * @brief Delegate for X-ray generator configuration.
	 *
	 * @details Implements the Delegate pattern to handle business logic for
	 *          X-ray generator configuration. Connects GeneratorConfigurationWidget
	 *          to DeviceRepository for CRUD operations.
	 *
	 *          Key responsibilities:
	 *          - Receive generator settings from the UI widget
	 *          - Validate configuration changes
	 *          - Persist changes to database via DeviceRepository
	 *          - Handle apply/accept/reject actions from settings dialog
	 *
	 *          Generator configuration includes parameters like:
	 *          - KVP range and defaults
	 *          - mA/mAs settings
	 *          - Exposure time limits
	 *          - Communication protocol settings
	 *          - Safety interlocks configuration
	 *
	 * @see GeneratorConfigurationWidget
	 * @see GeneratorConfigurationBuilder
	 * @see DeviceRepository
	 * @see IPageAction
	 *
	 * @ingroup Device
	 */
	class GeneratorConfigurationDelegate :
	public QObject,
	public IDelegate,
	public IPageAction
	{
		Q_OBJECT
		Q_INTERFACES(IDelegate IPageAction)

	public:
		/**
		 * @brief Constructs a GeneratorConfigurationDelegate.
		 *
		 * @param[in] widget Pointer to the associated UI widget.
		 * @param[in] repository Shared pointer to the device repository.
		 * @param[in] parent Parent QObject for memory management.
		 */
		GeneratorConfigurationDelegate(GeneratorConfigurationWidget* widget, std::shared_ptr<rpo::DeviceRepository> repository, QObject* parent);

		/**
		 * @brief Returns the delegate's unique name identifier.
		 * @return QString containing "GeneratorConfigurationDelegate".
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
		~GeneratorConfigurationDelegate();

	private:
		GeneratorConfigurationWidget* m_widget;              ///< Pointer to the associated UI widget.
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