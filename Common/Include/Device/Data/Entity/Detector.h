#ifndef DETECTOR_H
#define DETECTOR_H

#include <QString>
#include <QMetaType>
#include <QDate>
#include <QDateTime>
#include "DetectorUtils.h"


namespace Etrek::Device::Data::Entity {

    class Detector {
    public:
        int Id = -1;                          ///< Primary key
        QString Manufacturer;                 ///< Manufacturer instance
        QString ModelName;                    ///< Manufacturer's model name (0008,1090)
        QString Resolution;                   ///< Resolution in terms of pixel size (0054,0220)
        QString Size;                         ///< Physical size (e.g., 14" x 17") (0054,0222)
        double PixelWidth = 0.0;              ///< Pixel width in mm (0054,0224)
        double PixelHeight = 0.0;             ///< Pixel height in mm (0054,0225)
        double Sensitivity = 0.0;             ///< Detector sensitivity (0054,0226)
        QString ImagerPixelSpacing;           ///< "row_spacing;column_spacing" (0018,1164)
        QString Identifier;                    ///< Detector serial/ID (0018,700A)
        QString SpaceLUTReference;            ///< LUT reference (0054,0228)
        double SpatialResolution = 0.0;       ///< Spatial resolution in mm (0018,1050)
        QString ScanOptions;                   ///< Scan parameters (0018,0022)
        int Width = 0;                         ///< Detector width in pixels
        int Height = 0;                        ///< Detector height in pixels
        int WidthShift = 0;                    ///< Optional shifts
        int HeightShift = 0;
        int SaturationValue = 0;
        bool IsActive = false;

        QDateTime CreateDate;
        QDateTime UpdateDate;

        Detector() = default;

        bool operator==(const Detector& other) const {
            return Id == other.Id;
        }
    };

} // namespace Etrek::Device::Data::Entity

Q_DECLARE_METATYPE(Etrek::Device::Data::Entity::Detector)

#endif // DETECTOR_H
