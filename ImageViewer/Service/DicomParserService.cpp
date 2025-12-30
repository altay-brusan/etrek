#include "DicomParserService.h"

#include <QFile>
#include <QFileInfo>

// DCMTK includes
#include <dcmtk/dcmdata/dctk.h>
#include <dcmtk/dcmdata/dcfilefo.h>
#include <dcmtk/dcmdata/dcdeftag.h>
#include <dcmtk/dcmdata/dcpixel.h>
#include <dcmtk/dcmdata/dcpxitem.h>
#include <dcmtk/dcmdata/dcistrmb.h>
#include <dcmtk/dcmimgle/dcmimage.h>

namespace Etrek::ImageViewer::Service {

DicomParserService::DicomParserService() = default;

DicomParserService::~DicomParserService() = default;

DicomMetadata DicomParserService::parseMetadata(const QString& filePath) {
    DicomMetadata metadata;

    DcmFileFormat fileFormat;
    OFCondition status = fileFormat.loadFile(filePath.toStdString().c_str());

    if (status.bad()) {
        m_lastError = QString("Failed to load DICOM file: %1").arg(status.text());
        return metadata;
    }

    DcmDataset* dataset = fileFormat.getDataset();
    if (!dataset) {
        m_lastError = "Failed to get DICOM dataset";
        return metadata;
    }

    return extractMetadataFromDataset(dataset);
}

DicomMetadata DicomParserService::parseMetadata(const QByteArray& data) {
    DicomMetadata metadata;

    DcmInputBufferStream stream;
    stream.setBuffer(data.constData(), data.size());
    stream.setEos();

    DcmFileFormat fileFormat;
    fileFormat.transferInit();
    OFCondition status = fileFormat.read(stream);
    fileFormat.transferEnd();

    if (status.bad()) {
        m_lastError = QString("Failed to parse DICOM data: %1").arg(status.text());
        return metadata;
    }

    DcmDataset* dataset = fileFormat.getDataset();
    if (!dataset) {
        m_lastError = "Failed to get DICOM dataset";
        return metadata;
    }

    return extractMetadataFromDataset(dataset);
}

QByteArray DicomParserService::extractPixelData(const QString& filePath) {
    DcmFileFormat fileFormat;
    OFCondition status = fileFormat.loadFile(filePath.toStdString().c_str());

    if (status.bad()) {
        m_lastError = QString("Failed to load DICOM file: %1").arg(status.text());
        return QByteArray();
    }

    DcmDataset* dataset = fileFormat.getDataset();
    if (!dataset) {
        m_lastError = "Failed to get DICOM dataset";
        return QByteArray();
    }

    return extractPixelDataFromDataset(dataset);
}

QByteArray DicomParserService::extractPixelData(const QByteArray& data) {
    DcmInputBufferStream stream;
    stream.setBuffer(data.constData(), data.size());
    stream.setEos();

    DcmFileFormat fileFormat;
    fileFormat.transferInit();
    OFCondition status = fileFormat.read(stream);
    fileFormat.transferEnd();

    if (status.bad()) {
        m_lastError = QString("Failed to parse DICOM data: %1").arg(status.text());
        return QByteArray();
    }

    DcmDataset* dataset = fileFormat.getDataset();
    if (!dataset) {
        m_lastError = "Failed to get DICOM dataset";
        return QByteArray();
    }

    return extractPixelDataFromDataset(dataset);
}

bool DicomParserService::loadDicomFile(const QString& filePath, DicomMetadata& metadata, QByteArray& pixelData) {
    DcmFileFormat fileFormat;
    OFCondition status = fileFormat.loadFile(filePath.toStdString().c_str());

    if (status.bad()) {
        m_lastError = QString("Failed to load DICOM file: %1").arg(status.text());
        return false;
    }

    DcmDataset* dataset = fileFormat.getDataset();
    if (!dataset) {
        m_lastError = "Failed to get DICOM dataset";
        return false;
    }

    metadata = extractMetadataFromDataset(dataset);
    pixelData = extractPixelDataFromDataset(dataset);

    return !pixelData.isEmpty();
}

bool DicomParserService::loadDicomData(const QByteArray& data, DicomMetadata& metadata, QByteArray& pixelData) {
    DcmInputBufferStream stream;
    stream.setBuffer(data.constData(), data.size());
    stream.setEos();

    DcmFileFormat fileFormat;
    fileFormat.transferInit();
    OFCondition status = fileFormat.read(stream);
    fileFormat.transferEnd();

    if (status.bad()) {
        m_lastError = QString("Failed to parse DICOM data: %1").arg(status.text());
        return false;
    }

    DcmDataset* dataset = fileFormat.getDataset();
    if (!dataset) {
        m_lastError = "Failed to get DICOM dataset";
        return false;
    }

    metadata = extractMetadataFromDataset(dataset);
    pixelData = extractPixelDataFromDataset(dataset);

    return !pixelData.isEmpty();
}

bool DicomParserService::isDicomFile(const QString& filePath) {
    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly)) {
        return false;
    }

    // Check for DICM magic bytes at offset 128
    if (file.size() < 132) {
        // File too small, but might still be DICOM without preamble
        file.seek(0);
        QByteArray start = file.read(8);
        // Check for implicit VR tags (group 0x0008 is common)
        if (start.size() >= 4) {
            unsigned short group = *reinterpret_cast<const unsigned short*>(start.constData());
            if (group == 0x0008 || group == 0x0800) {  // Little or big endian
                return true;
            }
        }
        return false;
    }

    file.seek(128);
    QByteArray magic = file.read(4);
    return magic == "DICM";
}

QString DicomParserService::lastError() const {
    return m_lastError;
}

DicomMetadata DicomParserService::extractMetadataFromDataset(DcmDataset* dataset) {
    DicomMetadata metadata;

    // Patient Module
    metadata.patientName = getStringTag(dataset, DCM_PatientName.getGroup(), DCM_PatientName.getElement());
    metadata.patientId = getStringTag(dataset, DCM_PatientID.getGroup(), DCM_PatientID.getElement());
    metadata.patientBirthDate = getStringTag(dataset, DCM_PatientBirthDate.getGroup(), DCM_PatientBirthDate.getElement());
    metadata.patientSex = getStringTag(dataset, DCM_PatientSex.getGroup(), DCM_PatientSex.getElement());

    // Study Module
    metadata.studyInstanceUid = getStringTag(dataset, DCM_StudyInstanceUID.getGroup(), DCM_StudyInstanceUID.getElement());
    metadata.studyDate = getStringTag(dataset, DCM_StudyDate.getGroup(), DCM_StudyDate.getElement());
    metadata.studyTime = getStringTag(dataset, DCM_StudyTime.getGroup(), DCM_StudyTime.getElement());
    metadata.studyDescription = getStringTag(dataset, DCM_StudyDescription.getGroup(), DCM_StudyDescription.getElement());
    metadata.accessionNumber = getStringTag(dataset, DCM_AccessionNumber.getGroup(), DCM_AccessionNumber.getElement());
    metadata.referringPhysician = getStringTag(dataset, DCM_ReferringPhysicianName.getGroup(), DCM_ReferringPhysicianName.getElement());

    // Series Module
    metadata.seriesInstanceUid = getStringTag(dataset, DCM_SeriesInstanceUID.getGroup(), DCM_SeriesInstanceUID.getElement());
    metadata.modality = getStringTag(dataset, DCM_Modality.getGroup(), DCM_Modality.getElement());
    metadata.seriesDescription = getStringTag(dataset, DCM_SeriesDescription.getGroup(), DCM_SeriesDescription.getElement());
    metadata.bodyPartExamined = getStringTag(dataset, DCM_BodyPartExamined.getGroup(), DCM_BodyPartExamined.getElement());
    metadata.seriesNumber = getIntTag(dataset, DCM_SeriesNumber.getGroup(), DCM_SeriesNumber.getElement());

    // Instance Module
    metadata.sopInstanceUid = getStringTag(dataset, DCM_SOPInstanceUID.getGroup(), DCM_SOPInstanceUID.getElement());
    metadata.sopClassUid = getStringTag(dataset, DCM_SOPClassUID.getGroup(), DCM_SOPClassUID.getElement());
    metadata.instanceNumber = getIntTag(dataset, DCM_InstanceNumber.getGroup(), DCM_InstanceNumber.getElement());
    metadata.contentDate = getStringTag(dataset, DCM_ContentDate.getGroup(), DCM_ContentDate.getElement());
    metadata.contentTime = getStringTag(dataset, DCM_ContentTime.getGroup(), DCM_ContentTime.getElement());
    metadata.imageType = getStringTag(dataset, DCM_ImageType.getGroup(), DCM_ImageType.getElement());

    // Image Pixel Module
    metadata.rows = getIntTag(dataset, DCM_Rows.getGroup(), DCM_Rows.getElement());
    metadata.columns = getIntTag(dataset, DCM_Columns.getGroup(), DCM_Columns.getElement());
    metadata.bitsAllocated = getIntTag(dataset, DCM_BitsAllocated.getGroup(), DCM_BitsAllocated.getElement(), 16);
    metadata.bitsStored = getIntTag(dataset, DCM_BitsStored.getGroup(), DCM_BitsStored.getElement(), 12);
    metadata.highBit = getIntTag(dataset, DCM_HighBit.getGroup(), DCM_HighBit.getElement(), 11);
    metadata.pixelRepresentation = getIntTag(dataset, DCM_PixelRepresentation.getGroup(), DCM_PixelRepresentation.getElement(), 0);
    metadata.samplesPerPixel = getIntTag(dataset, DCM_SamplesPerPixel.getGroup(), DCM_SamplesPerPixel.getElement(), 1);
    metadata.photometricInterpretation = getStringTag(dataset, DCM_PhotometricInterpretation.getGroup(), DCM_PhotometricInterpretation.getElement(), "MONOCHROME2");

    // VOI LUT Module
    metadata.windowCenter = getDoubleTag(dataset, DCM_WindowCenter.getGroup(), DCM_WindowCenter.getElement());
    metadata.windowWidth = getDoubleTag(dataset, DCM_WindowWidth.getGroup(), DCM_WindowWidth.getElement());
    metadata.windowCenterWidthExplanation = getStringTag(dataset, DCM_WindowCenterWidthExplanation.getGroup(), DCM_WindowCenterWidthExplanation.getElement());

    // Modality LUT
    metadata.rescaleIntercept = getDoubleTag(dataset, DCM_RescaleIntercept.getGroup(), DCM_RescaleIntercept.getElement(), 0.0);
    metadata.rescaleSlope = getDoubleTag(dataset, DCM_RescaleSlope.getGroup(), DCM_RescaleSlope.getElement(), 1.0);
    metadata.rescaleType = getStringTag(dataset, DCM_RescaleType.getGroup(), DCM_RescaleType.getElement());

    // Pixel Spacing
    getPixelSpacing(dataset, metadata.pixelSpacingX, metadata.pixelSpacingY);

    // X-Ray specific
    metadata.kvp = getDoubleTag(dataset, DCM_KVP.getGroup(), DCM_KVP.getElement());
    metadata.exposureTime = getDoubleTag(dataset, DCM_ExposureTime.getGroup(), DCM_ExposureTime.getElement());
    metadata.tubeCurrent = getDoubleTag(dataset, DCM_XRayTubeCurrent.getGroup(), DCM_XRayTubeCurrent.getElement());

    return metadata;
}

QByteArray DicomParserService::extractPixelDataFromDataset(DcmDataset* dataset) {
    QByteArray pixelData;

    DcmElement* pixelElement = nullptr;
    OFCondition status = dataset->findAndGetElement(DCM_PixelData, pixelElement);

    if (status.bad() || !pixelElement) {
        m_lastError = "No pixel data found in DICOM dataset";
        return pixelData;
    }

    // Try to get uncompressed pixel data first
    Uint8* pixelDataPtr = nullptr;
    status = pixelElement->getUint8Array(pixelDataPtr);

    if (status.good() && pixelDataPtr) {
        unsigned long length = pixelElement->getLength();
        pixelData = QByteArray(reinterpret_cast<const char*>(pixelDataPtr), static_cast<int>(length));
    } else {
        // Handle compressed pixel data
        Uint16* pixelData16 = nullptr;
        status = pixelElement->getUint16Array(pixelData16);

        if (status.good() && pixelData16) {
            unsigned long length = pixelElement->getLength();
            pixelData = QByteArray(reinterpret_cast<const char*>(pixelData16), static_cast<int>(length));
        } else {
            m_lastError = "Failed to extract pixel data";
        }
    }

    return pixelData;
}

QString DicomParserService::getStringTag(DcmDataset* dataset, unsigned short group, unsigned short element, const QString& defaultValue) {
    OFString value;
    DcmTagKey tag(group, element);
    if (dataset->findAndGetOFString(tag, value).good()) {
        return QString::fromStdString(value.c_str());
    }
    return defaultValue;
}

int DicomParserService::getIntTag(DcmDataset* dataset, unsigned short group, unsigned short element, int defaultValue) {
    Sint32 value = defaultValue;
    DcmTagKey tag(group, element);
    // Try Sint32 first
    if (dataset->findAndGetSint32(tag, value).good()) {
        return static_cast<int>(value);
    }
    // Try Uint16
    Uint16 value16 = 0;
    if (dataset->findAndGetUint16(tag, value16).good()) {
        return static_cast<int>(value16);
    }
    return defaultValue;
}

double DicomParserService::getDoubleTag(DcmDataset* dataset, unsigned short group, unsigned short element, double defaultValue) {
    Float64 value = defaultValue;
    DcmTagKey tag(group, element);
    if (dataset->findAndGetFloat64(tag, value).good()) {
        return static_cast<double>(value);
    }
    // Try as string and convert
    OFString strValue;
    if (dataset->findAndGetOFString(tag, strValue).good()) {
        bool ok = false;
        double parsed = QString::fromStdString(strValue.c_str()).toDouble(&ok);
        if (ok) {
            return parsed;
        }
    }
    return defaultValue;
}

bool DicomParserService::getPixelSpacing(DcmDataset* dataset, double& spacingX, double& spacingY) {
    OFString value;
    if (dataset->findAndGetOFString(DCM_PixelSpacing, value, 0).good()) {
        // Pixel spacing is formatted as "row_spacing\\column_spacing"
        QString spacing = QString::fromStdString(value.c_str());
        QStringList parts = spacing.split('\\');
        if (parts.size() >= 1) {
            spacingY = parts[0].toDouble();  // Row spacing
        }
        if (parts.size() >= 2) {
            spacingX = parts[1].toDouble();  // Column spacing
        }
        return true;
    }

    // Try Imager Pixel Spacing for CR/DR
    if (dataset->findAndGetOFString(DCM_ImagerPixelSpacing, value, 0).good()) {
        QString spacing = QString::fromStdString(value.c_str());
        QStringList parts = spacing.split('\\');
        if (parts.size() >= 1) {
            spacingY = parts[0].toDouble();
        }
        if (parts.size() >= 2) {
            spacingX = parts[1].toDouble();
        }
        return true;
    }

    spacingX = 1.0;
    spacingY = 1.0;
    return false;
}

} // namespace Etrek::ImageViewer::Service
