#ifndef POSITIONER_H
#define POSITIONER_H

/**
 * @file Positioner.h
 * @brief Declares the Positioner entity for scan protocol configuration.
 *
 * Represents a physical or semantic patient/imaging positioner definition
 * (e.g., "Head First Supine", "Prone", "PA", etc.). The type is Qt
 * meta-type enabled for QVariant, queued connections, and model/view roles.
 */

#include <QString>
#include <QMetaType>
#include <QDateTime>

namespace Etrek::ScanProtocol::Data::Entity {

    /**
     * @class Positioner
     * @brief Persistable entity describing a standardized positioner.
     *
     * Stores a unique display name and optional description with creation/update
     * timestamps. Equality prefers primary key comparison when available.
     */
    class Positioner {
    public:
        /**
         * @brief Surrogate primary key.
         *
         * Defaults to -1 to indicate an unsaved/transient instance.
         */
        int Id = -1;

        /**
         * @brief Unique human-readable name for the positioner.
         * @example "Head First Supine", "Feet First Prone", "PA"
         */
        QString PositionName;

        /**
         * @brief Optional descriptive text for the positioner.
         */
        QString Description;

        /**
         * @brief Creation timestamp (UTC unless specified).
         */
        QDateTime CreateDate;

        /**
         * @brief Last update timestamp (UTC unless specified).
         */
        QDateTime UpdateDate;

        /**
         * @brief Default constructor.
         */
        Positioner() = default;

        /**
         * @brief Equality operator.
         *
         * If both IDs are valid (>= 0), compares by @ref Id.
         * Otherwise, compares @ref PositionName case-insensitively to avoid
         * treating two distinct unsaved instances as equal unless names match.
         *
         * @param other The other Positioner to compare.
         * @return true if considered equal; otherwise false.
         */
        bool operator==(const Positioner& other) const noexcept {
            if (Id >= 0 && other.Id >= 0) return Id == other.Id;
            return PositionName.compare(other.PositionName, Qt::CaseInsensitive) == 0;
        }
    };

} // namespace Etrek::ScanProtocol::Data::Entity

/**
 * @brief Enables Positioner for use with Qt's meta-object system (e.g., QVariant).
 */
Q_DECLARE_METATYPE(Etrek::ScanProtocol::Data::Entity::Positioner)

#endif // POSITIONER_H
