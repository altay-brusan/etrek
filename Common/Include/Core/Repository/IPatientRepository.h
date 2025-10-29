#ifndef IPATIENTREPOSITORY_H
#define IPATIENTREPOSITORY_H

#include "Core/Data/Entity/Patient.h"
#include "Specification/Result.h"
#include "Worklist/Specification/WorklistEnum.h"
#include <QString>

namespace Etrek::Core::Repository {

/**
 * @interface IPatientRepository
 * @brief Interface for patient data access operations.
 */
class IPatientRepository {
public:
    virtual ~IPatientRepository() = default;

    /**
     * @brief Updates the status of a patient.
     * @param patientId The ID of the patient to update.
     * @param status The new status.
     * @param reason Optional reason for the status change.
     * @return Result indicating success or failure.
     */
    virtual Etrek::Specification::Result<bool> updateStatus(
        int patientId,
        ProcedureStepStatus status,
        const QString& reason = QString()) = 0;

    /**
     * @brief Retrieves a patient by ID.
     * @param patientId The ID of the patient.
     * @return Result containing the Patient entity or an error.
     */
    virtual Etrek::Specification::Result<Etrek::Core::Data::Entity::Patient> getPatient(int patientId) const = 0;
};

} // namespace Etrek::Core::Repository

#endif // IPATIENTREPOSITORY_H
