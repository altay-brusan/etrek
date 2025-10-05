#ifndef VIEW_H
#define VIEW_H

/**
 * @file View.h
 * @brief Entity representing a radiographic view (e.g., "Head AP").
 *
 * @details
 * Maps to the `views` table. See individual fields for schema mapping notes.
 */

#include <QDateTime>
#include <QMetaType>
#include <QString>
#include <optional>

#include "BodyPart.h"
#include "ScanProtocolUtil.h"

namespace Etrek::ScanProtocol::Data::Entity {

/**
 * @class View
 * @brief Application entity for a radiographic view definition.
 *
 * @note Equality compares only the primary key (`Id`).
 */
class View {
public:
  int Id = -1;  ///< Primary key (`views.id`). `-1` indicates not yet persisted.
  QString Name; ///< Human-readable name (e.g., "Head AP").
  QString Description; ///< Optional free-text description.

  BodyPart BodyPart; ///< Foreign key to `body_parts` (denormalized here).

  // --- Patient orientation / position (DICOM-inspired) ---

  /// DICOM patient orientation code for the **row** direction (optional).
  /// See ::Etrek::ScanProtocol::PatientOrientation.
  std::optional<Etrek::ScanProtocol::PatientOrientation> PatientOrientationRow;

  /// DICOM patient orientation code for the **column** direction (optional).
  /// See ::Etrek::ScanProtocol::PatientOrientation.
  std::optional<Etrek::ScanProtocol::PatientOrientation> PatientOrientationCol;

  /// Patient position (DICOM 0018,5100-like). Optional in DB.
  /// See ::Etrek::ScanProtocol::PatientPosition.
  std::optional<Etrek::ScanProtocol::PatientPosition> PatientPosition;

  /// Projection profile. Defaults to AP|PA. Views support DUAL.
  /// See ::Etrek::ScanProtocol::ProjectionProfile.
  Etrek::ScanProtocol::ProjectionProfile ProjectionProfile =
      Etrek::ScanProtocol::ProjectionProfile::AP_PA;

  /// View position (DICOM 0018,5101). Optional (often unset for AXIAL, etc.).
  /// See ::Etrek::ScanProtocol::ViewPosition.
  std::optional<Etrek::ScanProtocol::ViewPosition> ViewPosition;

  /// Image laterality (DICOM 0020,0062). Optional.
  /// See ::Etrek::ScanProtocol::ImageLaterality.
  std::optional<Etrek::ScanProtocol::ImageLaterality> ImageLaterality;

  /// Optional clockwise image rotation in degrees (stored as TINYINT in DB).
  std::optional<int> ImageRotate;

  // --- Files / algorithms / UI hints ---

  QString IconFileLocation; ///< Path to an icon file (`icon_file_location`).
  QString CollimatorSize;   ///< Collimator size hint (`collimator_size`).
  QString ImageProcessingAlgorithm; ///< Recommended algorithm
                                    ///< (`image_processing_algorithm`).

  bool ImageHorizontalFlip =
      false; ///< UI hint: mirror image horizontally (`image_horizontal_flip`).

  /// Optional label mark (e.g., R/L/LAT/OBL).
  /// See ::Etrek::ScanProtocol::LabelMark.
  std::optional<Etrek::ScanProtocol::LabelMark> LabelMark;

  /// Optional label placement on the image.
  /// See ::Etrek::ScanProtocol::LabelPosition.
  std::optional<Etrek::ScanProtocol::LabelPosition> LabelPosition;

  /// Positioner linkage by name (FK by `position_name`). Stored as plain string
  /// to avoid circular deps.
  QString PositionName;

  bool IsActive = true; ///< Soft-delete flag (`is_active`).

  /// Default constructor.
  View() = default;

  /// @brief Equality compares primary key (`Id`).
  /// @param other The other view.
  /// @return `true` if `Id` matches; otherwise `false`.
  bool operator==(const View &other) const noexcept { return Id == other.Id; }

  /// @brief Inequality.
  /// @param other The other view.
  /// @return `true` if `Id` differs; otherwise `false`.
  bool operator!=(const View &other) const noexcept {
    return !(*this == other);
  }
};

} // namespace Etrek::ScanProtocol::Data::Entity

Q_DECLARE_METATYPE(Etrek::ScanProtocol::Data::Entity::View)

#endif // VIEW_H
