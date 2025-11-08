#ifndef ETREK_DICOM_DATA_ENTITY_SERIES_H
#define ETREK_DICOM_DATA_ENTITY_SERIES_H

#include <QString>
#include <QDateTime>

namespace Etrek::Dicom::Data::Entity {

    class Series {
    public:
        int Id = -1;
        int StudyId = -1;                      // Foreign key to studies table
        QString SeriesInstanceUID;
        int SeriesNumber = 0;                  // (0008,1030)
        QString Modality;                      // (0008,0060)
        QString SeriesDescription;             // (0008,103E)
        QString OperatorName;                  // (0008,1070)
        QString BodyPartExamined;              // (0018,0015)
        QString PatientPosition;               // (0018,5100)
        QString ViewPosition;                  // (0018,5101)
        QString ImageLaterality;               // (0020,0062): R, L, B, U
        int AcquisitionDeviceId = -1;          // Foreign key to general_equipments
        QString PresentationIntentType;        // FOR PRESENTATION, FOR PROCESSING

        Series() = default;

        bool IsValid() const { return Id >= 0; }
    };

} // namespace Etrek::Dicom::Data::Entity

#endif // ETREK_DICOM_DATA_ENTITY_SERIES_H
