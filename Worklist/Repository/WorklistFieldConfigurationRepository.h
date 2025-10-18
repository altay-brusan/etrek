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

    class WorklistFieldConfigurationRepository
    {
    public:
        explicit WorklistFieldConfigurationRepository(std::shared_ptr<Etrek::Core::Data::Model::DatabaseConnectionSetting> connectionSetting);

        // Get all field configurations
        Etrek::Specification::Result<QVector<Etrek::Worklist::Data::Entity::WorklistFieldConfiguration>> getAll() const;

        // Get configuration by field name
        Etrek::Specification::Result<Etrek::Worklist::Data::Entity::WorklistFieldConfiguration> getByFieldName(WorklistFieldName fieldName) const;

        // Update is_enabled for a field
        Etrek::Specification::Result<bool> updateIsEnabled(WorklistFieldName fieldName, bool isEnabled);

    private:
        QSqlDatabase createConnection(const QString& connectionName) const;

        std::shared_ptr<Etrek::Core::Data::Model::DatabaseConnectionSetting> m_connectionSetting;
        Etrek::Core::Globalization::TranslationProvider* translator;
        std::shared_ptr<Etrek::Core::Log::AppLogger> logger;
    };

}

#endif // WORKLISTFIELDCONFIGURATIONREPOSITORY_H
