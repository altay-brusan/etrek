#ifndef EXAMINATIONCONTEXT_H
#define EXAMINATIONCONTEXT_H

#include "IExaminationContext.h"
#include "Entity/WorklistEntry.h"
#include <QString>
#include <QDate>
#include <QDateTime>
#include <QMap>
#include <optional>

namespace Etrek::Core::Context {

/**
 * @class ExaminationContext
 * @brief Concrete implementation of examination workflow context.
 *
 * This context is created when a user selects a Modality Worklist item
 * and navigates to the Examination page. It extracts relevant data from
 * the WorklistEntry's DICOM attributes for easy access.
 */
class ExaminationContext : public Etrek::Context::IExaminationContext {
public:
    /**
     * @brief Constructs an ExaminationContext from a WorklistEntry.
     * @param entry The selected worklist entry.
     */
    explicit ExaminationContext(const Etrek::Worklist::Data::Entity::WorklistEntry& entry);

    ~ExaminationContext() override = default;

    // --- IContext Implementation ---
    bool isValid() const override;
    QDateTime timestamp() const override;

    // --- IWorkflowContext Implementation ---
    bool isComplete() const override;
    void markComplete() override;

    // --- IExaminationContext Implementation ---
    std::optional<Etrek::Worklist::Data::Entity::WorklistEntry> worklistEntry() const override;
    int worklistEntryId() const override;

    // Patient Demographics
    QString patientName() const override;
    QString patientId() const override;
    QDate patientBirthDate() const override;
    QString patientGender() const override;

    // Study/Procedure Information
    QString accessionNumber() const override;
    QString studyDescription() const override;
    QString bodyPartExamined() const override;
    QString requestedProcedureDescription() const override;
    QString referringPhysician() const override;

    // --- Additional Accessors ---

    /**
     * @brief Returns any DICOM attribute value by tag name.
     * @param tagName The DICOM tag name (e.g., "PatientName").
     * @return The attribute value, or empty string if not found.
     */
    QString attributeValue(const QString& tagName) const;

    /**
     * @brief Returns all extracted attributes as a map.
     * @return Map of tag names to values.
     */
    QMap<QString, QString> allAttributes() const;

private:
    void extractAttributesFromEntry();
    QString formatPatientNameForDisplay(const QString& dicomName) const;
    QDate parseDicomDate(const QString& dicomDate) const;

    Etrek::Worklist::Data::Entity::WorklistEntry m_entry;
    QMap<QString, QString> m_attributes;
    QDateTime m_timestamp;
    bool m_isComplete;
};

} // namespace Etrek::Core::Context

#endif // EXAMINATIONCONTEXT_H
