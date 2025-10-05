#ifndef TECHNIQUEPARAMETER_H
#define TECHNIQUEPARAMETER_H

#include "AnatomicRegion.h"
#include "BodyPart.h"
#include "ScanProtocolUtil.h"
#include <QDate>
#include <QDateTime>
#include <QMetaType>
#include <QString>
#include <optional>

namespace sp = Etrek::ScanProtocol;
namespace ent = Etrek::ScanProtocol::Data::Entity;

namespace Etrek::ScanProtocol::Data::Entity {

    class TechniqueParameter {
    public:
        int Id = -1;
        ///< Unique identifier (primary key in the database)

        ent::BodyPart BodyPart;
        ///< Anatomical body part associated with this technique

        sp::BodySize Size = BodySize::Medium;
        ///< Body size category (Fat, Medium, Thin, Paediatric)

        sp::TechniqueProfile Profile = TechniqueProfile::AP_PA;
        ///< Projection profile (e.g., AP|PA, LAT, OBL, AXIAL)

        int Kvp = 0;
        ///< Kilovoltage peak (tube voltage) used for exposure

        int Ma = 0;
        ///< Tube current (milliampere-seconds, mAs total)

        int Ms = 0;
        ///< Exposure time in milliseconds

        int FKvp = 0;
        ///< Fixed kVp value (override for body part, if defined)

        double FMa = 0;
        ///< Fixed mAs value (override for body part, if defined)

        int FocalSpotSize;
        ///< Focal spot size (usually in microns or mm)

        double SIDMin = 0.0;
        ///< Minimum Source-to-Image Distance (SID) in cm

        double SIDMax = 0.0;
        ///< Maximum Source-to-Image Distance (SID) in cm

        std::optional<GridType> GridType;
        ///< Grid type (Parallel, Focused, Crossed, Moving, Virtual) or null if none

        QString GridRatio;
        ///< Grid ratio (e.g., "8:1", "10:1")

        QString GridSpeed;
        ///< Grid speed classification (e.g., "High", "Medium")

        sp::ExposureStyle ExposureStyle;
        ///< Exposure style (Mas Mode, Time Mode, AEC Mode, Manual)

        QString AecFields;
        ///< Active AEC (Automatic Exposure Control) detector fields configuration

        int AecDensity;
        ///< AEC density setting (positive/negative shift for exposure)

        TechniqueParameter() = default;

        /**
        * @brief Equality operator for comparing with a TechniqueParameter.
        * @param other The TechniqueParameter to compare against.
        * @return True if the TechniqueParameter belongs to the same TechniqueParameter.
        */
        bool
        operator==(const ent::TechniqueParameter&other) const {
        return Id == other.Id;
        }
};

} // namespace Etrek::ScanProtocol::Data::Entity

Q_DECLARE_METATYPE(Etrek::ScanProtocol::Data::Entity::TechniqueParameter)

#endif // TECHNIQUEPARAMETER_H
