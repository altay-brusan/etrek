#ifndef SERIES_H
#define SERIES_H

#include <QString>
#include <QDateTime>
#include "Worklist/Specification/WorklistEnum.h"

namespace Etrek::Core::Data::Entity {

class Series {
public:
    int Id = -1;
    int StudyId = -1;
    QString SeriesInstanceUid;
    int SeriesNumber = -1;
    QString Modality;
    QString SeriesDescription;
    QString OperatorName;
    QString BodyPartExamined;
    QString PatientPosition;
    QString ViewPosition;
    QString ImageLaterality;  // 'R', 'L', 'B', 'U'
    int AcquisitionDeviceId = -1;
    QString PresentationIntentType;  // 'FOR PRESENTATION', 'FOR PROCESSING'
    
    // Status tracking fields
    ProcedureStepStatus Status = ProcedureStepStatus::PENDING;
    QString StatusReason;
    QDateTime StatusUpdatedAt;

    Series() = default;
};

} // namespace Etrek::Core::Data::Entity

#endif // SERIES_H
