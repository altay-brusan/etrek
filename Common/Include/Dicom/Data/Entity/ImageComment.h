#ifndef IMAGECOMMENT_H
#define IMAGECOMMENT_H

/**
 * @file ImageComment.h
 * @brief Declares the ImageComment entity used to store image-level comments/notes.
 *
 * This header defines a lightweight value type representing a comment attached
 * to an image (e.g., DICOM object). It is Qt meta-type enabled for use in
 * QVariant, queued signal/slots, and model/view code.
 */

#include <QString>
#include <QMetaType>
#include <QDate>       // (Optional) Currently not used; remove if unnecessary.
#include <QDateTime>

 /// Top-level namespace for Etrek components.
 /**
  * @namespace Etrek::Dicom::Data::Entity
  * @brief Data-layer DICOM entities (persistable domain objects).
  */
namespace Etrek::Dicom::Data::Entity {

    /**
     * @class ImageComment
     * @brief Persistable entity representing a user-facing image comment/note.
     *
     * Typical uses include attaching short headings and full-text notes to an image,
     * along with timestamps for creation and last update. The equality operator
     * compares by primary key (`Id`) only.
     */
    class ImageComment {
    public:
        /**
         * @brief Surrogate primary key.
         *
         * Defaults to `-1` to indicate an unsaved/transient instance.
         */
        int Id = -1;

        /**
         * @brief Short, user-facing heading/title for the comment.
         *
         * Intended for summary displays (tables, cards, list items).
         */
        QString Heading;

        /**
         * @brief Full comment text (free-form note).
         *
         * Store the complete narrative or annotation associated with the image.
         */
        QString Comment;

        /**
         * @brief Flag indicating the comment is a “reject” reason/annotation.
         *
         * Stored as a string for backward compatibility (e.g., "true"/"false",
         * "1"/"0", or coded values). Consider migrating to a boolean or enum
         * when API compatibility allows.
         */
        QString IsRejectComment;

        /**
         * @brief Timestamp for when the entity was created (UTC unless specified).
         */
        QDateTime CreateDate;

        /**
         * @brief Timestamp for the last modification (UTC unless specified).
         */
        QDateTime UpdateDate;

        /**
         * @brief Default constructor.
         */
        ImageComment() = default;

        /**
         * @brief Equality operator comparing by primary key.
         * @param other The other instance to compare against.
         * @return `true` if both objects have the same @ref Id; otherwise `false`.
         *
         * @note Payload fields (e.g., @ref Heading, @ref Comment) are not compared.
         */
        bool operator==(const ImageComment& other) const {
            return Id == other.Id;
        }
    };

} // namespace Etrek::Dicom::Data::Entity

/**
 * @brief Enables ImageComment to be used with Qt's meta-object system.
 *
 * Allows use in QVariant, queued signal/slot connections, and model/view roles.
 */
Q_DECLARE_METATYPE(Etrek::Dicom::Data::Entity::ImageComment)

#endif // IMAGECOMMENT_H
