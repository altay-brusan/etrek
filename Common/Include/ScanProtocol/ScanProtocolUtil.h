#ifndef SCANPROTOCOLUTIL_H
#define SCANPROTOCOLUTIL_H

#include <QString>
#include <QVector>
#include <QMetaType>
#include <optional>

namespace Etrek::ScanProtocol {

    // ---------------- Technique / Exposure ----------------
    enum class BodySize { Fat, Medium, Thin, Paediatric };
    enum class TechniqueProfile { AP_PA, LAT, OBL, AXIAL };
    enum class ExposureStyle { MasMode, TimeMode, AECMode, Manual };
    enum class GridType { Parallel, Focused, Crossed, Moving, Virtual };
    enum class TechniqueParameterRole { Primary, Low, High };

    // ---------------- View / Orientation ------------------
    enum class ProjectionProfile { AP_PA, LAT, OBL, AXIAL, DUAL };
    enum class PatientPosition { Supine, Prone, Erect, Lateral };

    // DICOM-derived patient orientation tags (Row/Col share same domain)
    enum class PatientOrientation {
        L, R, A, P, H, F,
        LA, LP, LH, LF,
        RA, RP, RH, RF,
        AL, AR, AH, AF,
        PL, PR, PH, PF,
        HL, HR, HA, HP,
        FL, FR, FA, FP
    };

    enum class ViewPosition { AP, PA, LL, RL, RLD, LLD, RLO, LLO };
    enum class ImageLaterality { L, R, B };
    enum class LabelMark { R, L, PA, AP, LAT, OBL, FLAT, UPRT };
    enum class LabelPosition { LEFT_TOP, RIGHT_TOP, LEFT_BOTTOM, RIGHT_BOTTOM };

    // ---------------- Procedure ------------------
    enum class PrintOrientation { Landscape, Portrait };
    enum class PrintFormat { STANDARD_1_1, STANDARD_1_2, STANDARD_2_1, STANDARD_2_2 };
    
    
    class ScanProtocolUtil {
    public:
        // ---------- BodySize ----------
        static QString toString(BodySize v) {
            switch (v) {
            case BodySize::Fat:        return "Fat";
            case BodySize::Medium:     return "Medium";
            case BodySize::Thin:       return "Thin";
            case BodySize::Paediatric: return "Paediatric";
            }
            return "Medium";
        }
        static BodySize parseSize(const QString& s) {
            if (s.compare("Fat", Qt::CaseInsensitive) == 0)        return BodySize::Fat;
            if (s.compare("Medium", Qt::CaseInsensitive) == 0)     return BodySize::Medium;
            if (s.compare("Thin", Qt::CaseInsensitive) == 0)       return BodySize::Thin;
            if (s.compare("Paediatric", Qt::CaseInsensitive) == 0) return BodySize::Paediatric;
            return BodySize::Medium;
        }
        static QVector<QString> allSizeStrings() {
            return { "Fat", "Medium", "Thin", "Paediatric" };
        }

        // ---------- TechniqueProfile ----------
        static QString toString(TechniqueProfile v) {
            switch (v) {
            case TechniqueProfile::AP_PA: return "AP|PA";
            case TechniqueProfile::LAT:   return "LAT";
            case TechniqueProfile::OBL:   return "OBL";
            case TechniqueProfile::AXIAL: return "AXIAL";
            }
            return "AP|PA";
        }
        static TechniqueProfile parseProfile(const QString& s) {
            if (s.compare("AP|PA", Qt::CaseInsensitive) == 0) return TechniqueProfile::AP_PA;
            if (s.compare("LAT", Qt::CaseInsensitive) == 0) return TechniqueProfile::LAT;
            if (s.compare("OBL", Qt::CaseInsensitive) == 0) return TechniqueProfile::OBL;
            if (s.compare("AXIAL", Qt::CaseInsensitive) == 0) return TechniqueProfile::AXIAL;
            return TechniqueProfile::AP_PA;
        }
        static QVector<QString> allProfileStrings() {
            return { "AP|PA", "LAT", "OBL", "AXIAL" };
        }

        // ---------- ExposureStyle ----------
        static QString toString(ExposureStyle v) {
            switch (v) {
            case ExposureStyle::MasMode:  return "Mas Mode";
            case ExposureStyle::TimeMode: return "Time Mode";
            case ExposureStyle::AECMode:  return "AEC Mode";
            case ExposureStyle::Manual:   return "Manual";
            }
            return "Manual";
        }
        static ExposureStyle parseExposureStyle(const QString& s) {
            if (s.compare("Mas Mode", Qt::CaseInsensitive) == 0)  return ExposureStyle::MasMode;
            if (s.compare("Time Mode", Qt::CaseInsensitive) == 0) return ExposureStyle::TimeMode;
            if (s.compare("AEC Mode", Qt::CaseInsensitive) == 0)  return ExposureStyle::AECMode;
            if (s.compare("Manual", Qt::CaseInsensitive) == 0)    return ExposureStyle::Manual;
            return ExposureStyle::Manual;
        }
        static QVector<QString> allExposureStyleStrings() {
            return { "Mas Mode", "Time Mode", "AEC Mode", "Manual" };
        }

        // ---------- GridType ----------
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
        static GridType parseGridType(const QString& s) {
            if (s.compare("Parallel", Qt::CaseInsensitive) == 0) return GridType::Parallel;
            if (s.compare("Focused", Qt::CaseInsensitive) == 0) return GridType::Focused;
            if (s.compare("Crossed", Qt::CaseInsensitive) == 0) return GridType::Crossed;
            if (s.compare("Moving", Qt::CaseInsensitive) == 0) return GridType::Moving;
            if (s.compare("Virtual", Qt::CaseInsensitive) == 0) return GridType::Virtual;
            return GridType::Parallel;
        }
        static QVector<QString> allGridTypeStrings() {
            return { "Parallel", "Focused", "Crossed", "Moving", "Virtual" };
        }

        // ---------- TechniqueParameterRole ----------
        static QString toDbString(TechniqueParameterRole r) {
            switch (r) {
            case TechniqueParameterRole::Primary: return QStringLiteral("PRIMARY");
            case TechniqueParameterRole::Low:     return QStringLiteral("LOW");
            case TechniqueParameterRole::High:    return QStringLiteral("HIGH");
            }
            return QStringLiteral("PRIMARY");
        }
        static TechniqueParameterRole fromDbString(const QString& s) {
            if (s.compare(QStringLiteral("LOW"), Qt::CaseInsensitive) == 0) return TechniqueParameterRole::Low;
            if (s.compare(QStringLiteral("HIGH"), Qt::CaseInsensitive) == 0) return TechniqueParameterRole::High;
            return TechniqueParameterRole::Primary;
        }
        // Convenience aliases mirroring the removed entity helpers
        static inline QString roleToDbString(TechniqueParameterRole r) { return toDbString(r); }
        static inline TechniqueParameterRole roleFromDbString(const QString& s) { return fromDbString(s); }

        // ===================== VIEW UTILS =====================

        // ---------- ProjectionProfile (Views) ----------
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
        static ProjectionProfile parseProjectionProfile(const QString& s) {
            if (s.compare("AP|PA", Qt::CaseInsensitive) == 0) return ProjectionProfile::AP_PA;
            if (s.compare("LAT", Qt::CaseInsensitive) == 0) return ProjectionProfile::LAT;
            if (s.compare("OBL", Qt::CaseInsensitive) == 0) return ProjectionProfile::OBL;
            if (s.compare("AXIAL", Qt::CaseInsensitive) == 0) return ProjectionProfile::AXIAL;
            if (s.compare("DUAL", Qt::CaseInsensitive) == 0) return ProjectionProfile::DUAL;
            return ProjectionProfile::AP_PA;
        }

        // ---------- PatientPosition ----------
        static QString toString(PatientPosition v) {
            switch (v) {
            case PatientPosition::Supine:  return "Supine";
            case PatientPosition::Prone:   return "Prone";
            case PatientPosition::Erect:   return "Erect";
            case PatientPosition::Lateral: return "Lateral";
            }
            return "Supine";
        }
        static std::optional<PatientPosition> parsePatientPosition(const QString& s) {
            if (s.compare("Supine", Qt::CaseInsensitive) == 0) return PatientPosition::Supine;
            if (s.compare("Prone", Qt::CaseInsensitive) == 0) return PatientPosition::Prone;
            if (s.compare("Erect", Qt::CaseInsensitive) == 0) return PatientPosition::Erect;
            if (s.compare("Lateral", Qt::CaseInsensitive) == 0) return PatientPosition::Lateral;
            return std::nullopt;
        }
        static QVector<QString> allPatientPositionStrings() {
            return { "Supine","Prone","Erect","Lateral" };
        }

        // ---------- PatientOrientation ----------
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
        static QVector<QString> allPatientOrientationStrings() {
            return { "L","R","A","P","H","F","LA","LP","LH","LF",
                     "RA","RP","RH","RF","AL","AR","AH","AF",
                     "PL","PR","PH","PF","HL","HR","HA","HP",
                     "FL","FR","FA","FP" };
        }

        // ---------- ViewPosition ----------
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
        static QVector<QString> allViewPositionStrings() {
            return { "AP","PA","LL","RL","RLD","LLD","RLO","LLO" };
        }

        // ---------- ImageLaterality ----------
        static QString toString(ImageLaterality v) {
            switch (v) {
            case ImageLaterality::L: return "L";
            case ImageLaterality::R: return "R";
            case ImageLaterality::B: return "B";
            }
            return "L";
        }
        static std::optional<ImageLaterality> parseImageLaterality(const QString& s) {
            const QString v = s.trimmed().toUpper();
            if (v == "L") return ImageLaterality::L;
            if (v == "R") return ImageLaterality::R;
            if (v == "B") return ImageLaterality::B;
            return std::nullopt;
        }
        static QVector<QString> allImageLateralityStrings() {
            return { "L","R","B" };
        }

        // ---------- LabelMark ----------
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
        static QVector<QString> allLabelMarkStrings() {
            return { "R","L","PA","AP","LAT","OBL","FLAT","UPRT" };
        }

        // ---------- LabelPosition ----------
        static QString toString(LabelPosition v) {
            switch (v) {
            case LabelPosition::LEFT_TOP:     return "LEFT TOP";
            case LabelPosition::RIGHT_TOP:    return "RIGHT TOP";
            case LabelPosition::LEFT_BOTTOM:  return "LEFT BOTTOM";
            case LabelPosition::RIGHT_BOTTOM: return "RIGHT BOTTOM";
            }
            return "RIGHT TOP";
        }
        static std::optional<LabelPosition> parseLabelPosition(const QString& s) {
            const QString v = s.trimmed().toUpper();
            if (v == "LEFT TOP")     return LabelPosition::LEFT_TOP;
            if (v == "RIGHT TOP")    return LabelPosition::RIGHT_TOP;
            if (v == "LEFT BOTTOM")  return LabelPosition::LEFT_BOTTOM;
            if (v == "RIGHT BOTTOM") return LabelPosition::RIGHT_BOTTOM;
            return std::nullopt;
        }
        static QVector<QString> allLabelPositionStrings() {
            return { "LEFT TOP","RIGHT TOP","LEFT BOTTOM","RIGHT BOTTOM" };
        }
    
        // --- PrintOrientation ---
        static QString toString(PrintOrientation v) {
            switch (v) {
            case PrintOrientation::Landscape: return "Landscape";
            case PrintOrientation::Portrait:  return "Portrait";
            }
            return "Landscape";
        }
        static std::optional<PrintOrientation> parsePrintOrientation(const QString& s) {
            if (s.compare("Landscape", Qt::CaseInsensitive) == 0) return PrintOrientation::Landscape;
            if (s.compare("Portrait", Qt::CaseInsensitive) == 0) return PrintOrientation::Portrait;
            return std::nullopt;
        }

        // --- PrintFormat ---
        // DB uses literal values like "STANDARD\1,1"
        static QString toString(PrintFormat v) {
            switch (v) {
            case PrintFormat::STANDARD_1_1: return "STANDARD\\1,1";
            case PrintFormat::STANDARD_1_2: return "STANDARD\\1,2";
            case PrintFormat::STANDARD_2_1: return "STANDARD\\2,1";
            case PrintFormat::STANDARD_2_2: return "STANDARD\\2,2";
            }
            return "STANDARD\\1,1";
        }
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
