#ifndef WORKLISTENTRY_H
#define WORKLISTENTRY_H
#include <QString>
#include <QDateTime>
#include <QList>

#include "WorklistAttribute.h"
#include "WorklistEnum.h"
#include "WorklistProfile.h"

namespace Etrek::Worklist::Data::Entity {

class WorklistEntry {
public:
    int Id = -1;
    Source Source;
    WorklistProfile Profile;
    ProcedureStepStatus Status;
    QDateTime CreatedAt;
    QDateTime UpdatedAt;

    QList<WorklistAttribute> Attributes;  // List of related DICOM tags

    WorklistEntry() = default;
};
}

#endif // WORKLISTENTRY_H
