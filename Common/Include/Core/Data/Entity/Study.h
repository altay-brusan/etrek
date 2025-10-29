#ifndef STUDY_H
#define STUDY_H

#include <QString>
#include <QDate>
#include <QTime>
#include <QDateTime>
#include "Worklist/Specification/WorklistEnum.h"

namespace Etrek::Core::Data::Entity {

class Study {
public:
    int Id = -1;
    int PatientId = -1;
    QString StudyInstanceUid;
    QString StudyId;
    QString AccessionNumber;
    QString IssuerOfAccessionNumber;
    QString ReferringPhysicianName;
    QDate StudyDate;
    QTime StudyTime;
    QString StudyDescription;
    int PatientAge = -1;
    int PatientSize = -1;
    QString Allergy;
    
    // Status tracking fields
    ProcedureStepStatus Status = ProcedureStepStatus::PENDING;
    QString StatusReason;
    QDateTime StatusUpdatedAt;

    Study() = default;
};

} // namespace Etrek::Core::Data::Entity

#endif // STUDY_H
