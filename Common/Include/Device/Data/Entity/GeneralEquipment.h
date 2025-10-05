#ifndef GENERALEQUIPMENT_H
#define GENERALEQUIPMENT_H

/**
 * @file GeneralEquipment.h
 * @brief Entity representing general equipment metadata for a device.
 */

#include <QString>
#include <QDate>
#include <QTime>
#include <QMetaType>
#include "Institution.h"

namespace Etrek::Device::Data::Entity {

    /**
     * @class GeneralEquipment
     * @brief Persistent metadata about a device and its environment.
     *
     * @note Id == -1 indicates a non-persisted instance.
     */
    class GeneralEquipment {
    public:
        int Id = -1;                 ///< Primary identifier.

        QString DeviceSerialNumber;  ///< Device serial number.
        QString DeviceUID;           ///< Optional unique device identifier.
        QString Manufacturer;        ///< Manufacturer name.
        QString ModelName;           ///< Model name.
        QString StationName;         ///< Station or host name.
        Institution Institution;     ///< Owning/operating institution.
        QString DepartmentName;      ///< Department name.

        QDate DateOfLastCalibration; ///< Date of last calibration.
        QTime TimeOfLastCalibration; ///< Time of last calibration.
        QDate DateOfManufacture;     ///< Date of manufacture.

        QString SoftwareVersions;    ///< Software version information.
        QString GantryId;            ///< Gantry identifier (if applicable).

        bool IsActive = true;        ///< Whether the device is active.

        /// Default constructor.
        GeneralEquipment() = default;

        /// Equality compares primary identifiers.
        bool operator==(const GeneralEquipment& other) const noexcept {
            return Id == other.Id;
        }
    };

} // namespace Etrek::Device::Data::Entity

/// Qt metatype declaration (needed only if used with QVariant/queued connections).
Q_DECLARE_METATYPE(Etrek::Device::Data::Entity::GeneralEquipment)

#endif // GENERALEQUIPMENT_H
