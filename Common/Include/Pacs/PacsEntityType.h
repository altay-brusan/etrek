#ifndef PACSENTITYTYPE_H
#define PACSENTITYTYPE_H

#include <QString>
#include <QVector>
#include <QMetaType>

namespace Etrek::Pacs {

    enum class PacsEntityType {
        Archive,
        MPPS
    };

    class PacsEntityTypeUtils {
    public:
        // ---------- To String ----------
        static QString toString(PacsEntityType type) {
            switch (type) {
            case PacsEntityType::Archive:   return "Archive";
            case PacsEntityType::MPPS:      return "MPPS";            
            }
            return "Unknown";
        }

        // ---------- From String ----------
        static PacsEntityType parse(const QString& str) {
            if (str.compare("Archive", Qt::CaseInsensitive) == 0) return PacsEntityType::Archive;
            if (str.compare("MPPS", Qt::CaseInsensitive) == 0) return PacsEntityType::MPPS;
            
            return PacsEntityType::Archive; // default fallback
        }

        // ---------- All as strings (for UI dropdowns etc.) ----------
        static QVector<QString> allStrings() {
            return {
                "Archive",
                "MPPS"                
            };
        }
    };

} // namespace Etrek::Device

Q_DECLARE_METATYPE(Etrek::Pacs::PacsEntityType)

#endif // CONNECTIONTYPEENUMS_H
