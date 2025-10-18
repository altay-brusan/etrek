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
    namespace spc = Etrek::Specification;
    namespace ent = Etrek::Worklist::Data::Entity;
    namespace mdl = Etrek::Core::Data::Model;
    namespace glob = Etrek::Core::Globalization;
    namespace lg = Etrek::Core::Log;

    class WorklistFieldConfigurationRepository
    {
    public:
        explicit WorklistFieldConfigurationRepository(std::shared_ptr<mdl::DatabaseConnectionSetting> connectionSetting);

        // Get all field configurations
        spc::Result<QVector<ent::WorklistFieldConfiguration>> getAll() const;

        // Get configuration by field name
        spc::Result<ent::WorklistFieldConfiguration> getByFieldName(WorklistFieldName fieldName) const;

        // Update is_enabled for a field
        spc::Result<bool> updateIsEnabled(WorklistFieldName fieldName, bool isEnabled);

    private:
        QSqlDatabase createConnection(const QString& connectionName) const;

        std::shared_ptr<mdl::DatabaseConnectionSetting> m_connectionSetting;
        glob::TranslationProvider* translator;
        std::shared_ptr<lg::AppLogger> logger;
    };

}

#endif // WORKLISTFIELDCONFIGURATIONREPOSITORY_H
