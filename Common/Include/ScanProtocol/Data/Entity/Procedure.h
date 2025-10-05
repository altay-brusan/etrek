#ifndef PROCEDURE_H
#define PROCEDURE_H

/**
 * @file Procedure.h
 * @brief Declares the Procedure entity for scan protocol configuration.
 *
 * @details
 * Represents an imaging procedure (e.g., radiographic projection/series)
 * with optional links to @ref Etrek::ScanProtocol::Data::Entity::AnatomicRegion
 * and @ref Etrek::ScanProtocol::Data::Entity::BodyPart, plus hydrated
 * @ref Etrek::ScanProtocol::Data::Entity::View associations for UI and workflow.
 *
 * The type is Qt meta-type enabled for QVariant, queued connections, and
 * model/view roles.
 */

#include <QString>
#include <QMetaType>
#include <QVector>
#include <optional>

#include "AnatomicRegion.h"
#include "BodyPart.h"
#include "View.h"


namespace Etrek::ScanProtocol::Data::Entity {

    /**
     * @class Procedure
     * @brief Persistable entity describing an imaging procedure.
     *
     * Encapsulates display attributes, internal/standard codes, activation flags,
     * optional print settings, and (optionally) hydrated view relations. Equality
     * compares only the surrogate key (@ref Id).
     *
     * @note
     * - Optional foreign keys (e.g., @ref Region, @ref BodyPart) are represented
     *   with `std::optional<>` to clearly signal unset/nullable relationships.
     * - Print settings (@ref PrintOrientation, @ref PrintFormat) are optional and
     *   may be omitted when procedures do not define print behavior.
     * - @ref Views is a convenience aggregation and may be left empty by data
     *   access layers that defer hydration.
     *
     * @see Etrek::ScanProtocol::PrintOrientation
     * @see Etrek::ScanProtocol::PrintFormat
     * @see Etrek::ScanProtocol::Data::Entity::View
     */
    class Procedure {
    public:
        /**
         * @brief Surrogate primary key.
         *
         * Defaults to -1 to indicate an unsaved/transient instance.
         */
        int Id = -1;

        /**
         * @brief Human-readable display name of the procedure.
         * @example "CSpine AP", "Chest PA", "Knee LAT"
         */
        QString Name;

        /**
         * @brief Code value used to identify the procedure (standard or internal).
         *
         * May carry a DICOM/SNOMED code (e.g., "T-11100") or an internal code
         * depending on your deployment conventions.
         */
        QString CodeValue;

        /**
         * @brief Coding scheme identifier (e.g., "SRT" for SNOMED RT).
         *
         * Defaults to "SRT". Adjust to match your terminology system when
         * interoperating with standardized vocabularies.
         */
        QString CodingSchema = "SRT";

        /**
         * @brief Human-readable meaning/label of the code.
         *
         * Often mirrors @ref Name for internal schemas.
         */
        QString CodeMeaning;

        /**
         * @brief Indicates whether acquisition should be rendered/printed at true size.
         *
         * When true, systems that support true-size output should avoid scaling.
         */
        bool IsTrueSize = false;

        /**
         * @brief Optional print orientation for render/print workflows.
         *
         * When unset, downstream logic should apply a default.
         * @see Etrek::ScanProtocol::PrintOrientation
         */
        std::optional<Etrek::ScanProtocol::PrintOrientation> PrintOrientation;

        /**
         * @brief Optional print format/layout (e.g., film/sheet size).
         *
         * When unset, downstream logic should apply a default.
         * @see Etrek::ScanProtocol::PrintFormat
         */
        std::optional<Etrek::ScanProtocol::PrintFormat> PrintFormat;

        /**
         * @brief Optional association to an anatomic region.
         *
         * Leave empty for procedures that are not region-specific.
         */
        std::optional<AnatomicRegion> Region;

        /**
         * @brief Optional association to a body part.
         *
         * Leave empty for procedures that are not body-part specific.
         */
        std::optional<BodyPart> BodyPart;

        /**
         * @brief Activation flag controlling visibility/availability in UI.
         */
        bool IsActive = true;

        /**
         * @brief Convenience collection of views for this procedure.
         *
         * Represents a many-to-many relation. May be left empty if the repository
         * or service layer doesn’t hydrate view associations.
         */
        QVector<View> Views;

        /**
         * @brief Default constructor.
         */
        Procedure() = default;

        /**
         * @brief Equality operator comparing by primary key only.
         * @param other Another Procedure to compare with.
         * @return true if both have the same @ref Id; otherwise false.
         */
        bool operator==(const Procedure& other) const noexcept { return Id == other.Id; }

        /**
         * @brief Inequality operator (negation of @ref operator==).
         */
        bool operator!=(const Procedure& other) const noexcept { return !(*this == other); }
    };

} // namespace Etrek::ScanProtocol::Data::Entity

/**
 * @brief Enables Procedure for use with Qt's meta-object system (e.g., QVariant).
 */
Q_DECLARE_METATYPE(Etrek::ScanProtocol::Data::Entity::Procedure)

#endif // PROCEDURE_H
