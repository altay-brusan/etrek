#ifndef GENERATOR_H
#define GENERATOR_H

/**
 * @file Generator.h
 * @brief Entity representing an X-ray generator and its key attributes.
 */

#include <QString>
#include <QDate>
#include <QDateTime>
#include <QMetaType>

namespace Etrek::Device::Data::Entity {

    /**
     * @class Generator
     * @brief Describes an X-ray generator: identity, dates, outputs, and status.
     *
     * @note Default-constructed values reflect application defaults; Id == -1 means not persisted.
     */
    class Generator {
    public:
        int Id = -1;                           ///< Primary identifier.

        QString Manufacturer;                  ///< Manufacturer name.
        QString ModelNumber;                   ///< Model designation.
        QString PartNumber;                    ///< Part number (if applicable).
        QString SerialNumber;                  ///< Serial number.
        QString TypeNumber;                    ///< Type/code number (if applicable).
        QString TechnicalSpecifications;       ///< Free-text technical specs.

        QDate ManufactureDate;                 ///< Manufacturing date.
        QDate InstallationDate;                ///< Installation date.
        QDate CalibrationDate;                 ///< Last calibration date.

        int Output1 = -1;                      ///< ID of first associated X-ray tube/output (−1 if none).
        int Output2 = -1;                      ///< ID of second associated X-ray tube/output (−1 if none).

        bool IsActive = false;                 ///< Whether the generator is active/usable.
        bool IsOutput1Active = false;          ///< Whether output 1 is enabled.
        bool IsOutput2Active = false;          ///< Whether output 2 is enabled.

        QDateTime CreateDate;                  ///< Creation timestamp.
        QDateTime UpdateDate;                  ///< Last update timestamp.

        /// Default constructor.
        Generator() = default;

        /// Equality compares primary identifiers.
        bool operator==(const Generator& other) const noexcept { return Id == other.Id; }
    };

} // namespace Etrek::Device::Data::Entity

/// Qt metatype declaration (needed only if used with QVariant/queued connections).
Q_DECLARE_METATYPE(Etrek::Device::Data::Entity::Generator)

#endif // GENERATOR_H
