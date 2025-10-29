#ifndef ETREK_CORE_DATA_ENTITY_STUDY_H
#define ETREK_CORE_DATA_ENTITY_STUDY_H

#include <QString>

namespace Etrek::Core::Data::Entity {

    class Study {
    public:
        int Id = -1;
        QString StudyInstanceUID;
        QString StudyId;          // DICOM (0020,0010)
        QString AdmissionId;      // DICOM (0038,0010)
        QString AccessionNumber;  // DICOM (0008,0050)
        QString IssuerOfAccessionNumber;
        QString ReferringPhysicianName;
        QString StudyDate;  // stored as DATE in DB; use string for simplicity in this entity
        QString StudyTime;  // stored as TIME in DB
        QString StudyDescription;
        int PatientAge = 0;
        int PatientSize = 0;
        QString Allergy;
    };

} // namespace Etrek::Core::Data::Entity

#endif // ETREK_CORE_DATA_ENTITY_STUDY_H

