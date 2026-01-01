#ifndef IMAGEVIEWERTYPES_H
#define IMAGEVIEWERTYPES_H

#include <QString>
#include <QDateTime>
#include <QPointF>
#include <cstdint>

namespace Etrek::ImageViewer {

/**
 * @enum ToolType
 * @brief Available imaging tools.
 */
enum class ToolType {
    POINTER,      ///< Default pointer/select tool
    ZOOM,         ///< Zoom in/out
    PAN,          ///< Pan/scroll image
    WINDOW_LEVEL, ///< Adjust window/level (contrast/brightness)
    RULER,        ///< Distance measurement
    ANGLE,        ///< Angle measurement
    RESET         ///< Reset view to original state
};

/**
 * @enum ImageFormat
 * @brief Supported image file formats.
 */
enum class ImageFormat {
    UNKNOWN,
    DICOM,
    PNG,
    JPEG,
    JPEG2000,
    BMP,
    TIFF
};

/**
 * @enum OverlayCorner
 * @brief Positions for DICOM overlay text.
 */
enum class OverlayCorner {
    TOP_LEFT,
    TOP_RIGHT,
    BOTTOM_LEFT,
    BOTTOM_RIGHT
};

/**
 * @enum InteractionType
 * @brief Types of viewport interactions.
 */
enum class InteractionType {
    MOUSE_PRESS,
    MOUSE_MOVE,
    MOUSE_RELEASE,
    MOUSE_WHEEL,
    MOUSE_DOUBLE_CLICK
};

/**
 * @struct DicomMetadata
 * @brief DICOM image metadata for display and processing.
 */
struct DicomMetadata {
    // Patient Module (PS3.3 C.7.1.1)
    QString patientName;
    QString patientId;
    QString patientBirthDate;
    QString patientSex;

    // Study Module (PS3.3 C.7.2.1)
    QString studyInstanceUid;
    QString studyDate;
    QString studyTime;
    QString studyDescription;
    QString accessionNumber;
    QString referringPhysician;

    // Series Module (PS3.3 C.7.3.1)
    QString seriesInstanceUid;
    QString modality;
    QString seriesDescription;
    QString bodyPartExamined;
    int seriesNumber = 0;

    // Image Module
    QString sopInstanceUid;
    QString sopClassUid;
    int instanceNumber = 0;
    int imageNumber = 0;       // Current image index in series
    int totalImages = 1;       // Total images in series
    int totalSlices = 1;       // Total slices (for multi-slice series)
    QString contentDate;
    QString contentTime;
    QString imageType;

    // Image Pixel Module (PS3.3 C.7.6.3)
    int rows = 0;
    int columns = 0;
    int bitsAllocated = 16;
    int bitsStored = 12;
    int highBit = 11;
    int pixelRepresentation = 0;  // 0 = unsigned, 1 = signed
    int samplesPerPixel = 1;
    QString photometricInterpretation;  // MONOCHROME1, MONOCHROME2, RGB

    // VOI LUT Module (PS3.3 C.11.2)
    double windowCenter = 0.0;
    double windowWidth = 0.0;
    QString windowCenterWidthExplanation;

    // Modality LUT Module
    double rescaleIntercept = 0.0;
    double rescaleSlope = 1.0;
    QString rescaleType;

    // Pixel Spacing
    double pixelSpacingX = 1.0;  // mm
    double pixelSpacingY = 1.0;  // mm

    // Image specific
    double kvp = 0.0;
    double exposureTime = 0.0;
    double tubeCurrent = 0.0;

    /**
     * @brief Check if window/level values are present.
     */
    bool hasWindowLevel() const {
        return windowWidth > 0.0;
    }

    /**
     * @brief Check if this is a signed pixel representation.
     */
    bool isSigned() const {
        return pixelRepresentation == 1;
    }

    /**
     * @brief Get the maximum possible pixel value based on bits stored.
     */
    int maxPixelValue() const {
        return (1 << bitsStored) - 1;
    }
};

/**
 * @struct ViewportState
 * @brief Current state of a viewport (zoom, pan, window/level).
 */
struct ViewportState {
    // Zoom and pan
    double zoomFactor = 1.0;
    double panX = 0.0;
    double panY = 0.0;

    // Window/Level
    double windowWidth = 0.0;
    double windowCenter = 0.0;
    bool isWindowLevelSet = false;

    // Original values for reset
    double originalWindowWidth = 0.0;
    double originalWindowCenter = 0.0;

    // Inversion
    bool isInverted = false;

    // Flip/Rotate
    bool isFlippedHorizontal = false;
    bool isFlippedVertical = false;
    int rotationAngle = 0;  // 0, 90, 180, 270

    /**
     * @brief Reset to default state.
     */
    void reset() {
        zoomFactor = 1.0;
        panX = 0.0;
        panY = 0.0;
        windowWidth = originalWindowWidth;
        windowCenter = originalWindowCenter;
        isInverted = false;
        isFlippedHorizontal = false;
        isFlippedVertical = false;
        rotationAngle = 0;
    }

    /**
     * @brief Initialize window/level from DICOM metadata.
     */
    void initializeFromMetadata(const DicomMetadata& metadata) {
        if (metadata.hasWindowLevel()) {
            windowWidth = metadata.windowWidth;
            windowCenter = metadata.windowCenter;
        } else {
            // Default: full dynamic range
            int maxVal = metadata.maxPixelValue();
            windowWidth = maxVal;
            windowCenter = maxVal / 2.0;
        }
        originalWindowWidth = windowWidth;
        originalWindowCenter = windowCenter;
        isWindowLevelSet = true;
    }
};

/**
 * @struct MeasurementLine
 * @brief Represents a distance measurement between two points.
 */
struct MeasurementLine {
    QPointF startPoint;
    QPointF endPoint;
    double distanceMm = 0.0;  // Distance in millimeters
    double distancePixels = 0.0;  // Distance in pixels
    bool isComplete = false;

    /**
     * @brief Calculate distance using pixel spacing.
     */
    void calculateDistance(double pixelSpacingX, double pixelSpacingY) {
        double dx = (endPoint.x() - startPoint.x()) * pixelSpacingX;
        double dy = (endPoint.y() - startPoint.y()) * pixelSpacingY;
        distanceMm = std::sqrt(dx * dx + dy * dy);

        double dxPx = endPoint.x() - startPoint.x();
        double dyPx = endPoint.y() - startPoint.y();
        distancePixels = std::sqrt(dxPx * dxPx + dyPx * dyPx);
    }
};

/**
 * @struct MeasurementAngle
 * @brief Represents an angle measurement with three points.
 */
struct MeasurementAngle {
    QPointF point1;  // First endpoint
    QPointF vertex;  // Vertex (middle point)
    QPointF point2;  // Second endpoint
    double angleDegrees = 0.0;
    bool isComplete = false;

    /**
     * @brief Calculate angle at vertex.
     */
    void calculateAngle() {
        QPointF v1 = point1 - vertex;
        QPointF v2 = point2 - vertex;

        double dot = v1.x() * v2.x() + v1.y() * v2.y();
        double mag1 = std::sqrt(v1.x() * v1.x() + v1.y() * v1.y());
        double mag2 = std::sqrt(v2.x() * v2.x() + v2.y() * v2.y());

        if (mag1 > 0 && mag2 > 0) {
            double cosAngle = dot / (mag1 * mag2);
            cosAngle = std::clamp(cosAngle, -1.0, 1.0);
            angleDegrees = std::acos(cosAngle) * 180.0 / 3.14159265358979323846;
        }
    }
};

/**
 * @struct ImageLoadResult
 * @brief Result of loading an image file.
 */
struct ImageLoadResult {
    bool success = false;
    QString errorMessage;
    DicomMetadata metadata;
    // Note: vtkImageData* is not included here to avoid VTK dependency in Common
    // The actual image data is managed separately
};

/**
 * @brief Detect image format from file extension.
 */
inline ImageFormat detectFormat(const QString& filePath) {
    QString lower = filePath.toLower();
    if (lower.endsWith(".dcm") || lower.endsWith(".dicom")) {
        return ImageFormat::DICOM;
    } else if (lower.endsWith(".png")) {
        return ImageFormat::PNG;
    } else if (lower.endsWith(".jpg") || lower.endsWith(".jpeg")) {
        return ImageFormat::JPEG;
    } else if (lower.endsWith(".jp2") || lower.endsWith(".j2k") || lower.endsWith(".j2c")) {
        return ImageFormat::JPEG2000;
    } else if (lower.endsWith(".bmp")) {
        return ImageFormat::BMP;
    } else if (lower.endsWith(".tif") || lower.endsWith(".tiff")) {
        return ImageFormat::TIFF;
    }
    // Check for DICOM files without extension (common in medical imaging)
    return ImageFormat::UNKNOWN;
}

} // namespace Etrek::ImageViewer

#endif // IMAGEVIEWERTYPES_H
