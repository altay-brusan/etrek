#ifndef GENERATOR_H
#define GENERATOR_H

#include <QString>
#include <QDate>
#include <QMetaType>
#include <QVector>
#include "XRayTube.h"
#include "GeneralEquipment.h"

namespace Etrek::Device::Data::Entity {

    /**
     * @class Generator
     * @brief Represents an X-ray generator entity, matching the `generators` table.
     *
     * Each generator has identifying and descriptive properties, outputs,
     * installation dates, and technical specifications.
     * The generator list is fixed and should only be modified by the software provider.
     */
    class Generator {
    public:
        int Id = -1;                             ///< Primary key
        QString Manufacturer;                    ///< Manufacturer name
        QString ModelNumber;
        QString PartNumber;
        QString SerialNumber;
        QString TypeNumber;
        QString TechnicalSpecifications;
        QDate ManufactureDate;
        QDate InstallationDate;
        QDate CalibrationDate;

        int Output1 = -1;                        ///< First XRayTube object
        int Output2 = -1;                        ///< Second XRayTube object
        
        bool IsActive = false;
        bool IsOutput1Active = false;
        bool IsOutput2Active = false;

        QDateTime CreateDate;
        QDateTime UpdateDate;

        Generator() = default;

        bool operator==(const Generator& other) const {
            return Id == other.Id;
        }
    };

} // namespace Etrek::Device::Data::Entity

Q_DECLARE_METATYPE(Etrek::Device::Data::Entity::Generator)

#endif // GENERATOR_H
