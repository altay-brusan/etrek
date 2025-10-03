#ifndef CONNECTORENUMS_H
#define CONNECTORENUMS_H

#include <QString>
#include <QVector>
#include <QMetaType>

namespace Etrek::Device {

    enum class Connector {
        SyncBox,
        Direct
    };

    class ConnectorUtils {
    public:
        // ---------- To String ----------
        static QString toString(Connector connector) {
            switch (connector) {
            case Connector::SyncBox: return "SyncBox";
            case Connector::Direct:  return "Direct";
            }
            return "Unknown";
        }

        // ---------- From String ----------
        static Connector parse(const QString& str) {
            if (str.compare("SyncBox", Qt::CaseInsensitive) == 0) return Connector::SyncBox;
            if (str.compare("Direct", Qt::CaseInsensitive) == 0) return Connector::Direct;
            return Connector::Direct; // default fallback
        }

        // ---------- All as strings (for UI dropdowns etc.) ----------
        static QVector<QString> allStrings() {
            return { "SyncBox", "Direct" };
        }
    };

} // namespace Etrek::Device

Q_DECLARE_METATYPE(Etrek::Device::Connector)

#endif // CONNECTORENUMS_H
