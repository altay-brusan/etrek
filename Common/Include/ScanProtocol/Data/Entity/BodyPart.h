#ifndef BODYPART_H
#define BODYPART_H

/**
 * @file BodyPart.h
 * @brief Declares the BodyPart entity for Scan Protocol configuration.
 *
 * Represents a selectable body part used by scan protocols and UI components,
 * linked to a broader @ref AnatomicRegion. This type is Qt meta-type enabled
 * for QVariant, queued connections, and model/view roles.
 */

#include <QString>
#include <QMetaType>
#include <QDate>       // (Optional) Currently unused; remove if unnecessary.
#include <QDateTime>   // (Optional) Currently unused; remove if unnecessary.
#include "AnatomicRegion.h"

 /// Scan Protocol data-layer entities for Etrek.
 /**
  * @namespace Etrek::ScanProtocol::Data::Entity
  * @brief Persistable domain objects backing scan protocol configuration.
  */
namespace Etrek::ScanProtocol::Data::Entity {

    /**
     * @class BodyPart
     * @brief Persistable entity describing a standardized body part.
     *
     * Stores display attributes and internal coding values for a body part used
     * across scan protocols, validation, and UI. Associated with an
     * @ref AnatomicRegion to support hierarchical organization in the application.
     * Equality compares only the surrogate key (@ref Id).
     */
    class BodyPart {
    public:
        /**
         * @brief Surrogate primary key.
         *
         * Defaults to -1 to indicate an unsaved/transient instance.
         */
        int Id = -1;

        /**
         * @brief Human-readable name of the body part.
         * @example "Liver", "Left Knee", "Cervical Spine"
         */
        QString Name;

        /**
         * @brief Internal code value used by Etrek to identify the body part.
         *
         * This is the unique identifier within the configured coding schema.
         */
        QString CodeValue;

        /**
         * @brief Coding schema identifier.
         *
         * Defaults to "ETREK". Consider mapping to standard coding schemes
         * (e.g., DICOM CID/LOINC/SNOMED) when applicable.
         */
        QString CodingSchema = "ETREK";

        /**
         * @brief Descriptive text for the body part (optional).
         *
         * Useful for tooltips, documentation, or audit displays.
         */
        QString Description;

        /**
         * @brief Associated anatomical region to which this body part belongs.
         *
         * Used for grouping and validation in scan protocol configuration.
         */
        AnatomicRegion Region;

        /**
         * @brief Indicates whether this body part is active/available for selection.
         */
        bool IsActive = true;

        /**
         * @brief Default constructor.
         */
        BodyPart() = default;

        /**
         * @brief Equality operator comparing by primary key only.
         * @param other The other instance to compare.
         * @return true if @ref Id values are equal; otherwise false.
         *
         * @note Payload fields (e.g., @ref Name, @ref CodeValue) are not compared.
         */
        bool operator==(const BodyPart& other) const noexcept {
            return Id == other.Id;
        }
    };

} // namespace Etrek::ScanProtocol::Data::Entity

/**
 * @brief Enables BodyPart to be used with Qt's meta-object system.
 *
 * Allows storage in QVariant, queued signal/slots, and model/view roles.
 */
Q_DECLARE_METATYPE(Etrek::ScanProtocol::Data::Entity::BodyPart)

#endif // BODYPART_H
