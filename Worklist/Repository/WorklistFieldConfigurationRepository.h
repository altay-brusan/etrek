#ifndef WORKLISTFIELDCONFIGURATIONREPOSITORY_H
#define WORKLISTFIELDCONFIGURATIONREPOSITORY_H

#include <QVector>
#include <QSqlDatabase>
#include "WorklistFieldConfiguration.h"
#include "DatabaseConnectionSetting.h"
#include "WorklistEnum.h"
#include "TranslationProvider.h"
#include "AppLogger.h"
#include "Result.h"

namespace Etrek::Worklist::Repository
{
    using namespace Etrek::Core::Globalization;
    using namespace Etrek::Core::Log;
    using namespace Etrek::Core::Data::Model;
    using namespace Etrek::Worklist::Data::Entity;
    class WorklistFieldConfigurationRepository
    {
    public:
        explicit WorklistFieldConfigurationRepository(std::shared_ptr<DatabaseConnectionSetting> connectionSetting);

        // Get all field configurations
        Result<QVector<WorklistFieldConfiguration>> getAll() const;

        // Get configuration by field name
        Result<WorklistFieldConfiguration> getByFieldName(WorklistFieldName fieldName) const;

        // Update is_enabled for a field
        Result<bool> updateIsEnabled(WorklistFieldName fieldName, bool isEnabled);

    private:
        QSqlDatabase createConnection(const QString& connectionName) const;

        std::shared_ptr<DatabaseConnectionSetting> m_connectionSetting;
        TranslationProvider* translator;
        std::shared_ptr<AppLogger> logger;
    };

}

#endif // WORKLISTFIELDCONFIGURATIONREPOSITORY_H
