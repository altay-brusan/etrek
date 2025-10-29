#ifndef IMAGE_H
#define IMAGE_H

#include <QString>
#include <QDate>
#include <QTime>
#include <QDateTime>
#include "Worklist/Specification/WorklistEnum.h"

namespace Etrek::Core::Data::Entity {

class Image {
public:
    int Id = -1;
    int StudyId = -1;
    int SeriesId = -1;
    QString InstanceNumber;
    QDate ContentDate;
    QTime ContentTime;
    QString ImageType;
    QString ImageComments;
    QString QualityControlImage;  // 'YES', 'NO', 'BOTH'
    QString SamplesPerPixel;
    QString PhotometricInterpretation;
    int RowCount = -1;
    int ColumnsCount = -1;
    int BitsAllocated = -1;
    int BitsStored = -1;
    int HighBit = -1;
    bool PixelRepresentation = false;  // 0: unsigned, 1: 2's complement
    QString PixelAspectRatio;
    QString ContrastAgent;
    QString ImageLaterality;  // 'R', 'L', 'B', 'U'
    QString PixelIntensityRelationship;
    QString PixelIntensitySign;  // '+1', '-1'
    double RescaleIntercept = 0.0;
    double RescaleSlope = 1.0;
    QString RescaleType;
    QString PresentationLutShape;  // 'IDENTITY', 'INVERSE'
    QString LossyImageCompression;  // '00', '01'
    double LossyImageCompressionRatio = 1.0;
    QString DerivationDescription;
    QString AcquisitionDeviceProcessingDescription;
    QString AcquisitionDeviceProcessingCode;
    QString PatientOrientation;
    QString CalibrationImage;  // 'YES', 'NO'
    QString BurnedInAnnotation;  // 'YES', 'NO'
    QString DetectorBinning;
    QString DetectorMode;
    QString FrameIncrementPointer;
    QString FrameLabelVector;
    int Kvp = -1;
    
    // Status tracking fields
    ProcedureStepStatus Status = ProcedureStepStatus::PENDING;
    QString StatusReason;
    QDateTime StatusUpdatedAt;

    Image() = default;
};

} // namespace Etrek::Core::Data::Entity

#endif // IMAGE_H
