#ifndef DETECTORENUMS_H
#define DETECTORENUMS_H

#include <QString>
#include <QVector>
#include <QMetaType>

namespace Etrek::Device {

    enum class DetectorCropMode {
        None,
        Software,
        Hardware
    };

    enum class DetectorSaveRawData {
        No_SAVE,
        BEFORE_CALIBRATION,
        AFTER_CALIBRATION,
        ALL
    };

    enum class DetectorOrder {
        Detector1,
        Detector2
    };


    class DetectorUtils {
    public:

        // ---------- CropMode ----------
        static QString toString(DetectorCropMode mode) {
            switch (mode) {
            case DetectorCropMode::None:     return "None";
            case DetectorCropMode::Software: return "Software";
            case DetectorCropMode::Hardware: return "Hardware";
            }
            return "Unknown";
        }

        static DetectorCropMode parseCropMode(const QString& str) {
            if (str.compare("None", Qt::CaseInsensitive) == 0) return DetectorCropMode::None;
            if (str.compare("Software", Qt::CaseInsensitive) == 0) return DetectorCropMode::Software;
            if (str.compare("Hardware", Qt::CaseInsensitive) == 0) return DetectorCropMode::Hardware;
            return DetectorCropMode::None; // default fallback
        }

        static QVector<QString> allCropModeStrings() {
            return { "None", "Software", "Hardware" };
        }

        // ---------- SaveRawData ----------
        static QString toString(DetectorSaveRawData mode) {
            switch (mode) {
            case DetectorSaveRawData::No_SAVE:             return "No_SAVE";
            case DetectorSaveRawData::BEFORE_CALIBRATION:  return "BEFORE_CALIBRATION";
            case DetectorSaveRawData::AFTER_CALIBRATION:   return "AFTER_CALIBRATION";
            case DetectorSaveRawData::ALL:                 return "ALL";
            }
            return "Unknown";
        }

        static DetectorSaveRawData parseSaveRawData(const QString& str) {
            if (str.compare("No_SAVE", Qt::CaseInsensitive) == 0) return DetectorSaveRawData::No_SAVE;
            if (str.compare("BEFORE_CALIBRATION", Qt::CaseInsensitive) == 0) return DetectorSaveRawData::BEFORE_CALIBRATION;
            if (str.compare("AFTER_CALIBRATION", Qt::CaseInsensitive) == 0) return DetectorSaveRawData::AFTER_CALIBRATION;
            if (str.compare("ALL", Qt::CaseInsensitive) == 0) return DetectorSaveRawData::ALL;
            return DetectorSaveRawData::No_SAVE; // default fallback
        }

        static QVector<QString> allSaveRawDataStrings() {
            return { "No_SAVE", "BEFORE_CALIBRATION", "AFTER_CALIBRATION", "ALL" };
        }

        // ---------- DetectorOrder ----------
        static QString toString(DetectorOrder order) {
            switch (order) {
            case DetectorOrder::Detector1: return "Detector 1";
            case DetectorOrder::Detector2: return "Detector 2";
            }
            return "Unknown";
        }

        static DetectorOrder parseDetectorOrder(const QString& str) {
            if (str.compare("Detector 1", Qt::CaseInsensitive) == 0) return DetectorOrder::Detector1;
            if (str.compare("Detector 2", Qt::CaseInsensitive) == 0) return DetectorOrder::Detector2;
            return DetectorOrder::Detector1; // default fallback
        }

        static QVector<QString> allDetectorOrderStrings() {
            return { "Detector 1", "Detector 2" };
        }
    };

} // namespace Etrek::Device

Q_DECLARE_METATYPE(Etrek::Device::DetectorCropMode)
Q_DECLARE_METATYPE(Etrek::Device::DetectorSaveRawData)
Q_DECLARE_METATYPE(Etrek::Device::DetectorOrder)

#endif // DETECTORENUMS_H
