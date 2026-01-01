#include "ImageLoaderService.h"
#include "DicomParserService.h"

#include <QFile>
#include <QFileInfo>

// VTK includes
#include <vtkImageData.h>
#include <vtkPNGReader.h>
#include <vtkJPEGReader.h>
#include <vtkBMPReader.h>
#include <vtkTIFFReader.h>
#include <vtkImageReader2Factory.h>
#include <vtkImageReader2.h>
#include <vtkPointData.h>
#include <vtkDataArray.h>
#include <vtkNew.h>
#include <vtkUnsignedCharArray.h>
#include <vtkUnsignedShortArray.h>
#include <vtkShortArray.h>
#include <vtkSignedCharArray.h>

namespace Etrek::ImageViewer::Service {

ImageLoaderService::ImageLoaderService()
    : m_dicomParser(std::make_unique<DicomParserService>())
{
}

ImageLoaderService::~ImageLoaderService() = default;

FullImageLoadResult ImageLoaderService::loadFromFile(const QString& filePath) {
    FullImageLoadResult result;

    if (!QFile::exists(filePath)) {
        result.errorMessage = QString("File not found: %1").arg(filePath);
        return result;
    }

    // Detect format
    ImageFormat format = detectFormat(filePath);

    // If unknown from extension, try to detect from content
    if (format == ImageFormat::UNKNOWN) {
        QFile file(filePath);
        if (file.open(QIODevice::ReadOnly)) {
            QByteArray header = file.read(132);  // Read enough for DICOM detection
            file.close();
            format = detectFormatFromContent(header);
        }
    }

    // Still unknown? Check if it's DICOM
    if (format == ImageFormat::UNKNOWN) {
        if (DicomParserService::isDicomFile(filePath)) {
            format = ImageFormat::DICOM;
        }
    }

    // Load based on format
    switch (format) {
        case ImageFormat::DICOM:
            return loadDicom(filePath);
        case ImageFormat::PNG:
            return loadPng(filePath);
        case ImageFormat::JPEG:
            return loadJpeg(filePath);
        case ImageFormat::BMP:
            return loadBmp(filePath);
        case ImageFormat::TIFF:
            return loadTiff(filePath);
        case ImageFormat::JPEG2000:
            // Try DICOM loader which may handle JPEG2000 compressed DICOMs
            // or fall through to auto-detection
            return loadDicom(filePath);
        default:
            // Try VTK's auto-detection
            {
                vtkNew<vtkImageReader2Factory> factory;
                vtkSmartPointer<vtkImageReader2> reader;
                reader.TakeReference(factory->CreateImageReader2(filePath.toStdString().c_str()));

                if (reader) {
                    reader->SetFileName(filePath.toStdString().c_str());
                    reader->Update();

                    result.imageData = reader->GetOutput();
                    if (result.imageData && result.imageData->GetNumberOfPoints() > 0) {
                        int dims[3];
                        result.imageData->GetDimensions(dims);
                        result.metadata = createDefaultMetadata(dims[0], dims[1], 8);
                        result.success = true;
                    } else {
                        result.errorMessage = "Failed to load image with auto-detection";
                    }
                } else {
                    result.errorMessage = QString("Unsupported image format: %1").arg(filePath);
                }
            }
            break;
    }

    return result;
}

FullImageLoadResult ImageLoaderService::loadFromBytes(const QByteArray& data, ImageFormat format) {
    FullImageLoadResult result;

    if (data.isEmpty()) {
        result.errorMessage = "Empty image data";
        return result;
    }

    // Auto-detect format if not specified
    if (format == ImageFormat::UNKNOWN) {
        format = detectFormatFromContent(data);
    }

    // Currently only DICOM from bytes is fully supported
    if (format == ImageFormat::DICOM || format == ImageFormat::UNKNOWN) {
        return loadDicomFromBytes(data);
    }

    result.errorMessage = "Loading non-DICOM formats from memory is not yet supported";
    return result;
}

FullImageLoadResult ImageLoaderService::loadDicom(const QString& filePath) {
    FullImageLoadResult result;

    DicomMetadata metadata;
    QByteArray pixelData;

    if (!m_dicomParser->loadDicomFile(filePath, metadata, pixelData)) {
        result.errorMessage = m_dicomParser->lastError();
        return result;
    }

    result.metadata = metadata;

    // Convert pixel data to vtkImageData
    result.imageData = convertToVtkImageData(
        pixelData,
        metadata.columns,
        metadata.rows,
        metadata.bitsAllocated,
        metadata.bitsStored,
        metadata.isSigned(),
        metadata.samplesPerPixel,
        metadata.photometricInterpretation
    );

    if (result.imageData) {
        // Use uniform spacing (1.0, 1.0) for display so pixels appear square
        // The actual pixel spacing is stored in metadata for measurements
        result.imageData->SetSpacing(1.0, 1.0, 1.0);
        result.success = true;
    } else {
        result.errorMessage = "Failed to convert DICOM pixel data to VTK image";
    }

    return result;
}

FullImageLoadResult ImageLoaderService::loadDicomFromBytes(const QByteArray& data) {
    FullImageLoadResult result;

    DicomMetadata metadata;
    QByteArray pixelData;

    if (!m_dicomParser->loadDicomData(data, metadata, pixelData)) {
        result.errorMessage = m_dicomParser->lastError();
        return result;
    }

    result.metadata = metadata;

    result.imageData = convertToVtkImageData(
        pixelData,
        metadata.columns,
        metadata.rows,
        metadata.bitsAllocated,
        metadata.bitsStored,
        metadata.isSigned(),
        metadata.samplesPerPixel,
        metadata.photometricInterpretation
    );

    if (result.imageData) {
        // Use uniform spacing (1.0, 1.0) for display so pixels appear square
        // The actual pixel spacing is stored in metadata for measurements
        result.imageData->SetSpacing(1.0, 1.0, 1.0);
        result.success = true;
    } else {
        result.errorMessage = "Failed to convert DICOM pixel data to VTK image";
    }

    return result;
}

FullImageLoadResult ImageLoaderService::loadPng(const QString& filePath) {
    return loadWithVtkReader(filePath, "vtkPNGReader");
}

FullImageLoadResult ImageLoaderService::loadJpeg(const QString& filePath) {
    return loadWithVtkReader(filePath, "vtkJPEGReader");
}

FullImageLoadResult ImageLoaderService::loadBmp(const QString& filePath) {
    return loadWithVtkReader(filePath, "vtkBMPReader");
}

FullImageLoadResult ImageLoaderService::loadTiff(const QString& filePath) {
    return loadWithVtkReader(filePath, "vtkTIFFReader");
}

FullImageLoadResult ImageLoaderService::loadWithVtkReader(const QString& filePath, const char* readerClassName) {
    FullImageLoadResult result;

    vtkSmartPointer<vtkImageReader2> reader;

    if (strcmp(readerClassName, "vtkPNGReader") == 0) {
        vtkNew<vtkPNGReader> pngReader;
        if (!pngReader->CanReadFile(filePath.toStdString().c_str())) {
            result.errorMessage = "Cannot read PNG file";
            return result;
        }
        pngReader->SetFileName(filePath.toStdString().c_str());
        pngReader->Update();
        result.imageData = pngReader->GetOutput();
    } else if (strcmp(readerClassName, "vtkJPEGReader") == 0) {
        vtkNew<vtkJPEGReader> jpegReader;
        if (!jpegReader->CanReadFile(filePath.toStdString().c_str())) {
            result.errorMessage = "Cannot read JPEG file";
            return result;
        }
        jpegReader->SetFileName(filePath.toStdString().c_str());
        jpegReader->Update();
        result.imageData = jpegReader->GetOutput();
    } else if (strcmp(readerClassName, "vtkBMPReader") == 0) {
        vtkNew<vtkBMPReader> bmpReader;
        if (!bmpReader->CanReadFile(filePath.toStdString().c_str())) {
            result.errorMessage = "Cannot read BMP file";
            return result;
        }
        bmpReader->SetFileName(filePath.toStdString().c_str());
        bmpReader->Update();
        result.imageData = bmpReader->GetOutput();
    } else if (strcmp(readerClassName, "vtkTIFFReader") == 0) {
        vtkNew<vtkTIFFReader> tiffReader;
        if (!tiffReader->CanReadFile(filePath.toStdString().c_str())) {
            result.errorMessage = "Cannot read TIFF file";
            return result;
        }
        tiffReader->SetFileName(filePath.toStdString().c_str());
        tiffReader->Update();
        result.imageData = tiffReader->GetOutput();
    }

    if (result.imageData && result.imageData->GetNumberOfPoints() > 0) {
        int dims[3];
        result.imageData->GetDimensions(dims);

        // Determine bit depth from VTK scalar type
        int bitsAllocated = 8;
        int scalarType = result.imageData->GetScalarType();
        if (scalarType == VTK_UNSIGNED_SHORT || scalarType == VTK_SHORT) {
            bitsAllocated = 16;
        } else if (scalarType == VTK_UNSIGNED_INT || scalarType == VTK_INT ||
                   scalarType == VTK_FLOAT) {
            bitsAllocated = 32;
        }

        result.metadata = createDefaultMetadata(dims[0], dims[1], bitsAllocated);
        result.metadata.samplesPerPixel = result.imageData->GetNumberOfScalarComponents();
        result.success = true;
    } else {
        result.errorMessage = QString("Failed to load image: %1").arg(filePath);
    }

    return result;
}

bool ImageLoaderService::isSupportedFormat(const QString& filePath) {
    ImageFormat format = detectFormat(filePath);
    return format != ImageFormat::UNKNOWN;
}

ImageFormat ImageLoaderService::detectFormatFromContent(const QByteArray& data) {
    if (data.size() < 4) {
        return ImageFormat::UNKNOWN;
    }

    // Check for DICOM (DICM at offset 128)
    if (data.size() >= 132 && data.mid(128, 4) == "DICM") {
        return ImageFormat::DICOM;
    }

    // Check PNG magic bytes
    const unsigned char pngMagic[] = {0x89, 'P', 'N', 'G'};
    if (data.size() >= 4 && memcmp(data.constData(), pngMagic, 4) == 0) {
        return ImageFormat::PNG;
    }

    // Check JPEG magic bytes
    const unsigned char jpegMagic[] = {0xFF, 0xD8, 0xFF};
    if (data.size() >= 3 && memcmp(data.constData(), jpegMagic, 3) == 0) {
        return ImageFormat::JPEG;
    }

    // Check BMP magic bytes
    if (data.size() >= 2 && data[0] == 'B' && data[1] == 'M') {
        return ImageFormat::BMP;
    }

    // Check TIFF magic bytes (little-endian or big-endian)
    if (data.size() >= 4 &&
        ((data[0] == 'I' && data[1] == 'I' && data[2] == 42 && data[3] == 0) ||
         (data[0] == 'M' && data[1] == 'M' && data[2] == 0 && data[3] == 42))) {
        return ImageFormat::TIFF;
    }

    // Check JPEG2000 magic bytes
    const unsigned char jp2Magic[] = {0x00, 0x00, 0x00, 0x0C, 0x6A, 0x50, 0x20, 0x20};
    if (data.size() >= 8 && memcmp(data.constData(), jp2Magic, 8) == 0) {
        return ImageFormat::JPEG2000;
    }

    // Check for DICOM without preamble (starts with group 0x0008)
    if (data.size() >= 4) {
        unsigned short group = *reinterpret_cast<const unsigned short*>(data.constData());
        if (group == 0x0008) {  // Little endian
            return ImageFormat::DICOM;
        }
    }

    return ImageFormat::UNKNOWN;
}

vtkSmartPointer<vtkImageData> ImageLoaderService::convertToVtkImageData(
    const QByteArray& pixelData,
    int width,
    int height,
    int bitsAllocated,
    int bitsStored,
    bool isSigned,
    int samplesPerPixel,
    const QString& photometric)
{
    if (pixelData.isEmpty() || width <= 0 || height <= 0) {
        return nullptr;
    }

    vtkSmartPointer<vtkImageData> imageData = vtkSmartPointer<vtkImageData>::New();
    imageData->SetDimensions(width, height, 1);

    int bytesPerPixel = bitsAllocated / 8;
    int expectedSize = width * height * samplesPerPixel * bytesPerPixel;

    // Verify data size
    if (pixelData.size() < expectedSize) {
        // Data might be compressed or different format - return nullptr
        return nullptr;
    }

    // Allocate based on bit depth
    if (bitsAllocated == 8) {
        if (isSigned) {
            imageData->AllocateScalars(VTK_SIGNED_CHAR, samplesPerPixel);
            auto* scalars = imageData->GetPointData()->GetScalars();
            memcpy(scalars->GetVoidPointer(0), pixelData.constData(), expectedSize);
        } else {
            imageData->AllocateScalars(VTK_UNSIGNED_CHAR, samplesPerPixel);
            auto* scalars = imageData->GetPointData()->GetScalars();
            memcpy(scalars->GetVoidPointer(0), pixelData.constData(), expectedSize);
        }
    } else if (bitsAllocated == 16) {
        if (isSigned) {
            imageData->AllocateScalars(VTK_SHORT, samplesPerPixel);
            auto* scalars = imageData->GetPointData()->GetScalars();
            memcpy(scalars->GetVoidPointer(0), pixelData.constData(), expectedSize);
        } else {
            imageData->AllocateScalars(VTK_UNSIGNED_SHORT, samplesPerPixel);
            auto* scalars = imageData->GetPointData()->GetScalars();
            memcpy(scalars->GetVoidPointer(0), pixelData.constData(), expectedSize);
        }
    } else if (bitsAllocated == 32) {
        imageData->AllocateScalars(VTK_INT, samplesPerPixel);
        auto* scalars = imageData->GetPointData()->GetScalars();
        memcpy(scalars->GetVoidPointer(0), pixelData.constData(), expectedSize);
    } else {
        // Unsupported bit depth - try to handle as 16-bit unsigned
        imageData->AllocateScalars(VTK_UNSIGNED_SHORT, samplesPerPixel);
        auto* scalars = imageData->GetPointData()->GetScalars();
        memcpy(scalars->GetVoidPointer(0), pixelData.constData(),
               std::min(static_cast<int>(pixelData.size()), expectedSize));
    }

    // Handle MONOCHROME1 (inverted) - VTK will handle this through window/level
    // The photometric interpretation is stored in metadata for the renderer to use

    return imageData;
}

DicomMetadata ImageLoaderService::createDefaultMetadata(int width, int height, int bitsAllocated) {
    DicomMetadata metadata;
    metadata.columns = width;
    metadata.rows = height;
    metadata.bitsAllocated = bitsAllocated;
    metadata.bitsStored = bitsAllocated;
    metadata.highBit = bitsAllocated - 1;
    metadata.pixelRepresentation = 0;  // unsigned
    metadata.samplesPerPixel = 1;
    metadata.photometricInterpretation = "MONOCHROME2";
    metadata.pixelSpacingX = 1.0;
    metadata.pixelSpacingY = 1.0;

    // Calculate default window/level
    int maxVal = (1 << bitsAllocated) - 1;
    metadata.windowWidth = maxVal;
    metadata.windowCenter = maxVal / 2.0;

    return metadata;
}

} // namespace Etrek::ImageViewer::Service
