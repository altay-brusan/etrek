#ifndef PATIENT_H
#define PATIENT_H

#include <QString>
#include <QDate>
#include <QDateTime>
#include "Worklist/Specification/WorklistEnum.h"

namespace Etrek::Core::Data::Entity {

class Patient {
public:
    int Id = -1;
    QString PatientId;
    QString PatientName;
    QDate PatientBirthDate;
    QString PatientSex;  // 'M', 'F', 'O', 'U'
    QString IssuerOfPatientId;
    QString OtherPatientIds;
    QString PatientComments;
    
    // Status tracking fields
    ProcedureStepStatus Status = ProcedureStepStatus::PENDING;
    QString StatusReason;
    QDateTime StatusUpdatedAt;
    
    QDateTime CreateDate;
    QDateTime UpdateDate;

    Patient() = default;
};

} // namespace Etrek::Core::Data::Entity

#endif // PATIENT_H
