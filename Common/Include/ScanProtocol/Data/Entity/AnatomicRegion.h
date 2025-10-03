#ifndef ANATOMICREGION_H
#define ANATOMICREGION_H

#include <QString>
#include <QMetaType>
#include <QDate>
#include <QDateTime>

namespace Etrek::ScanProtocol::Data::Entity {

    class AnatomicRegion {
    public:
        int Id = -1;                        ///< Primary key
        QString Name;                       ///< Anatomic region name
        QString CodeValue;                  ///< Unique code used inside Etrek to identify body part
        QString CodingSchema = "ETREK";     ///< Internal coding schema
        QString CodeMeaning = "ETREK";      ///< Internal meaning of code 
        QString Description;                ///< Anatomic region descrioption
		int DisplayOrder = 0;               ///< Display order
        bool IsActive = true;

        AnatomicRegion() = default;

        bool operator==(const AnatomicRegion& other) const {
            return Id == other.Id;
        }
    };

} // namespace Etrek::ScanProtocol::Data::Entity::AnatomicRegion

Q_DECLARE_METATYPE(Etrek::ScanProtocol::Data::Entity::AnatomicRegion)

#endif // ANATOMICREGION_H
