#ifndef IWORKLISTFIELDCONFIGURATIONREPOSITORY_H
#define IWORKLISTFIELDCONFIGURATIONREPOSITORY_H

#include <QVector>
#include <QSqlDatabase>

#include "Worklist/Data/Entity/WorklistFieldConfiguration.h"
#include "Specification/Result.h"

namespace Etrek::Worklist::Repository
{

    namespace spc = Etrek::Specification;
    namespace ent = Etrek::Worklist::Data::Entity;

    class IWorklistFieldConfigurationRepository
    {
    public:

        // Get all field configurations
        virtual spc::Result<QVector<ent::WorklistFieldConfiguration>> getAll() const = 0;

        // Get configuration by field name
        virtual spc::Result<ent::WorklistFieldConfiguration> getByFieldName(ent::WorklistFieldName fieldName) const = 0;

        // Update is_enabled for a field
        virtual spc::Result<bool> updateIsEnabled(ent::WorklistFieldName fieldName, bool isEnabled) = 0;

    };

}

#endif // WORKLISTFIELDCONFIGURATIONREPOSITORY_H
