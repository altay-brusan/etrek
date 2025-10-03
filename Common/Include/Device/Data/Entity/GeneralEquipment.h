#ifndef GENERALEQUIPMENT_H
#define GENERALEQUIPMENT_H

#include <QString>
#include <QDate>
#include <QTime>
#include <QMetaType>
#include "Institution.h"

namespace Etrek::Device::Data::Entity {

    class GeneralEquipment {
    public:
        int Id = -1;                               // id INT AUTO_INCREMENT PRIMARY KEY
        QString DeviceSerialNumber;                // device_serial_number VARCHAR(64) NOT NULL UNIQUE
        QString DeviceUID;                         // device_uid VARCHAR(64)
        QString Manufacturer;                      // manufacturer VARCHAR(128)
        QString ModelName;                         // manufacturers_model_name VARCHAR(128)
        QString StationName;                       // station_name VARCHAR(64)
        Institution Institution;                   // institution_id INT, FK to institutions.id
        QString DepartmentName;                    // department_name VARCHAR(128)
        QDate DateOfLastCalibration;               // date_of_last_calibration DATE
        QTime TimeOfLastCalibration;               // time_of_last_calibration TIME
        QDate DateOfManufacture;                   // date_of_manufacture DATE
        QString SoftwareVersions;                  // software_versions TEXT
        QString GantryId;                          // gantry_id VARCHAR(255)
        bool IsActive = true;                      // is_active BOOLEAN DEFAULT TRUE

        GeneralEquipment() = default;

        // Equality operator based on primary key
        bool operator==(const GeneralEquipment& other) const {
            return Id == other.Id;
        }
    };

} // namespace Etrek::Device::Data::Entity

Q_DECLARE_METATYPE(Etrek::Device::Data::Entity::GeneralEquipment)

#endif // GENERALEQUIPMENT_H
