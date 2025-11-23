#ifndef IEXAMINATIONCONTEXT_H
#define IEXAMINATIONCONTEXT_H

#include "IWorkflowContext.h"
#include <QString>
#include <QDate>
#include <optional>

namespace Etrek::Worklist::Data::Entity {
    class WorklistEntry;
}

namespace Etrek::Context {

/**
 * @interface IExaminationContext
 * @brief Interface for examination workflow context.
 *
 * This context is created when a user selects a Modality Worklist item
 * and navigates to the Examination page. It provides all the information
 * needed to initialize the examination workflow.
 *
 * The context includes:
 * - Selected WorklistEntry and its attributes
 * - Patient demographics extracted from DICOM tags
 * - Study/procedure information
 */
class IExaminationContext : public IWorkflowContext {
public:
    ~IExaminationContext() override = default;

    /**
     * @brief Returns the context type identifier.
     * @return "Examination"
     */
    QString contextType() const override { return QStringLiteral("Examination"); }

    /**
     * @brief Returns the workflow identifier.
     * @return "Examination"
     */
    QString workflowId() const override { return QStringLiteral("Examination"); }

    // --- WorklistEntry Access ---

    /**
     * @brief Returns the selected worklist entry.
     * @return Optional containing the WorklistEntry if set; empty otherwise.
     */
    virtual std::optional<Etrek::Worklist::Data::Entity::WorklistEntry> worklistEntry() const = 0;

    /**
     * @brief Returns the worklist entry ID.
     * @return The ID of the selected worklist entry, or -1 if not set.
     */
    virtual int worklistEntryId() const = 0;

    // --- Patient Demographics (extracted from DICOM attributes) ---

    /**
     * @brief Returns the patient's full name.
     * @return Patient name in display format.
     */
    virtual QString patientName() const = 0;

    /**
     * @brief Returns the patient ID.
     * @return The patient's identifier.
     */
    virtual QString patientId() const = 0;

    /**
     * @brief Returns the patient's date of birth.
     * @return The patient's birth date.
     */
    virtual QDate patientBirthDate() const = 0;

    /**
     * @brief Returns the patient's gender.
     * @return Gender string (M/F/O).
     */
    virtual QString patientGender() const = 0;

    // --- Study/Procedure Information ---

    /**
     * @brief Returns the accession number.
     * @return The accession number for this examination.
     */
    virtual QString accessionNumber() const = 0;

    /**
     * @brief Returns the study description.
     * @return Description of the study/examination.
     */
    virtual QString studyDescription() const = 0;

    /**
     * @brief Returns the body part being examined.
     * @return The body part name.
     */
    virtual QString bodyPartExamined() const = 0;

    /**
     * @brief Returns the requested procedure description.
     * @return Description of the requested procedure.
     */
    virtual QString requestedProcedureDescription() const = 0;

    /**
     * @brief Returns the referring physician's name.
     * @return The referring physician's name.
     */
    virtual QString referringPhysician() const = 0;
};

} // namespace Etrek::Context

#endif // IEXAMINATIONCONTEXT_H
