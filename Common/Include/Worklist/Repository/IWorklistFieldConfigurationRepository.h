#ifndef IWORKLISTFIELDCONFIGURATIONREPOSITORY_H
#define IWORKLISTFIELDCONFIGURATIONREPOSITORY_H

#include <QVector>
#include <QSqlDatabase>

#include "Worklist/Data/Entity/WorklistFieldConfiguration.h"
#include "Specification/Result.h"

namespace Etrek::Worklist::Repository
{

    class IWorklistFieldConfigurationRepository
    {
    public:

        // Get all field configurations
        virtual Etrek::Specification::Result<QVector<Etrek::Worklist::Data::Entity::WorklistFieldConfiguration>> getAll() const = 0;

        // Get configuration by field name
        virtual Etrek::Specification::Result<Etrek::Worklist::Data::Entity::WorklistFieldConfiguration> getByFieldName(Etrek::Worklist::Data::Entity::WorklistFieldName fieldName) const = 0;

        // Update is_enabled for a field
        virtual Etrek::Specification::Result<bool> updateIsEnabled(Etrek::Worklist::Data::Entity::WorklistFieldName fieldName, bool isEnabled) = 0;

    };

}

#endif // WORKLISTFIELDCONFIGURATIONREPOSITORY_H
