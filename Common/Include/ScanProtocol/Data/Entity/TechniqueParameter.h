#ifndef TECHNIQUEPARAMETER_H
#define TECHNIQUEPARAMETER_H

#include <QString>
#include <QMetaType>
#include <QDate>
#include <QDateTime>
#include <optional>
#include "AnatomicRegion.h"
#include "BodyPart.h"
#include "ScanProtocolUtil.h"

using namespace Etrek::ScanProtocol;
using namespace Etrek::ScanProtocol::Data::Entity;

namespace Etrek::ScanProtocol::Data::Entity {

    class TechniqueParameter {
    public:
        int Id = -1;
        ///< Unique identifier (primary key in the database)

        BodyPart BodyPart;
        ///< Anatomical body part associated with this technique

        BodySize Size = BodySize::Medium;
        ///< Body size category (Fat, Medium, Thin, Paediatric)

        TechniqueProfile Profile = TechniqueProfile::AP_PA;
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

        ExposureStyle ExposureStyle;
        ///< Exposure style (Mas Mode, Time Mode, AEC Mode, Manual)

        QString AecFields;
        ///< Active AEC (Automatic Exposure Control) detector fields configuration

        int AecDensity;
        ///< AEC density setting (positive/negative shift for exposure)

        TechniqueParameter() = default;

        /**
         * @brief Equality operator for comparing with a BodyPart.
         * @param other The BodyPart to compare against.
         * @return True if the TechniqueParameter belongs to the same BodyPart.
         */
        bool operator==(const Etrek::ScanProtocol::Data::Entity::BodyPart& other) const {
            return Id == other.Id;
        }
    };


} // namespace Etrek::ScanProtocol::Data::Entity::TechniqueParameter

Q_DECLARE_METATYPE(Etrek::ScanProtocol::Data::Entity::TechniqueParameter)

#endif // TECHNIQUEPARAMETER_H
