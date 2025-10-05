#ifndef VIEW_H
#define VIEW_H

/**
 * @file View.h
 * @brief Declares the View entity representing a radiographic projection/view.
 *
 * Represents a radiographic view definition (e.g., "Head AP") including
 * projection, orientation, laterality, and other metadata for protocol
 * configuration and UI display.
 */

#include <QDateTime>
#include <QMetaType>
#include <QString>
#include <optional>

#include "BodyPart.h"
#include "ScanProtocolUtil.h"

namespace Etrek::ScanProtocol::Data::Entity {

	namespace sp = Etrek::ScanProtocol;

    /**
     * @class View
     * @brief Application entity for a radiographic view definition.
     *
     * Encapsulates metadata describing a specific radiographic projection, including
     * DICOM-inspired orientation, projection profile, label placement, and UI hints.
     *
     * @note Equality compares only the surrogate key (@ref Id).
     */
    class View {
    public:
        /** @brief Primary key (`views.id`). `-1` indicates not yet persisted. */
        int Id = -1;

        /** @brief Human-readable name (e.g., "Head AP"). */
        QString Name;

        /** @brief Optional free-text description of the view. */
        QString Description;

        /** @brief Associated body part (foreign key to `body_parts`). */
        BodyPart BodyPart;

        // --- Patient orientation / position (DICOM-inspired) ---

        /**
         * @brief DICOM patient orientation code for the **row** direction (optional).
         * @see Etrek::ScanProtocol::PatientOrientation
         */
        std::optional<sp::PatientOrientation> PatientOrientationRow;

        /**
         * @brief DICOM patient orientation code for the **column** direction (optional).
         * @see Etrek::ScanProtocol::PatientOrientation
         */
        std::optional<sp::PatientOrientation> PatientOrientationCol;

        /**
         * @brief Patient position (DICOM 0018,5100-like). Optional in DB.
         * @see Etrek::ScanProtocol::PatientPosition
         */
        std::optional<sp::PatientPosition> PatientPosition;

        /**
         * @brief Projection profile for the view (default: AP/PA).
         * @see Etrek::ScanProtocol::ProjectionProfile
         */
        sp::ProjectionProfile ProjectionProfile =
            sp::ProjectionProfile::AP_PA;

        /**
         * @brief View position (DICOM 0018,5101). Optional.
         * Often unset for axial or non-standard projections.
         * @see Etrek::ScanProtocol::ViewPosition
         */
        std::optional<sp::ViewPosition> ViewPosition;

        /**
         * @brief Image laterality (DICOM 0020,0062). Optional.
         * @see Etrek::ScanProtocol::ImageLaterality
         */
        std::optional<sp::ImageLaterality> ImageLaterality;

        /**
         * @brief Optional clockwise image rotation in degrees.
         * Stored as a small integer (TINYINT) in the database.
         */
        std::optional<int> ImageRotate;

        // --- Files / algorithms / UI hints ---

        /** @brief Path to an icon file (`icon_file_location`). */
        QString IconFileLocation;

        /** @brief Collimator size hint (`collimator_size`). */
        QString CollimatorSize;

        /** @brief Recommended image processing algorithm (`image_processing_algorithm`). */
        QString ImageProcessingAlgorithm;

        /** @brief UI hint: mirror image horizontally (`image_horizontal_flip`). */
        bool ImageHorizontalFlip = false;

        /**
         * @brief Optional label mark (e.g., "R", "L", "LAT", "OBL").
         * @see Etrek::ScanProtocol::LabelMark
         */
        std::optional<sp::LabelMark> LabelMark;

        /**
         * @brief Optional label placement position on the image.
         * @see Etrek::ScanProtocol::LabelPosition
         */
        std::optional<sp::LabelPosition> LabelPosition;

        /**
         * @brief Positioner linkage by name (foreign key by `position_name`).
         *
         * Stored as plain string to avoid circular dependencies with Positioner.
         */
        QString PositionName;

        /** @brief Indicates whether the view is active (`is_active`). */
        bool IsActive = true;

        /** @brief Default constructor. */
        View() = default;

        /**
         * @brief Equality operator comparing by @ref Id.
         * @param other The other view to compare.
         * @return true if both have the same @ref Id; otherwise false.
         */
        bool operator==(const View& other) const noexcept { return Id == other.Id; }

        /**
         * @brief Inequality operator (negation of @ref operator==).
         * @param other The other view to compare.
         * @return true if IDs differ; otherwise false.
         */
        bool operator!=(const View& other) const noexcept { return !(*this == other); }
    };

} // namespace Etrek::ScanProtocol::Data::Entity

/**
 * @brief Enables View for use with Qt's meta-object system (e.g., QVariant).
 */
Q_DECLARE_METATYPE(Etrek::ScanProtocol::Data::Entity::View)

#endif // VIEW_H
