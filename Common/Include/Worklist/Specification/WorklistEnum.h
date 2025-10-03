#ifndef WORKLISTENUM_H
#define WORKLISTENUM_H
#include <QString>
#include <stdexcept>

// ProcedureStepStatus enum (already defined)
enum class ProcedureStepStatus {
    SCHEDULED,
    PENDING,
    COMPLETED,
    CANCELLED,
    IN_PROGRESS,
    ABORTED
};

// Source enum definition
enum class Source {
    LOCAL,
    RIS
};

// WorklistFieldName enum definition
enum class WorklistFieldName {
    PatientID,
    AccessionNumber,
    OtherPatientID,
    AdmissionID,
    ScheduledProcedureStepID,
    RequestedProcedureID,
    RequestedProcedureCodeValue,
    StudyInstanceUID,
    Unknown
};

// EnumToString template specialization for ProcedureStepStatus
template<typename T>
struct EnumToString;

// Specialize EnumToString for ProcedureStepStatus
template<>
struct EnumToString<ProcedureStepStatus> {
    static QString toString(ProcedureStepStatus status) {
        switch (status) {
        case ProcedureStepStatus::SCHEDULED: return "SCHEDULED";
        case ProcedureStepStatus::PENDING: return "PENDING";
        case ProcedureStepStatus::COMPLETED: return "COMPLETED";
        case ProcedureStepStatus::CANCELLED: return "CANCELLED";
        case ProcedureStepStatus::IN_PROGRESS: return "IN_PROGRESS";
        case ProcedureStepStatus::ABORTED: return "ABORTED";
        default: throw std::invalid_argument("Unknown status");
        }
    }
};

// EnumToString specialization for Source
template<>
struct EnumToString<Source> {
    static QString toString(Source source) {
        switch (source) {
        case Source::LOCAL: return "LOCAL";
        case Source::RIS: return "RIS";
        default: throw std::invalid_argument("Unknown source status");
        }
    }
};

// EnumToString specialization for WorklistFieldName
template<>
struct EnumToString<WorklistFieldName> {
    static QString toString(WorklistFieldName field) {
        switch (field) {
        case WorklistFieldName::PatientID: return "PatientID";
        case WorklistFieldName::AccessionNumber: return "AccessionNumber";
        case WorklistFieldName::OtherPatientID: return "OtherPatientID";
        case WorklistFieldName::AdmissionID: return "AdmissionID";
        case WorklistFieldName::ScheduledProcedureStepID: return "ScheduledProcedureStepID";
        case WorklistFieldName::RequestedProcedureID: return "RequestedProcedureID";
        case WorklistFieldName::RequestedProcedureCodeValue: return "RequestedProcedureCodeValue";
        case WorklistFieldName::StudyInstanceUID: return "StudyInstanceUID";
        default: return "Unknown";
        }
    }
};

// Convert QString to Source enum
inline Source QStringToSource(const QString& sourceStr) {
    if (sourceStr == "LOCAL") return Source::LOCAL;
    else if (sourceStr == "RIS") return Source::RIS;
    else {
        throw std::invalid_argument("Unknown source string");
    }
}

// Convert Source enum to string
inline QString SourceToString(Source source) {
    return EnumToString<Source>::toString(source);
}

// Convert QString to ProcedureStepStatus enum
inline ProcedureStepStatus QStringToStatus(const QString& statusStr) {
    if (statusStr == "SCHEDULED") return ProcedureStepStatus::SCHEDULED;
    else if (statusStr == "PENDING") return ProcedureStepStatus::PENDING;
    else if (statusStr == "COMPLETED") return ProcedureStepStatus::COMPLETED;
    else if (statusStr == "CANCELLED") return ProcedureStepStatus::CANCELLED;
    else if (statusStr == "IN_PROGRESS") return ProcedureStepStatus::IN_PROGRESS;
    else if (statusStr == "ABORTED") return ProcedureStepStatus::ABORTED;
    else {
        throw std::invalid_argument("Unknown status string");
    }
}

// Convert ProcedureStepStatus enum to string
inline QString ProcedureStepStatusToString(ProcedureStepStatus status) {
    return EnumToString<ProcedureStepStatus>::toString(status);
}

// Convert QString to WorklistFieldName enum
inline WorklistFieldName QStringToWorklistFieldName(const QString& fieldStr) {
    if (fieldStr == "PatientID") return WorklistFieldName::PatientID;
    else if (fieldStr == "AccessionNumber") return WorklistFieldName::AccessionNumber;
    else if (fieldStr == "OtherPatientID") return WorklistFieldName::OtherPatientID;
    else if (fieldStr == "AdmissionID") return WorklistFieldName::AdmissionID;
    else if (fieldStr == "ScheduledProcedureStepID") return WorklistFieldName::ScheduledProcedureStepID;
    else if (fieldStr == "RequestedProcedureID") return WorklistFieldName::RequestedProcedureID;
    else if (fieldStr == "RequestedProcedureCodeValue") return WorklistFieldName::RequestedProcedureCodeValue;
    else if (fieldStr == "StudyInstanceUID") return WorklistFieldName::StudyInstanceUID;
    else return WorklistFieldName::Unknown;
}

// Convert WorklistFieldName enum to string
inline QString WorklistFieldNameToString(WorklistFieldName field) {
    return EnumToString<WorklistFieldName>::toString(field);
}

#endif // WORKLISTENUM_H
