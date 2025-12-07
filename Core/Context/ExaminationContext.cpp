#include "ExaminationContext.h"
#include "Entity/WorklistAttribute.h"

namespace Etrek::Core::Context {

ExaminationContext::ExaminationContext(const Etrek::Worklist::Data::Entity::WorklistEntry& entry)
    : m_entry(entry)
    , m_timestamp(QDateTime::currentDateTime())
    , m_isComplete(false)
{
    extractAttributesFromEntry();
}

// --- IContext Implementation ---

bool ExaminationContext::isValid() const
{
    return m_entry.Id >= 0 && !m_attributes.isEmpty();
}

QDateTime ExaminationContext::timestamp() const
{
    return m_timestamp;
}

// --- IWorkflowContext Implementation ---

bool ExaminationContext::isComplete() const
{
    return m_isComplete;
}

void ExaminationContext::markComplete()
{
    m_isComplete = true;
}

// --- IExaminationContext Implementation ---

std::optional<Etrek::Worklist::Data::Entity::WorklistEntry> ExaminationContext::worklistEntry() const
{
    if (m_entry.Id >= 0) {
        return m_entry;
    }
    return std::nullopt;
}

int ExaminationContext::worklistEntryId() const
{
    return m_entry.Id;
}

QString ExaminationContext::patientName() const
{
    return formatPatientNameForDisplay(m_attributes.value("PatientName", ""));
}

QString ExaminationContext::patientId() const
{
    return m_attributes.value("PatientID", "");
}

QDate ExaminationContext::patientBirthDate() const
{
    return parseDicomDate(m_attributes.value("PatientBirthDate", ""));
}

QString ExaminationContext::patientGender() const
{
    return m_attributes.value("PatientSex", "");
}

QString ExaminationContext::accessionNumber() const
{
    return m_attributes.value("AccessionNumber", "");
}

QString ExaminationContext::studyDescription() const
{
    QString desc = m_attributes.value("StudyDescription", "");
    if (desc.isEmpty()) {
        desc = m_attributes.value("StudyID", "");
    }
    return desc;
}

QString ExaminationContext::bodyPartExamined() const
{
    return m_attributes.value("BodyPartExamined", "");
}

QString ExaminationContext::requestedProcedureDescription() const
{
    return m_attributes.value("RequestedProcedureDescription", "");
}

QString ExaminationContext::referringPhysician() const
{
    return formatPatientNameForDisplay(m_attributes.value("ReferringPhysicianName", ""));
}

// --- Additional Accessors ---

QString ExaminationContext::attributeValue(const QString& tagName) const
{
    return m_attributes.value(tagName, "");
}

QMap<QString, QString> ExaminationContext::allAttributes() const
{
    return m_attributes;
}

// --- Private Methods ---

void ExaminationContext::extractAttributesFromEntry()
{
    m_attributes.clear();
    for (const auto& attr : m_entry.Attributes) {
        m_attributes[attr.Tag.Name] = attr.TagValue;
    }
}

QString ExaminationContext::formatPatientNameForDisplay(const QString& dicomName) const
{
    // DICOM PN format: LastName^FirstName^MiddleName^Prefix^Suffix
    // Convert to display format: FirstName MiddleName LastName
    if (dicomName.isEmpty()) {
        return QString();
    }

    QStringList parts = dicomName.split('^');
    QString lastName = parts.size() > 0 ? parts[0].trimmed() : "";
    QString firstName = parts.size() > 1 ? parts[1].trimmed() : "";
    QString middleName = parts.size() > 2 ? parts[2].trimmed() : "";

    QStringList displayParts;
    if (!firstName.isEmpty()) displayParts << firstName;
    if (!middleName.isEmpty()) displayParts << middleName;
    if (!lastName.isEmpty()) displayParts << lastName;

    return displayParts.join(" ");
}

QDate ExaminationContext::parseDicomDate(const QString& dicomDate) const
{
    // DICOM DA format: YYYYMMDD
    if (dicomDate.isEmpty() || dicomDate.length() != 8) {
        return QDate();
    }

    bool ok;
    int year = dicomDate.mid(0, 4).toInt(&ok);
    if (!ok) return QDate();

    int month = dicomDate.mid(4, 2).toInt(&ok);
    if (!ok) return QDate();

    int day = dicomDate.mid(6, 2).toInt(&ok);
    if (!ok) return QDate();

    return QDate(year, month, day);
}

// --- Procedure and View Selection (for LOCAL worklist entries) ---

void ExaminationContext::setProcedureId(int procedureId)
{
    m_procedureId = procedureId;
}

int ExaminationContext::procedureId() const
{
    return m_procedureId;
}

void ExaminationContext::setViewIds(const QVector<int>& viewIds)
{
    m_viewIds = viewIds;
}

QVector<int> ExaminationContext::viewIds() const
{
    return m_viewIds;
}

} // namespace Etrek::Core::Context
