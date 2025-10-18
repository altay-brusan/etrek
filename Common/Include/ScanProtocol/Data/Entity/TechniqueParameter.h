#ifndef TECHNIQUEPARAMETER_H
#define TECHNIQUEPARAMETER_H

/**
 * @file TechniqueParameter.h
 * @brief Declares the TechniqueParameter entity used for defining radiographic exposure parameters.
 *
 * Represents configurable exposure settings for a given body part and projection,
 * including kVp, mAs, SID, grid, and AEC configuration. It forms the technical basis
 * for scan protocol definition in the system.
 */
#include <QDate>
#include <QDateTime>
#include <QMetaType>
#include <QString>
#include <optional>

#include "AnatomicRegion.h"
#include "BodyPart.h"
#include "ScanProtocolUtil.h"

namespace Etrek::ScanProtocol::Data::Entity {

    /**
     * @class TechniqueParameter
     * @brief Persistable entity defining technical exposure parameters for a scan.
     *
     * Stores parameters used to determine imaging technique for a given body part,
     * size, and projection. Equality compares by @ref Id only.
     */
    class TechniqueParameter {
    public:
        /** @brief Unique identifier (primary key in the database). */
        int Id = -1;

        /** @brief Anatomical body part associated with this technique. */
        Etrek::ScanProtocol::Data::Entity::BodyPart BodyPart;

        /** @brief Body size category (e.g., Fat, Medium, Thin, Paediatric). */
        Etrek::ScanProtocol::BodySize Size = Etrek::ScanProtocol::BodySize::Medium;

        /** @brief Projection profile (e.g., AP/PA, LAT, OBL, AXIAL). */
        Etrek::ScanProtocol::TechniqueProfile Profile = Etrek::ScanProtocol::TechniqueProfile::AP_PA;

        /** @brief Kilovoltage peak (tube voltage) used for exposure. */
        int Kvp = 0;

        /** @brief Tube current (milliampere-seconds, mAs total). */
        int Ma = 0;

        /** @brief Exposure time in milliseconds. */
        int Ms = 0;

        /** @brief Fixed kVp value (override for body part, if defined). */
        int FKvp = 0;

        /** @brief Fixed mAs value (override for body part, if defined). */
        double FMa = 0;

        /** @brief Focal spot size (usually in microns or mm). */
        int FocalSpotSize = 0;

        /** @brief Minimum Source-to-Image Distance (SID) in cm. */
        double SIDMin = 0.0;

        /** @brief Maximum Source-to-Image Distance (SID) in cm. */
        double SIDMax = 0.0;

        /** @brief Grid type (Parallel, Focused, Crossed, Moving, Virtual) or null if none. */
        std::optional<Etrek::ScanProtocol::GridType> GridType;

        /** @brief Grid ratio (e.g., "8:1", "10:1"). */
        QString GridRatio;

        /** @brief Grid speed classification (e.g., "High", "Medium"). */
        QString GridSpeed;

        /** @brief Exposure style (Mas Mode, Time Mode, AEC Mode, Manual). */
        Etrek::ScanProtocol::ExposureStyle ExposureStyle;

        /** @brief Active AEC (Automatic Exposure Control) detector fields configuration. */
        QString AecFields;

        /** @brief AEC density setting (positive/negative shift for exposure). */
        int AecDensity = 0;

        /** @brief Default constructor. */
        TechniqueParameter() = default;

        /**
         * @brief Equality operator comparing by @ref Id only.
         * @param other The TechniqueParameter to compare against.
         * @return True if both objects have the same @ref Id; otherwise false.
         */
        bool operator==(const Etrek::ScanProtocol::Data::Entity::TechniqueParameter& other) const noexcept {
            return Id == other.Id;
        }
    };

} // namespace Etrek::ScanProtocol::Data::Entity

/**
 * @brief Enables TechniqueParameter for use with Qt's meta-object system (e.g., QVariant).
 */
Q_DECLARE_METATYPE(Etrek::ScanProtocol::Data::Entity::TechniqueParameter)

#endif // TECHNIQUEPARAMETER_H
