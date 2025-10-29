#ifndef ISERIESREPOSITORY_H
#define ISERIESREPOSITORY_H

#include "Core/Data/Entity/Series.h"
#include "Specification/Result.h"
#include "Worklist/Specification/WorklistEnum.h"
#include <QString>

namespace Etrek::Core::Repository {

/**
 * @interface ISeriesRepository
 * @brief Interface for series data access operations with status tracking.
 */
class ISeriesRepository {
public:
    virtual ~ISeriesRepository() = default;

    /**
     * @brief Updates the status of a series.
     * @param seriesId The ID of the series to update.
     * @param status The new status.
     * @param reason Optional reason for the status change.
     * @return Result indicating success or failure.
     */
    virtual Etrek::Specification::Result<bool> updateStatus(
        int seriesId,
        ProcedureStepStatus status,
        const QString& reason = QString()) = 0;

    /**
     * @brief Retrieves a series by ID.
     * @param seriesId The ID of the series.
     * @return Result containing the Series entity or an error.
     */
    virtual Etrek::Specification::Result<Etrek::Core::Data::Entity::Series> getSeries(int seriesId) const = 0;
};

} // namespace Etrek::Core::Repository

#endif // ISERIESREPOSITORY_H
