#ifndef IWORKLISTFIELDCONFIGURATIONREPOSITORY_H
#define IWORKLISTFIELDCONFIGURATIONREPOSITORY_H

#include <QVector>
#include <QSqlDatabase>

#include "Worklist/Data/Entity/WorklistFieldConfiguration.h"
#include "Specification/Result.h"

namespace Etrek::Worklist::Repository
{

    using Etrek::Specification::Result;

    class IWorklistFieldConfigurationRepository
    {
    public:       

        // Get all field configurations
        virtual  Result<QVector<Etrek::Worklist::Data::Entity::WorklistFieldConfiguration>> getAll() const = 0;

        // Get configuration by field name
        virtual Result<Etrek::Worklist::Data::Entity::WorklistFieldConfiguration> getByFieldName(WorklistFieldName fieldName) const = 0;

        // Update is_enabled for a field
        virtual Result<bool> updateIsEnabled(WorklistFieldName fieldName, bool isEnabled) = 0;

    };

}

#endif // WORKLISTFIELDCONFIGURATIONREPOSITORY_H
