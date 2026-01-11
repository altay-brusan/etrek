/**
 * @file ImageCommentConfigurationDelegate.h
 * @brief Delegate for DICOM image comment configuration persistence.
 *
 * @details This delegate handles the connection between the ImageCommentConfigurationWidget
 *          and the ImageCommentRepository, ensuring image comment templates and settings
 *          are persisted to the database.
 *
 * @author Etrek Development Team
 * @date 2026-01-11
 *
 * @copyright Copyright (c) 2024-2026 Etrek Medical Imaging
 *
 * @see ImageCommentConfigurationWidget
 * @see ImageCommentRepository
 * @see IPageAction
 */

#ifndef IMAGECOMMENTCONFIGURATIONDELEGATE_H
#define IMAGECOMMENTCONFIGURATIONDELEGATE_H

#include <QObject>
#include <QWidget>
#include <memory>
#include "IDelegate.h"
#include "IPageAction.h"

namespace Etrek::Dicom::Repository { class ImageCommentRepository; }
class ImageCommentConfigurationWidget;

/**
 * @namespace Etrek::Dicom::Delegate
 * @brief Contains delegate classes for DICOM-related UI components.
 */
namespace Etrek::Dicom::Delegate
{
    namespace rpo = Etrek::Dicom::Repository;

    /**
     * @class ImageCommentConfigurationDelegate
     * @brief Delegate for DICOM image comment template configuration.
     *
     * @details Implements the Delegate pattern to handle business logic for
     *          image comment configuration. Connects ImageCommentConfigurationWidget
     *          to ImageCommentRepository for CRUD operations.
     *
     *          Key responsibilities:
     *          - Receive image comment templates from the UI widget
     *          - Validate comment template syntax
     *          - Persist changes to database via ImageCommentRepository
     *          - Handle apply/accept/reject actions from settings dialog
     *
     *          Image comment configuration includes:
     *          - Comment templates with placeholders
     *          - Default comments for different image types
     *          - Auto-comment rules based on examination type
     *
     * @see ImageCommentConfigurationWidget
     * @see ImageCommentConfigurationBuilder
     * @see ImageCommentRepository
     * @see IPageAction
     *
     * @ingroup Dicom
     */
    class ImageCommentConfigurationDelegate :
	public QObject,
	public IDelegate,
	public IPageAction
    {
        Q_OBJECT
        Q_INTERFACES(IDelegate IPageAction)

    public:
        /**
         * @brief Constructs an ImageCommentConfigurationDelegate.
         *
         * @param[in] widget Pointer to the associated UI widget.
         * @param[in] repository Shared pointer to the image comment repository.
         * @param[in] parent Parent QObject for memory management.
         */
        ImageCommentConfigurationDelegate(
            ImageCommentConfigurationWidget* widget,
            std::shared_ptr<rpo::ImageCommentRepository> repository,
            QObject* parent = nullptr);

        /**
         * @brief Returns the delegate's unique name identifier.
         * @return QString containing "ImageCommentConfigurationDelegate".
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
		~ImageCommentConfigurationDelegate();

    private:
        ImageCommentConfigurationWidget* m_widget = nullptr;         ///< Pointer to the associated UI widget.
        std::shared_ptr<rpo::ImageCommentRepository> m_repository;   ///< Repository for database operations.

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

#endif // IMAGECOMMENTCONFIGURATIONDELEGATE_H
