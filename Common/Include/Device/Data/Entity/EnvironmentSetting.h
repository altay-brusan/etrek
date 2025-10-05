#ifndef ENVIRONMENTSETTINGS_H
#define ENVIRONMENTSETTINGS_H

/**
 * @file EnvironmentSettings.h
 * @brief Entity for environment-level settings in the Device module.
 *
 * @details
 * Represents persisted configuration that controls worklist behavior,
 * housekeeping periods, protocol features, and integration flags.
 * Enumerations and helper functions are defined in @ref EnvironmentSettingUtils.h.
 */

#include <QDateTime>
#include <QMetaType>

#include "EnvironmentSettingUtils.h"  ///< Enums & helpers (StudyLevel, LookupTable, etc.)

namespace Etrek::Device::Data::Entity {

    /// \brief Local alias to access device enums/types without full qualification.
    /// \note Header-safe (scoped inside this namespace).
    namespace dev = Etrek::Device;

    /**
     * @class EnvironmentSetting
     * @brief Persistent configuration entity for runtime environment behavior.
     *
     * @note Default-constructed values reflect application defaults.
     */
    class EnvironmentSetting {
    public:
        int Id = -1;  ///< Database primary key (`id`). \note `-1` indicates not persisted.

        // -------------------- Enum-backed settings --------------------

        dev::StudyLevel  StudyLevelValue = dev::StudyLevel::MultiSeriesStudy;  ///< Study granularity policy.
        dev::LookupTable LookupTableValue = dev::LookupTable::None;             ///< Display LUT selection.

        // -------------------- Periods (durations) --------------------

        int WorklistClearPeriodDays = 30;      ///< Days before worklist items are auto-cleared.
        int WorklistRefreshPeriodSeconds = 60; ///< Seconds between automatic worklist refresh cycles.
        int DeleteLogPeriodDays = 90;          ///< Days to retain logs before deletion.

        // -------------------- Feature flags --------------------

        bool AutoRefreshWorklist = true; ///< Enable periodic refresh of the worklist.
        bool AutoClearDiskSpace = true;  ///< Enable automated disk space cleanup.
        bool EnableMPPS = true;          ///< Enable MPPS (Modality Performed Procedure Step).
        bool ContinueOnEchoFail = true;  ///< Proceed even if DICOM ECHO fails.

        // -------------------- Timestamps --------------------

        QDateTime CreateDate;  ///< Creation timestamp.
        QDateTime UpdateDate;  ///< Last update timestamp.

        /// @brief Default constructor.
        EnvironmentSetting() = default;

        /**
         * @brief Equality by primary key.
         * @param other The other environment setting.
         * @return `true` if @c Id matches; otherwise `false`.
         */
        bool operator==(const EnvironmentSetting& other) const { return Id == other.Id; }
    };

} // namespace Etrek::Device::Data::Entity

/// @brief Qt metatype declaration for QVariant/queued usage.
Q_DECLARE_METATYPE(Etrek::Device::Data::Entity::EnvironmentSetting)

#endif // ENVIRONMENTSETTINGS_H
