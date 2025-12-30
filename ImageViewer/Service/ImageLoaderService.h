#ifndef IMAGELOADERSERVICE_H
#define IMAGELOADERSERVICE_H

#include <QString>
#include <QByteArray>
#include <memory>

#include <vtkSmartPointer.h>
#include <vtkImageData.h>

#include "ImageViewerTypes.h"

namespace Etrek::ImageViewer::Service {

class DicomParserService;

/**
 * @struct ImageLoadResult
 * @brief Result of an image loading operation.
 */
struct FullImageLoadResult {
    bool success = false;
    QString errorMessage;
    DicomMetadata metadata;
    vtkSmartPointer<vtkImageData> imageData;
};

/**
 * @class ImageLoaderService
 * @brief Service for loading medical images from various sources and formats.
 *
 * Supported formats:
 * - DICOM (.dcm, .dicom, or files without extension)
 * - PNG
 * - JPEG
 * - JPEG2000 (via DCMTK if available)
 * - BMP
 * - TIFF
 *
 * Features:
 * - Auto-detect file format
 * - Auto-detect bit depth
 * - Convert to vtkImageData for VTK rendering
 * - Load from file path or memory (QByteArray)
 */
class ImageLoaderService {
public:
    ImageLoaderService();
    ~ImageLoaderService();

    /**
     * @brief Load an image from a file path.
     * @param filePath Path to the image file
     * @return FullImageLoadResult containing success status, metadata, and VTK image data
     */
    FullImageLoadResult loadFromFile(const QString& filePath);

    /**
     * @brief Load an image from raw bytes.
     * @param data Raw image data
     * @param format Hint for the image format (UNKNOWN for auto-detect)
     * @return FullImageLoadResult
     */
    FullImageLoadResult loadFromBytes(const QByteArray& data, ImageFormat format = ImageFormat::UNKNOWN);

    /**
     * @brief Load DICOM image from file.
     */
    FullImageLoadResult loadDicom(const QString& filePath);

    /**
     * @brief Load DICOM image from memory.
     */
    FullImageLoadResult loadDicomFromBytes(const QByteArray& data);

    /**
     * @brief Load PNG image from file.
     */
    FullImageLoadResult loadPng(const QString& filePath);

    /**
     * @brief Load JPEG image from file.
     */
    FullImageLoadResult loadJpeg(const QString& filePath);

    /**
     * @brief Load BMP image from file.
     */
    FullImageLoadResult loadBmp(const QString& filePath);

    /**
     * @brief Load TIFF image from file.
     */
    FullImageLoadResult loadTiff(const QString& filePath);

    /**
     * @brief Check if a file format is supported.
     */
    static bool isSupportedFormat(const QString& filePath);

    /**
     * @brief Detect format from file contents (magic bytes).
     */
    static ImageFormat detectFormatFromContent(const QByteArray& data);

    /**
     * @brief Convert raw pixel data to vtkImageData.
     * @param pixelData Raw pixel bytes
     * @param width Image width
     * @param height Image height
     * @param bitsAllocated Bits per pixel component (8, 16, etc.)
     * @param bitsStored Actual bits used
     * @param isSigned Whether pixel values are signed
     * @param samplesPerPixel Number of components (1 for grayscale, 3 for RGB)
     * @param photometric Photometric interpretation
     * @return vtkImageData or nullptr on failure
     */
    static vtkSmartPointer<vtkImageData> convertToVtkImageData(
        const QByteArray& pixelData,
        int width,
        int height,
        int bitsAllocated,
        int bitsStored,
        bool isSigned,
        int samplesPerPixel = 1,
        const QString& photometric = "MONOCHROME2"
    );

    /**
     * @brief Create default metadata for non-DICOM images.
     */
    static DicomMetadata createDefaultMetadata(int width, int height, int bitsAllocated);

private:
    std::unique_ptr<DicomParserService> m_dicomParser;

    FullImageLoadResult loadWithVtkReader(const QString& filePath, const char* readerClassName);
};

} // namespace Etrek::ImageViewer::Service

#endif // IMAGELOADERSERVICE_H
