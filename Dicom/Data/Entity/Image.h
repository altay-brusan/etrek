#ifndef ETREK_DICOM_DATA_ENTITY_IMAGE_H
#define ETREK_DICOM_DATA_ENTITY_IMAGE_H

#include <QString>
#include <QDate>
#include <QTime>
#include <QDateTime>

namespace Etrek::Dicom::Data::Entity {

    class Image {
    public:
        int Id = -1;
        int StudyId = -1;                              // Foreign key to studies table
        int SeriesId = -1;                             // Foreign key to series table
        QString InstanceNumber;                        // (0020,0013)
        QDate ContentDate;                             // (0008,0023)
        QTime ContentTime;                             // (0008,0033)
        QString ImageType;                             // (0008,0008)
        QString ImageComments;                         // (0020,4000)
        QString QualityControlImage;                   // YES, NO, BOTH
        QString SamplesPerPixel;                       // (0028,0002)
        QString PhotometricInterpretation;             // (0028,0004)
        int RowCount = 0;                              // (0028,0010)
        int ColumnsCount = 0;                          // (0028,0011)
        int BitsAllocated = 0;                         // (0028,0100)
        int BitsStored = 0;                            // (0028,0101)
        int HighBit = 0;                               // (0028,0102)
        bool PixelRepresentation = false;              // (0028,0103)
        QString PixelAspectRatio;
        QString ContrastAgent;                         // (0018,0010)
        QString ImageLaterality;                       // (0020,0062): R, L, B, U
        QString PixelIntensityRelationship;            // (0028,1040)
        QString PixelIntensitySign;                    // +1, -1
        double RescaleIntercept = 0.0;                 // (0028,1052)
        double RescaleSlope = 0.0;                     // (0028,1053)
        QString RescaleType;                           // (0028,1054)
        QString PresentationLutShape;                  // IDENTITY, INVERSE
        QString LossyImageCompression;                 // 00, 01
        double LossyImageCompressionRatio = 0.0;       // (0028,2112)
        QString DerivationDescription;                 // (0008,2111)
        QString AcquisitionDeviceProcessingDescription; // (0018,1400)
        QString AcquisitionDeviceProcessingCode;       // (0018,1401)
        QString PatientOrientation;                    // (0020,0020)
        QString CalibrationImage;                      // YES, NO
        QString BurnedInAnnotation;                    // YES, NO
        QString DetectorBinning;                       // (0018,701A)
        QString DetectorMode;                          // (0018,7008)
        QString FrameIncrementPointer;                 // (0028,0009)
        QString FrameLabelVector;                      // (0018,2002)
        int Kvp = 0;                                   // (0018,0060)

        Image() = default;

        bool IsValid() const { return Id >= 0; }
    };

} // namespace Etrek::Dicom::Data::Entity

#endif // ETREK_DICOM_DATA_ENTITY_IMAGE_H
