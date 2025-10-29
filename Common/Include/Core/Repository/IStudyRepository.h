#ifndef ISTUDYREPOSITORY_H
#define ISTUDYREPOSITORY_H

#include "Core/Data/Entity/Study.h"
#include "Specification/Result.h"
#include "Worklist/Specification/WorklistEnum.h"
#include <QString>

namespace Etrek::Core::Repository {

/**
 * @interface IStudyRepository
 * @brief Interface for study data access operations with status tracking.
 */
class IStudyRepository {
public:
    virtual ~IStudyRepository() = default;

    /**
     * @brief Updates the status of a study.
     * @param studyId The ID of the study to update.
     * @param status The new status.
     * @param reason Optional reason for the status change.
     * @return Result indicating success or failure.
     */
    virtual Etrek::Specification::Result<bool> updateStatus(
        int studyId,
        ProcedureStepStatus status,
        const QString& reason = QString()) = 0;

    /**
     * @brief Retrieves a study by ID.
     * @param studyId The ID of the study.
     * @return Result containing the Study entity or an error.
     */
    virtual Etrek::Specification::Result<Etrek::Core::Data::Entity::Study> getStudy(int studyId) const = 0;
};

} // namespace Etrek::Core::Repository

#endif // ISTUDYREPOSITORY_H
