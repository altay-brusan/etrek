#ifndef XRAYTUBE_H
#define XRAYTUBE_H

#include <QString>
#include <QDate>
#include <QMetaType>
#include "DevicePosition.h"

namespace Etrek::Device::Data::Entity {

	namespace dev = Etrek::Device;

    /**
     * @class XRayTube
     * @brief Represents an X-ray tube entity, matching the `xray_tubes` table.
     *
     * Includes technical specs, manufacturer info, position, filters,
     * installation, calibration dates, and status flags.
     */
    class XRayTube {
    public:
        int Id = -1;                             ///< Primary key
        QString Manufacturer;
        QString ModelNumber;
        QString PartNumber;
        QString SerialNumber;                    ///< Unique
        QString TypeNumber;
        QString FocalSpot;
        int AnodeHeatCapacity = 0;               ///< in HU
        int CoolingRate = 0;                     ///< in HU/min
        int MaxVoltage = 0;                      ///< in kV
        int MaxCurrent = 0;                      ///< in mA
        QString PowerRange;                      ///< in kW
        QString TubeName;                        ///< e.g., "Ceiling Tube"
        dev::DevicePosition Position;
        QString TubeFilter;                  

        bool IsActive = false;

        QDate ManufactureDate;
        QDate InstallationDate;
        QDate CalibrationDate;

        QDateTime CreateDate;
        QDateTime UpdateDate;

        XRayTube() = default;

        bool operator==(const XRayTube& other) const {
            return Id == other.Id;
        }       
    };

} // namespace Etrek::Device::Data::Entity

Q_DECLARE_METATYPE(Etrek::Device::Data::Entity::XRayTube)

#endif // XRAYTUBE_H
