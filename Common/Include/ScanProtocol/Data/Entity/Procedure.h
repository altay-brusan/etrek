#ifndef PROCEDURE_H
#define PROCEDURE_H

/**
 * @file Procedure.h
 * @brief Entity representing an imaging procedure definition.
 *
 * @details
 * Maps to the `procedures` table. View associations are maintained via
 * the `procedure_views` join table (see ProcedureView).
 */

#include <QString>
#include <QMetaType>
#include <QVector>
#include <optional>

#include "AnatomicRegion.h"
#include "BodyPart.h"
#include "View.h"
#include "ScanProtocolUtil.h"

namespace Etrek::ScanProtocol::Data::Entity {

    /**
     * @class Procedure
     * @brief Application entity for an imaging procedure.
     */
    class Procedure {
    public:
        int     Id = -1;             ///< Primary key (`procedures.id`). `-1` = not persisted.
        QString Name;                ///< Display name (e.g., "CSpine AP").
        QString CodeValue;           ///< DICOM/inside Etrek code (e.g., "T-11100").
        QString CodingSchema = "SRT";///< Coding scheme (typically "SRT").
        QString CodeMeaning;         ///< Human-readable meaning.

        bool IsTrueSize = false;     ///< Maps to `is_true_size`.

        // Optional in DB → wrap as std::optional
        std::optional<Etrek::ScanProtocol::PrintOrientation> PrintOrientation; ///< `print_orientation`
        std::optional<Etrek::ScanProtocol::PrintFormat>      PrintFormat;      ///< `print_format`

        // Optional FKs; leave empty for custom procedures
        std::optional<AnatomicRegion> Region;    ///< `anatomic_region_id` (optional)
        std::optional<BodyPart>       BodyPart;  ///< `body_part_id` (optional)

        bool IsActive = true;         ///< `is_active`

        // Convenience: hydrated views for this procedure (optional to fill in repos)
        QVector<View> Views;          ///< Many-to-many via `procedure_views`

        Procedure() = default;

        /// @brief Equality by Id.
        bool operator==(const Procedure& other) const noexcept { return Id == other.Id; }
        bool operator!=(const Procedure& other) const noexcept { return !(*this == other); }
    };

} // namespace Etrek::ScanProtocol::Data::Entity

Q_DECLARE_METATYPE(Etrek::ScanProtocol::Data::Entity::Procedure)

#endif // PROCEDURE_H
