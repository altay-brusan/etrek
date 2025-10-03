#ifndef CONNECTIONTYPEENUMS_H
#define CONNECTIONTYPEENUMS_H

#include <QString>
#include <QVector>
#include <QMetaType>

namespace Etrek::Device {

    enum class ConnectionProtocol {
        RS_232,
        RS_485,
        CAN,
        LAN,
        MODBUS_RTU,
        MODBUS_TCP,
        WIFI,
        USB,
        ANALOG
    };

    class ConnectionProtocolUtils {
    public:
        // ---------- To String ----------
        static QString toString(ConnectionProtocol type) {
            switch (type) {
            case ConnectionProtocol::RS_232:      return "RS232";
            case ConnectionProtocol::RS_485:      return "RS485";
            case ConnectionProtocol::CAN:         return "CAN";
            case ConnectionProtocol::LAN:         return "LAN";
            case ConnectionProtocol::MODBUS_RTU:  return "Modbus RTU";
            case ConnectionProtocol::MODBUS_TCP:  return "Modbus TCP";
            case ConnectionProtocol::WIFI:        return "WiFi";
            case ConnectionProtocol::USB:         return "USB";
            case ConnectionProtocol::ANALOG:      return "Analog";
            }
            return "Unknown";
        }

        // ---------- From String ----------
        static ConnectionProtocol parse(const QString& str) {
            if (str.compare("RS232", Qt::CaseInsensitive) == 0) return ConnectionProtocol::RS_232;
            if (str.compare("RS485", Qt::CaseInsensitive) == 0) return ConnectionProtocol::RS_485;
            if (str.compare("CAN", Qt::CaseInsensitive) == 0) return ConnectionProtocol::CAN;
            if (str.compare("LAN", Qt::CaseInsensitive) == 0) return ConnectionProtocol::LAN;
            if (str.compare("Modbus RTU", Qt::CaseInsensitive) == 0) return ConnectionProtocol::MODBUS_RTU;
            if (str.compare("Modbus TCP", Qt::CaseInsensitive) == 0) return ConnectionProtocol::MODBUS_TCP;
            if (str.compare("WiFi", Qt::CaseInsensitive) == 0) return ConnectionProtocol::WIFI;
            if (str.compare("USB", Qt::CaseInsensitive) == 0) return ConnectionProtocol::USB;
            if (str.compare("Analog", Qt::CaseInsensitive) == 0) return ConnectionProtocol::ANALOG;

            return ConnectionProtocol::RS_232; // default fallback
        }

        // ---------- All as strings (for UI dropdowns etc.) ----------
        static QVector<QString> allStrings() {
            return {
                "RS232",
                "RS485",
                "CAN",
                "LAN",
                "Modbus RTU",
                "Modbus TCP",
                "WiFi",
                "USB",
                "Analog"
            };
        }
    };

} // namespace Etrek::Device

Q_DECLARE_METATYPE(Etrek::Device::ConnectionProtocol)

#endif // CONNECTIONTYPEENUMS_H
