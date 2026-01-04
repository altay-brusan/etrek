#ifndef ETREK_EXAMINATION_EXAMINATIONMODE_H
#define ETREK_EXAMINATION_EXAMINATIONMODE_H

#include <QString>

namespace Etrek::Examination {

/**
 * @enum ExaminationMode
 * @brief Defines the operational mode for an examination workflow.
 *
 * Different modes affect how acquisitions are handled:
 * - CLINICAL: Normal patient examination, creates DICOM objects
 * - DEMO: Exhibition mode, loads sample images for demonstration
 * - CALIBRATION: Detector/generator calibration, no patient data
 * - TEST: Device verification, internal testing only
 */
enum class ExaminationMode {
    CLINICAL,       // Normal patient exam, creates DICOM objects and persists to PACS
    DEMO,           // Exhibition mode, loads sample images, no persistence
    CALIBRATION,    // Detector/generator calibration, special handling
    TEST            // Device verification, internal testing
};

/**
 * @brief Converts ExaminationMode to a display-friendly string.
 * @param mode The examination mode.
 * @return Human-readable mode name.
 */
inline QString examinationModeToString(ExaminationMode mode) {
    switch (mode) {
        case ExaminationMode::CLINICAL:    return QStringLiteral("Clinical");
        case ExaminationMode::DEMO:        return QStringLiteral("Demo");
        case ExaminationMode::CALIBRATION: return QStringLiteral("Calibration");
        case ExaminationMode::TEST:        return QStringLiteral("Test");
        default:                           return QStringLiteral("Unknown");
    }
}

/**
 * @brief Parses a string to ExaminationMode.
 * @param str The string to parse (case-insensitive).
 * @return The corresponding mode, defaults to CLINICAL if unrecognized.
 */
inline ExaminationMode stringToExaminationMode(const QString& str) {
    QString lower = str.toLower().trimmed();
    if (lower == "demo")        return ExaminationMode::DEMO;
    if (lower == "calibration") return ExaminationMode::CALIBRATION;
    if (lower == "test")        return ExaminationMode::TEST;
    return ExaminationMode::CLINICAL;
}

/**
 * @brief Checks if the mode creates patient DICOM data.
 * @param mode The examination mode.
 * @return true if the mode creates real patient DICOM objects.
 */
inline bool modeCreatesPatientData(ExaminationMode mode) {
    return mode == ExaminationMode::CLINICAL;
}

/**
 * @brief Checks if the mode requires a worklist entry.
 * @param mode The examination mode.
 * @return true if the mode needs a valid worklist entry.
 */
inline bool modeRequiresWorklist(ExaminationMode mode) {
    return mode == ExaminationMode::CLINICAL;
}

} // namespace Etrek::Examination

#endif // ETREK_EXAMINATION_EXAMINATIONMODE_H
