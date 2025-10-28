#ifndef SCANPROTOCOLUTIL_H
#define SCANPROTOCOLUTIL_H

/**
 * @file ScanProtocolUtil.h
 * @brief Enumerations and helper utilities for Scan Protocol domain types.
 *
 * Provides strongly-typed enums for technique, view/orientation, and print
 * configuration, plus conversion helpers to/from user-visible and DB strings.
 */

#include <QString>
#include <QVector>
#include <QMetaType>
#include <optional>

namespace Etrek::ScanProtocol {

    /** @name Technique / Exposure enums
     *  @brief Domain concepts for exposure and technique selection.
     *  @{ */
     /**
      * @enum BodySize
      * @brief Patient/body habitus categories.
      */
    enum class BodySize { Fat, Medium, Thin, Paediatric };

    /**
     * @enum TechniqueProfile
     * @brief Projection categories for technique parameters.
     */
    enum class TechniqueProfile { AP_PA, LAT, OBL, AXIAL };

    /**
     * @enum ExposureStyle
     * @brief Exposure control style used by acquisition.
     */
    enum class ExposureStyle { MasMode, TimeMode, AECMode, Manual };

    /**
     * @enum GridType
     * @brief Anti-scatter grid design.
     */
    enum class GridType { Parallel, Focused, Crossed, Moving, Virtual };

    /**
     * @enum TechniqueParameterRole
     * @brief Role of a technique within a View (e.g., for multi-technique views).
     */
    enum class TechniqueParameterRole { Primary, Low, High };
    /** @} */

    /** @name View / Orientation enums
     *  @brief DICOM-inspired concepts for patient orientation and projection.
     *  @{ */
     /**
      * @enum ProjectionProfile
      * @brief High-level projection profile (views may support DUAL).
      */
    enum class ProjectionProfile { AP_PA, LAT, OBL, AXIAL, DUAL };

    /**
     * @enum PatientPosition
     * @brief Patient position during acquisition (DICOM 0018,5100-like).
     */
    enum class PatientPosition { Supine, Prone, Erect, Lateral };

    /**
     * @enum PatientOrientation
     * @brief DICOM-derived orientation codes for row/column axes.
     *
     * Single- and two-letter codes (e.g., L, R, LA, RP, etc.).
     */
    enum class PatientOrientation {
        L, R, A, P, H, F,
        LA, LP, LH, LF,
        RA, RP, RH, RF,
        AL, AR, AH, AF,
        PL, PR, PH, PF,
        HL, HR, HA, HP,
        FL, FR, FA, FP
    };

    /**
     * @enum ViewPosition
     * @brief DICOM-like view position (0018,5101).
     */
    enum class ViewPosition { AP, PA, LL, RL, RLD, LLD, RLO, LLO };

    /**
     * @enum ImageLaterality
     * @brief Laterality marker for the imaged anatomy.
     */
    enum class ImageLaterality { L, R, B };

    /**
     * @enum LabelMark
     * @brief Label content to overlay (e.g., side/position marks).
     */
    enum class LabelMark { R, L, PA, AP, LAT, OBL, FLAT, UPRT };

    /**
     * @enum LabelPosition
     * @brief Corner placement for labels in the rendered image.
     */
    enum class LabelPosition { LEFT_TOP, RIGHT_TOP, LEFT_BOTTOM, RIGHT_BOTTOM };
    /** @} */

    /** @name Print / Layout enums
     *  @brief Printing orientation and film layout.
     *  @{ */
     /**
      * @enum PrintOrientation
      * @brief Page/film orientation.
      */
    enum class PrintOrientation { Landscape, Portrait };

    /**
     * @enum PrintFormat
     * @brief Film layout (DICOM Basic Grayscale Print-like strings).
     *
     * Encoded as literals such as "STANDARD\\1,1".
     */
    enum class PrintFormat { STANDARD_1_1, STANDARD_1_2, STANDARD_2_1, STANDARD_2_2 };
    /** @} */

    /** @name Patient / Demographics enums
     *  @brief Basic patient demographics related enums.
     *  @{ */
    /**
     * @enum Gender
     * @brief Patient gender enumeration.
     */
    enum class Gender { Male, Female, Other };
    /** @} */

    /**
     * @class ScanProtocolUtil
     * @brief Utility functions for converting enums to/from strings.
     *
     * Provides stable string encodings for UI, persistence, and configuration.
     * - `toString(...)` returns a user-facing or DB-facing literal.
     * - `parse...(QString)` converts a string to the corresponding enum or
     *   `std::optional<std::...>` when invalid.
     */
    class ScanProtocolUtil {
    public:
        // ---------- BodySize ----------
        /** @brief Convert BodySize to display string. */
        static QString toString(BodySize v) {
            switch (v) {
            case BodySize::Fat:        return "Fat";
            case BodySize::Medium:     return "Medium";
            case BodySize::Thin:       return "Thin";
            case BodySize::Paediatric: return "Paediatric";
            }
            return "Medium";
        }
        /** @brief Parse BodySize from string (case-insensitive). */
        static BodySize parseSize(const QString& s) {
            if (s.compare("Fat", Qt::CaseInsensitive) == 0) return BodySize::Fat;
            if (s.compare("Medium", Qt::CaseInsensitive) == 0) return BodySize::Medium;
            if (s.compare("Thin", Qt::CaseInsensitive) == 0) return BodySize::Thin;
            if (s.compare("Paediatric", Qt::CaseInsensitive) == 0) return BodySize::Paediatric;
            return BodySize::Medium;
        }
        /** @brief All BodySize strings in canonical order. */
        static QVector<QString> allSizeStrings() {
            return { "Fat", "Medium", "Thin", "Paediatric" };
        }

        // ---------- TechniqueProfile ----------
        /** @brief Convert TechniqueProfile to display string. */
        static QString toString(TechniqueProfile v) {
            switch (v) {
            case TechniqueProfile::AP_PA: return "AP|PA";
            case TechniqueProfile::LAT:   return "LAT";
            case TechniqueProfile::OBL:   return "OBL";
            case TechniqueProfile::AXIAL: return "AXIAL";
            }
            return "AP|PA";
        }
        /** @brief Parse TechniqueProfile from string (case-insensitive). */
        static TechniqueProfile parseProfile(const QString& s) {
            if (s.compare("AP|PA", Qt::CaseInsensitive) == 0) return TechniqueProfile::AP_PA;
            if (s.compare("LAT", Qt::CaseInsensitive) == 0) return TechniqueProfile::LAT;
            if (s.compare("OBL", Qt::CaseInsensitive) == 0) return TechniqueProfile::OBL;
            if (s.compare("AXIAL", Qt::CaseInsensitive) == 0) return TechniqueProfile::AXIAL;
            return TechniqueProfile::AP_PA;
        }
        /** @brief All TechniqueProfile strings in canonical order. */
        static QVector<QString> allProfileStrings() {
            return { "AP|PA", "LAT", "OBL", "AXIAL" };
        }

        // ---------- ExposureStyle ----------
        /** @brief Convert ExposureStyle to display string. */
        static QString toString(ExposureStyle v) {
            switch (v) {
            case ExposureStyle::MasMode:  return "Mas Mode";
            case ExposureStyle::TimeMode: return "Time Mode";
            case ExposureStyle::AECMode:  return "AEC Mode";
            case ExposureStyle::Manual:   return "Manual";
            }
            return "Manual";
        }
        /** @brief Parse ExposureStyle from string (case-insensitive). */
        static ExposureStyle parseExposureStyle(const QString& s) {
            if (s.compare("Mas Mode", Qt::CaseInsensitive) == 0) return ExposureStyle::MasMode;
            if (s.compare("Time Mode", Qt::CaseInsensitive) == 0) return ExposureStyle::TimeMode;
            if (s.compare("AEC Mode", Qt::CaseInsensitive) == 0) return ExposureStyle::AECMode;
            if (s.compare("Manual", Qt::CaseInsensitive) == 0) return ExposureStyle::Manual;
            return ExposureStyle::Manual;
        }
        /** @brief All ExposureStyle strings in canonical order. */
        static QVector<QString> allExposureStyleStrings() {
            return { "Mas Mode", "Time Mode", "AEC Mode", "Manual" };
        }

        // ---------- GridType ----------
        /** @brief Convert GridType to display string. */
        static QString toString(GridType v) {
            switch (v) {
            case GridType::Parallel: return "Parallel";
            case GridType::Focused:  return "Focused";
            case GridType::Crossed:  return "Crossed";
            case GridType::Moving:   return "Moving";
            case GridType::Virtual:  return "Virtual";
            }
            return "Parallel";
        }
        /** @brief Parse GridType from string (case-insensitive). */
        static GridType parseGridType(const QString& s) {
            if (s.compare("Parallel", Qt::CaseInsensitive) == 0) return GridType::Parallel;
            if (s.compare("Focused", Qt::CaseInsensitive) == 0) return GridType::Focused;
            if (s.compare("Crossed", Qt::CaseInsensitive) == 0) return GridType::Crossed;
            if (s.compare("Moving", Qt::CaseInsensitive) == 0) return GridType::Moving;
            if (s.compare("Virtual", Qt::CaseInsensitive) == 0) return GridType::Virtual;
            return GridType::Parallel;
        }
        /** @brief All GridType strings in canonical order. */
        static QVector<QString> allGridTypeStrings() {
            return { "Parallel", "Focused", "Crossed", "Moving", "Virtual" };
        }

        // ===================== PATIENT UTILS =====================
        static QString toString(Gender g) {
            switch (g) {
            case Gender::Male:   return QStringLiteral("Male");
            case Gender::Female: return QStringLiteral("Female");
            case Gender::Other:  return QStringLiteral("Other");
            }
            return QStringLiteral("Male");
        }
        static std::optional<Gender> parseGender(const QString& s) {
            if (s.compare(QStringLiteral("Male"), Qt::CaseInsensitive) == 0)   return Gender::Male;
            if (s.compare(QStringLiteral("Female"), Qt::CaseInsensitive) == 0) return Gender::Female;
            if (s.compare(QStringLiteral("Other"), Qt::CaseInsensitive) == 0)  return Gender::Other;
            return std::nullopt;
        }
        static QVector<QString> allGenderStrings() {
            return { QStringLiteral("Male"), QStringLiteral("Female"), QStringLiteral("Other") };
        }

        // ---------- TechniqueParameterRole ----------
        /** @brief Convert TechniqueParameterRole to DB string literal. */
        static QString toDbString(TechniqueParameterRole r) {
            switch (r) {
            case TechniqueParameterRole::Primary: return QStringLiteral("PRIMARY");
            case TechniqueParameterRole::Low:     return QStringLiteral("LOW");
            case TechniqueParameterRole::High:    return QStringLiteral("HIGH");
            }
            return QStringLiteral("PRIMARY");
        }
        /** @brief Parse TechniqueParameterRole from DB string literal. */
        static TechniqueParameterRole fromDbString(const QString& s) {
            if (s.compare(QStringLiteral("LOW"), Qt::CaseInsensitive) == 0) return TechniqueParameterRole::Low;
            if (s.compare(QStringLiteral("HIGH"), Qt::CaseInsensitive) == 0) return TechniqueParameterRole::High;
            return TechniqueParameterRole::Primary;
        }
        /// Convenience alias. @see toDbString
        static inline QString roleToDbString(TechniqueParameterRole r) { return toDbString(r); }
        /// Convenience alias. @see fromDbString
        static inline TechniqueParameterRole roleFromDbString(const QString& s) { return fromDbString(s); }

        // ===================== VIEW UTILS =====================

        // ---------- ProjectionProfile (Views) ----------
        /** @brief Convert ProjectionProfile to display string. */
        static QString toString(ProjectionProfile v) {
            switch (v) {
            case ProjectionProfile::AP_PA: return "AP|PA";
            case ProjectionProfile::LAT:   return "LAT";
            case ProjectionProfile::OBL:   return "OBL";
            case ProjectionProfile::AXIAL: return "AXIAL";
            case ProjectionProfile::DUAL:  return "DUAL";
            }
            return "AP|PA";
        }
        /** @brief Parse ProjectionProfile from string (case-insensitive). */
        static ProjectionProfile parseProjectionProfile(const QString& s) {
            if (s.compare("AP|PA", Qt::CaseInsensitive) == 0) return ProjectionProfile::AP_PA;
            if (s.compare("LAT", Qt::CaseInsensitive) == 0) return ProjectionProfile::LAT;
            if (s.compare("OBL", Qt::CaseInsensitive) == 0) return ProjectionProfile::OBL;
            if (s.compare("AXIAL", Qt::CaseInsensitive) == 0) return ProjectionProfile::AXIAL;
            if (s.compare("DUAL", Qt::CaseInsensitive) == 0) return ProjectionProfile::DUAL;
            return ProjectionProfile::AP_PA;
        }

        // ---------- PatientPosition ----------
        /** @brief Convert PatientPosition to display string. */
        static QString toString(PatientPosition v) {
            switch (v) {
            case PatientPosition::Supine:  return "Supine";
            case PatientPosition::Prone:   return "Prone";
            case PatientPosition::Erect:   return "Erect";
            case PatientPosition::Lateral: return "Lateral";
            }
            return "Supine";
        }
        /** @brief Parse PatientPosition from string (case-insensitive) or null if invalid. */
        static std::optional<PatientPosition> parsePatientPosition(const QString& s) {
            if (s.compare("Supine", Qt::CaseInsensitive) == 0) return PatientPosition::Supine;
            if (s.compare("Prone", Qt::CaseInsensitive) == 0) return PatientPosition::Prone;
            if (s.compare("Erect", Qt::CaseInsensitive) == 0) return PatientPosition::Erect;
            if (s.compare("Lateral", Qt::CaseInsensitive) == 0) return PatientPosition::Lateral;
            return std::nullopt;
        }
        /** @brief All PatientPosition strings in canonical order. */
        static QVector<QString> allPatientPositionStrings() {
            return { "Supine","Prone","Erect","Lateral" };
        }

        // ---------- PatientOrientation ----------
        /** @brief Convert PatientOrientation to display string. */
        static QString toString(PatientOrientation v) {
            switch (v) {
            case PatientOrientation::L:  return "L";
            case PatientOrientation::R:  return "R";
            case PatientOrientation::A:  return "A";
            case PatientOrientation::P:  return "P";
            case PatientOrientation::H:  return "H";
            case PatientOrientation::F:  return "F";
            case PatientOrientation::LA: return "LA";
            case PatientOrientation::LP: return "LP";
            case PatientOrientation::LH: return "LH";
            case PatientOrientation::LF: return "LF";
            case PatientOrientation::RA: return "RA";
            case PatientOrientation::RP: return "RP";
            case PatientOrientation::RH: return "RH";
            case PatientOrientation::RF: return "RF";
            case PatientOrientation::AL: return "AL";
            case PatientOrientation::AR: return "AR";
            case PatientOrientation::AH: return "AH";
            case PatientOrientation::AF: return "AF";
            case PatientOrientation::PL: return "PL";
            case PatientOrientation::PR: return "PR";
            case PatientOrientation::PH: return "PH";
            case PatientOrientation::PF: return "PF";
            case PatientOrientation::HL: return "HL";
            case PatientOrientation::HR: return "HR";
            case PatientOrientation::HA: return "HA";
            case PatientOrientation::HP: return "HP";
            case PatientOrientation::FL: return "FL";
            case PatientOrientation::FR: return "FR";
            case PatientOrientation::FA: return "FA";
            case PatientOrientation::FP: return "FP";
            }
            return "L";
        }
        /** @brief Parse PatientOrientation from string or null if invalid. */
        static std::optional<PatientOrientation> parsePatientOrientation(const QString& s) {
            const QString v = s.trimmed().toUpper();
            if (v == "L")  return PatientOrientation::L;
            if (v == "R")  return PatientOrientation::R;
            if (v == "A")  return PatientOrientation::A;
            if (v == "P")  return PatientOrientation::P;
            if (v == "H")  return PatientOrientation::H;
            if (v == "F")  return PatientOrientation::F;
            if (v == "LA") return PatientOrientation::LA;
            if (v == "LP") return PatientOrientation::LP;
            if (v == "LH") return PatientOrientation::LH;
            if (v == "LF") return PatientOrientation::LF;
            if (v == "RA") return PatientOrientation::RA;
            if (v == "RP") return PatientOrientation::RP;
            if (v == "RH") return PatientOrientation::RH;
            if (v == "RF") return PatientOrientation::RF;
            if (v == "AL") return PatientOrientation::AL;
            if (v == "AR") return PatientOrientation::AR;
            if (v == "AH") return PatientOrientation::AH;
            if (v == "AF") return PatientOrientation::AF;
            if (v == "PL") return PatientOrientation::PL;
            if (v == "PR") return PatientOrientation::PR;
            if (v == "PH") return PatientOrientation::PH;
            if (v == "PF") return PatientOrientation::PF;
            if (v == "HL") return PatientOrientation::HL;
            if (v == "HR") return PatientOrientation::HR;
            if (v == "HA") return PatientOrientation::HA;
            if (v == "HP") return PatientOrientation::HP;
            if (v == "FL") return PatientOrientation::FL;
            if (v == "FR") return PatientOrientation::FR;
            if (v == "FA") return PatientOrientation::FA;
            if (v == "FP") return PatientOrientation::FP;
            return std::nullopt;
        }
        /** @brief All PatientOrientation strings in canonical order. */
        static QVector<QString> allPatientOrientationStrings() {
            return { "L","R","A","P","H","F","LA","LP","LH","LF",
                     "RA","RP","RH","RF","AL","AR","AH","AF",
                     "PL","PR","PH","PF","HL","HR","HA","HP",
                     "FL","FR","FA","FP" };
        }

        // ---------- ViewPosition ----------
        /** @brief Convert ViewPosition to display string. */
        static QString toString(ViewPosition v) {
            switch (v) {
            case ViewPosition::AP:  return "AP";
            case ViewPosition::PA:  return "PA";
            case ViewPosition::LL:  return "LL";
            case ViewPosition::RL:  return "RL";
            case ViewPosition::RLD: return "RLD";
            case ViewPosition::LLD: return "LLD";
            case ViewPosition::RLO: return "RLO";
            case ViewPosition::LLO: return "LLO";
            }
            return "AP";
        }
        /** @brief Parse ViewPosition from string or null if invalid. */
        static std::optional<ViewPosition> parseViewPosition(const QString& s) {
            const QString v = s.trimmed().toUpper();
            if (v == "AP")  return ViewPosition::AP;
            if (v == "PA")  return ViewPosition::PA;
            if (v == "LL")  return ViewPosition::LL;
            if (v == "RL")  return ViewPosition::RL;
            if (v == "RLD") return ViewPosition::RLD;
            if (v == "LLD") return ViewPosition::LLD;
            if (v == "RLO") return ViewPosition::RLO;
            if (v == "LLO") return ViewPosition::LLO;
            return std::nullopt;
        }
        /** @brief All ViewPosition strings in canonical order. */
        static QVector<QString> allViewPositionStrings() {
            return { "AP","PA","LL","RL","RLD","LLD","RLO","LLO" };
        }

        // ---------- ImageLaterality ----------
        /** @brief Convert ImageLaterality to display string. */
        static QString toString(ImageLaterality v) {
            switch (v) {
            case ImageLaterality::L: return "L";
            case ImageLaterality::R: return "R";
            case ImageLaterality::B: return "B";
            }
            return "L";
        }
        /** @brief Parse ImageLaterality from string or null if invalid. */
        static std::optional<ImageLaterality> parseImageLaterality(const QString& s) {
            const QString v = s.trimmed().toUpper();
            if (v == "L") return ImageLaterality::L;
            if (v == "R") return ImageLaterality::R;
            if (v == "B") return ImageLaterality::B;
            return std::nullopt;
        }
        /** @brief All ImageLaterality strings in canonical order. */
        static QVector<QString> allImageLateralityStrings() {
            return { "L","R","B" };
        }

        // ---------- LabelMark ----------
        /** @brief Convert LabelMark to display string. */
        static QString toString(LabelMark v) {
            switch (v) {
            case LabelMark::R:   return "R";
            case LabelMark::L:   return "L";
            case LabelMark::PA:  return "PA";
            case LabelMark::AP:  return "AP";
            case LabelMark::LAT: return "LAT";
            case LabelMark::OBL: return "OBL";
            case LabelMark::FLAT:return "FLAT";
            case LabelMark::UPRT:return "UPRT";
            }
            return "R";
        }
        /** @brief Parse LabelMark from string or null if invalid. */
        static std::optional<LabelMark> parseLabelMark(const QString& s) {
            const QString v = s.trimmed().toUpper();
            if (v == "R")    return LabelMark::R;
            if (v == "L")    return LabelMark::L;
            if (v == "PA")   return LabelMark::PA;
            if (v == "AP")   return LabelMark::AP;
            if (v == "LAT")  return LabelMark::LAT;
            if (v == "OBL")  return LabelMark::OBL;
            if (v == "FLAT") return LabelMark::FLAT;
            if (v == "UPRT") return LabelMark::UPRT;
            return std::nullopt;
        }
        /** @brief All LabelMark strings in canonical order. */
        static QVector<QString> allLabelMarkStrings() {
            return { "R","L","PA","AP","LAT","OBL","FLAT","UPRT" };
        }

        // ---------- LabelPosition ----------
        /** @brief Convert LabelPosition to display string. */
        static QString toString(LabelPosition v) {
            switch (v) {
            case LabelPosition::LEFT_TOP:     return "LEFT TOP";
            case LabelPosition::RIGHT_TOP:    return "RIGHT TOP";
            case LabelPosition::LEFT_BOTTOM:  return "LEFT BOTTOM";
            case LabelPosition::RIGHT_BOTTOM: return "RIGHT BOTTOM";
            }
            return "RIGHT TOP";
        }
        /** @brief Parse LabelPosition from string or null if invalid. */
        static std::optional<LabelPosition> parseLabelPosition(const QString& s) {
            const QString v = s.trimmed().toUpper();
            if (v == "LEFT TOP")     return LabelPosition::LEFT_TOP;
            if (v == "RIGHT TOP")    return LabelPosition::RIGHT_TOP;
            if (v == "LEFT BOTTOM")  return LabelPosition::LEFT_BOTTOM;
            if (v == "RIGHT BOTTOM") return LabelPosition::RIGHT_BOTTOM;
            return std::nullopt;
        }
        /** @brief All LabelPosition strings in canonical order. */
        static QVector<QString> allLabelPositionStrings() {
            return { "LEFT TOP","RIGHT TOP","LEFT BOTTOM","RIGHT BOTTOM" };
        }

        // --- PrintOrientation ---
        /** @brief Convert PrintOrientation to display string. */
        static QString toString(PrintOrientation v) {
            switch (v) {
            case PrintOrientation::Landscape: return "Landscape";
            case PrintOrientation::Portrait:  return "Portrait";
            }
            return "Landscape";
        }
        /** @brief Parse PrintOrientation from string or null if invalid. */
        static std::optional<PrintOrientation> parsePrintOrientation(const QString& s) {
            if (s.compare("Landscape", Qt::CaseInsensitive) == 0) return PrintOrientation::Landscape;
            if (s.compare("Portrait", Qt::CaseInsensitive) == 0) return PrintOrientation::Portrait;
            return std::nullopt;
        }

        // --- PrintFormat ---
        /**
         * @brief Convert PrintFormat to the DICOM-like literal used in DB (e.g., "STANDARD\\1,1").
         */
        static QString toString(PrintFormat v) {
            switch (v) {
            case PrintFormat::STANDARD_1_1: return "STANDARD\\1,1";
            case PrintFormat::STANDARD_1_2: return "STANDARD\\1,2";
            case PrintFormat::STANDARD_2_1: return "STANDARD\\2,1";
            case PrintFormat::STANDARD_2_2: return "STANDARD\\2,2";
            }
            return "STANDARD\\1,1";
        }
        /**
         * @brief Parse PrintFormat from a DICOM-like literal or return null if invalid.
         */
        static std::optional<PrintFormat> parsePrintFormat(const QString& s) {
            const auto v = s.trimmed();
            if (v.compare("STANDARD\\1,1", Qt::CaseInsensitive) == 0) return PrintFormat::STANDARD_1_1;
            if (v.compare("STANDARD\\1,2", Qt::CaseInsensitive) == 0) return PrintFormat::STANDARD_1_2;
            if (v.compare("STANDARD\\2,1", Qt::CaseInsensitive) == 0) return PrintFormat::STANDARD_2_1;
            if (v.compare("STANDARD\\2,2", Qt::CaseInsensitive) == 0) return PrintFormat::STANDARD_2_2;
            return std::nullopt;
        }
    };

} // namespace Etrek::ScanProtocol

// ---- Qt metatype declarations (for QVariant, queued connections, etc.) ----
Q_DECLARE_METATYPE(Etrek::ScanProtocol::BodySize)
Q_DECLARE_METATYPE(Etrek::ScanProtocol::TechniqueProfile)
Q_DECLARE_METATYPE(Etrek::ScanProtocol::ExposureStyle)
Q_DECLARE_METATYPE(Etrek::ScanProtocol::GridType)
Q_DECLARE_METATYPE(Etrek::ScanProtocol::TechniqueParameterRole)

Q_DECLARE_METATYPE(Etrek::ScanProtocol::ProjectionProfile)
Q_DECLARE_METATYPE(Etrek::ScanProtocol::PatientPosition)
Q_DECLARE_METATYPE(Etrek::ScanProtocol::PatientOrientation)
Q_DECLARE_METATYPE(Etrek::ScanProtocol::ViewPosition)
Q_DECLARE_METATYPE(Etrek::ScanProtocol::ImageLaterality)
Q_DECLARE_METATYPE(Etrek::ScanProtocol::LabelMark)
Q_DECLARE_METATYPE(Etrek::ScanProtocol::LabelPosition)

Q_DECLARE_METATYPE(Etrek::ScanProtocol::PrintOrientation)
Q_DECLARE_METATYPE(Etrek::ScanProtocol::PrintFormat)

#endif // SCANPROTOCOLUTIL_H
