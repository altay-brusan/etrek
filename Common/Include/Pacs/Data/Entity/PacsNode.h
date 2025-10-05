#ifndef PACSNODE_H
#define PACSNODE_H

#include "PacsEntityType.h"
#include <QDate>
#include <QDateTime>
#include <QMetaType>
#include <QString>

using namespace Etrek::Pacs;
namespace Etrek::Pacs::Data::Entity {

    class PacsNode {
    public:
        int Id = -1;                                    ///< Primary key       
        PacsEntityType Type = PacsEntityType::Archive;  ///< Type of PACS entity (Archive, MPPS, etc.)
        QString HostName;                               ///< short heading of the comment
        QString HostIp;                                 ///< full image note
        int Port = 0;                                   ///< Port number
        QString CalledAet;                              ///< is reject comment flag
        QString CallingAet;

  PacsNode() = default;

  bool operator==(const PacsNode &other) const { return Id == other.Id; }
};

} // namespace Etrek::Pacs::Data::Entity

Q_DECLARE_METATYPE(Etrek::Pacs::Data::Entity::PacsNode)

#endif // !PACSNODE_H
