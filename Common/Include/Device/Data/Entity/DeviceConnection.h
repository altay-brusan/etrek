#ifndef DEVICECONNECTION_H
#define DEVICECONNECTION_H

#include <QString>
#include <QMetaType>
#include <QDate>
#include <QDateTime>
#include <QJsonObject> 
#include "Connector.h"
#include "ConnectionProtocol.h"


namespace Etrek::Device::Data::Entity {

    class DeviceConnection {
    public:
        int Id = -1;                          ///< Primary key
        int GeneratorId = -1;                 ///< Foreign key to generator
        int TubeId = -1;                      ///< Foreign key to x-ray tube
        int PosinioerId = -1;                 ///< Foreign key to positioner
        Connector Connector;                  ///< Connector
        ConnectionProtocol Protocol;          ///< Connection protocol
        QString InterfaceName;                ///< Optional connection name
        QJsonObject Parameters;               ///< pConnection Parameters
       
        DeviceConnection() = default;

        bool operator==(const DeviceConnection& other) const {
            return Id == other.Id;
        }
    };

} // namespace Etrek::Device::Data::Entity

Q_DECLARE_METATYPE(Etrek::Device::Data::Entity::DeviceConnection)

#endif // DEVICECONNECTION_H
