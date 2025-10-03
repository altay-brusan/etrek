#ifndef ENVIRONMENTSETTINGUTILS_H
#define ENVIRONMENTSETTINGUTILS_H

#include <QString>
#include <QVector>
#include <QMetaType>

namespace Etrek::Device {

    // --- Enums match DB ENUM text exactly (see helpers below) ---
    enum class StudyLevel {
        MultiSeriesStudy,   // DB: "Multi-Series Study"
        SingleSeriesStudy   // DB: "Single-Series Study"
    };

    enum class LookupTable {
        VOILUT,             // DB: "VOI LUT"
        None                // DB: "None"
    };

    class EnvironmentSettingUtils {
    public:
        // -------- StudyLevel --------
        static QString toString(StudyLevel v) {
            switch (v) {
            case StudyLevel::MultiSeriesStudy:  return "Multi-Series Study";
            case StudyLevel::SingleSeriesStudy: return "Single-Series Study";
            }
            return "Multi-Series Study";
        }

        static StudyLevel parseStudyLevel(const QString& s) {
            if (s.compare("Multi-Series Study", Qt::CaseInsensitive) == 0) return StudyLevel::MultiSeriesStudy;
            if (s.compare("Single-Series Study", Qt::CaseInsensitive) == 0) return StudyLevel::SingleSeriesStudy;
            return StudyLevel::MultiSeriesStudy; // fallback to DB default
        }

        static QVector<QString> allStudyLevelStrings() {
            return { "Multi-Series Study", "Single-Series Study" };
        }

        // -------- LookupTable --------
        static QString toString(LookupTable v) {
            switch (v) {
            case LookupTable::VOILUT: return "VOI LUT";
            case LookupTable::None:   return "None";
            }
            return "None";
        }

        static LookupTable parseLookupTable(const QString& s) {
            if (s.compare("VOI LUT", Qt::CaseInsensitive) == 0) return LookupTable::VOILUT;
            if (s.compare("None", Qt::CaseInsensitive) == 0) return LookupTable::None;
            return LookupTable::None; // fallback to DB default
        }

        static QVector<QString> allLookupTableStrings() {
            return { "VOI LUT", "None" };
        }
    };

} // namespace Etrek::Device

Q_DECLARE_METATYPE(Etrek::Device::StudyLevel)
Q_DECLARE_METATYPE(Etrek::Device::LookupTable)

#endif // ENVIRONMENTSETTINGUTILS_H
