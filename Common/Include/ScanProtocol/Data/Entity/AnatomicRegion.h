#ifndef ANATOMICREGION_H
#define ANATOMICREGION_H

/**
 * @file AnatomicRegion.h
 * @brief Declares the AnatomicRegion entity for Scan Protocol configuration.
 *
 * Represents an anatomical region/body part used in scan protocols and UI
 * selection lists. The type is Qt meta-type enabled for QVariant, queued
 * signal/slot delivery, and model/view roles.
 */

#include <QString>
#include <QMetaType>

 /// Scan Protocol data-layer entities for Etrek.
 /**
  * @namespace Etrek::ScanProtocol::Data::Entity
  * @brief Persistable domain objects backing scan protocol configuration.
  */
namespace Etrek::ScanProtocol::Data::Entity {

    /**
     * @class AnatomicRegion
     * @brief Persistable entity describing a standardized anatomical region.
     *
     * Stores display attributes and internal coding values for a body part used
     * across scan protocols, validation, and UI. Equality compares only the
     * surrogate key (@ref Id).
     */
    class AnatomicRegion {
    public:
        /**
         * @brief Surrogate primary key.
         *
         * Defaults to -1 to indicate an unsaved/transient instance.
         */
        int Id = -1;

        /**
         * @brief Human-readable name of the anatomical region.
         * @example "Chest", "Abdomen", "Pelvis"
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
         * @brief Human-readable meaning/label of the code.
         *
         * Defaults to "ETREK". Often mirrors @ref Name for internal schemas.
         */
        QString CodeMeaning = "ETREK";

        /**
         * @brief Descriptive text for the anatomical region.
         *
         * Optional; may be used for tooltips or documentation.
         */
        QString Description;

        /**
         * @brief Relative ordering for UI lists and pickers.
         *
         * Lower numbers typically appear first.
         */
        int DisplayOrder = 0;

        /**
         * @brief Indicates whether this region is active/available for selection.
         */
        bool IsActive = true;

        /**
         * @brief Default constructor.
         */
        AnatomicRegion() = default;

        /**
         * @brief Equality operator comparing by primary key only.
         * @param other The other instance to compare.
         * @return true if @ref Id values are equal; otherwise false.
         *
         * @note Payload fields (e.g., @ref Name, @ref CodeValue) are not compared.
         */
        bool operator==(const AnatomicRegion& other) const noexcept {
            return Id == other.Id;
        }
    };

} // namespace Etrek::ScanProtocol::Data::Entity

/**
 * @brief Enables AnatomicRegion to be used with Qt's meta-object system.
 *
 * Allows storage in QVariant, queued signal/slots, and model/view roles.
 */
Q_DECLARE_METATYPE(Etrek::ScanProtocol::Data::Entity::AnatomicRegion)

#endif // ANATOMICREGION_H
