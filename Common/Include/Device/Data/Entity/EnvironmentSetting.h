#ifndef ENVIRONMENTSETTINGS_H
#define ENVIRONMENTSETTINGS_H

#include <QDateTime>
#include <QMetaType>

// use the enums + helpers
#include "EnvironmentSettingUtils.h"

namespace Etrek::Device::Data::Entity {

    class EnvironmentSetting {
    public:
        int Id = -1;  // id INT AUTO_INCREMENT PRIMARY KEY

        // ENUM columns (use types from EnvironmentSettingsUtils)
        Etrek::Device::StudyLevel  StudyLevelValue = Etrek::Device::StudyLevel::MultiSeriesStudy;
        Etrek::Device::LookupTable LookupTableValue = Etrek::Device::LookupTable::None;

        // Periods
        int WorklistClearPeriodDays = 30;  // worklist_clear_period_days
        int WorklistRefreshPeriodSeconds = 60;  // worklist_refresh_period_seconds
        int DeleteLogPeriodDays = 90;  // delete_log_period_days

        // Flags
        bool AutoRefreshWorklist = true;          // auto_refresh_worklist
        bool AutoClearDiskSpace = true;          // auto_clear_disk_space
        bool EnableMPPS = true;          // enable_mpps
        bool ContinueOnEchoFail = true;          // continue_on_echo_fail

        // Timestamps
        QDateTime CreateDate;                     // create_date
        QDateTime UpdateDate;                     // update_date

        EnvironmentSetting() = default;

        bool operator==(const EnvironmentSetting& other) const { return Id == other.Id; }
    };

} // namespace Etrek::Device::Data::Entity

Q_DECLARE_METATYPE(Etrek::Device::Data::Entity::EnvironmentSetting)

#endif // ENVIRONMENTSETTING_H
