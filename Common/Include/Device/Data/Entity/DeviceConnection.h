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
    
    namespace dev = Etrek::Device;
    
    class DeviceConnection {
    public:
        int Id = -1;                          ///< Primary key
        int GeneratorId = -1;                 ///< Foreign key to generator
        int TubeId = -1;                      ///< Foreign key to x-ray tube
        int PosinionerId = -1;                ///< Foreign key to positioner
        dev::Connector Connector;             ///< Connector
        dev::ConnectionProtocol Protocol;     ///< Connection protocol
        QString InterfaceName;                ///< Optional connection name
        QJsonObject Parameters;               ///< Connection Parameters
       
        DeviceConnection() = default;

        bool operator==(const DeviceConnection& other) const noexcept {
            return Id == other.Id;
        }
    };

} // namespace Etrek::Device::Data::Entity

Q_DECLARE_METATYPE(Etrek::Device::Data::Entity::DeviceConnection)

#endif // DEVICECONNECTION_H
