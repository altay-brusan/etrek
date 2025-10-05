#ifndef BODYPART_H
#define BODYPART_H

#include <QString>
#include <QMetaType>
#include <QDate>
#include <QDateTime>
#include "AnatomicRegion.h"

namespace ent = Etrek::ScanProtocol::Data::Entity;

namespace Etrek::ScanProtocol::Data::Entity {

    class BodyPart {
    public:
        int Id = -1;                        ///< Primary key
        QString Name;                       ///< body part name
        QString CodeValue;                  ///< Unique code used inside Etrek to identify body part
		QString CodingSchema = "ETREK";     ///< Internal coding schema
        QString Description;                ///< Body part descrioption
		ent::AnatomicRegion Region;         ///< Associated anatomic region
		bool IsActive = true;
        
        BodyPart() = default;

        bool operator==(const BodyPart& other) const noexcept {
            return Id == other.Id;
        }
    };

} // namespace Etrek::ScanProtocol::Data::Entity::BodyPart

Q_DECLARE_METATYPE(Etrek::ScanProtocol::Data::Entity::BodyPart)

#endif // BODYPART_H
