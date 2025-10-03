#ifndef INSTITUTION_H
#define INSTITUTION_H

#include <QString>
#include <QMetaType>

namespace Etrek::Device::Data::Entity {

    class Institution {
    public:
        int Id = -1;                                // id INT AUTO_INCREMENT PRIMARY KEY
        QString Name;                               // name VARCHAR(128) NOT NULL UNIQUE
        QString InstitutionType;                    // institution_type VARCHAR(128)
        QString InstitutionId;                      // institution_type VARCHAR(128)
        QString DepartmentName;                     // department_name VARCHAR(128)
        QString Address;                            // address VARCHAR(512) NULL
        QString ContactInformation;                 // contact_information VARCHAR(500)
        bool IsActive = true;                       // is_active BOOLEAN DEFAULT TRUE

        Institution() = default;

        // Equality comparison based on ID
        bool operator==(const Institution& other) const {
            return Id == other.Id;
        }
    };

} // namespace Etrek::Device::Data::Entity

Q_DECLARE_METATYPE(Etrek::Device::Data::Entity::Institution)

#endif // INSTITUTION_H
