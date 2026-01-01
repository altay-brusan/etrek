#ifndef DICOMPARSERSERVICE_H
#define DICOMPARSERSERVICE_H

#include <QString>
#include <QByteArray>
#include <memory>

#include "ImageViewerTypes.h"

// Forward declarations for DCMTK
class DcmDataset;
class DcmFileFormat;

namespace Etrek::ImageViewer::Service {

/**
 * @class DicomParserService
 * @brief DCMTK-based DICOM file parser for extracting metadata and pixel data.
 *
 * Features:
 * - Parse DICOM files from disk or memory
 * - Extract comprehensive metadata (patient, study, series, image)
 * - Extract pixel data as raw bytes
 * - Auto-detect bit depth, signed/unsigned
 * - Handle various transfer syntaxes
 */
class DicomParserService {
public:
    DicomParserService();
    ~DicomParserService();

    /**
     * @brief Parse metadata from a DICOM file.
     * @param filePath Path to the DICOM file
     * @return DicomMetadata struct with extracted information
     */
    DicomMetadata parseMetadata(const QString& filePath);

    /**
     * @brief Parse metadata from DICOM data in memory.
     * @param data Raw DICOM data
     * @return DicomMetadata struct with extracted information
     */
    DicomMetadata parseMetadata(const QByteArray& data);

    /**
     * @brief Extract raw pixel data from a DICOM file.
     * @param filePath Path to the DICOM file
     * @return Raw pixel data as QByteArray
     */
    QByteArray extractPixelData(const QString& filePath);

    /**
     * @brief Extract raw pixel data from DICOM data in memory.
     * @param data Raw DICOM data
     * @return Raw pixel data as QByteArray
     */
    QByteArray extractPixelData(const QByteArray& data);

    /**
     * @brief Load and parse a complete DICOM file.
     * @param filePath Path to the DICOM file
     * @param metadata Output: extracted metadata
     * @param pixelData Output: raw pixel data
     * @return True if successful
     */
    bool loadDicomFile(const QString& filePath, DicomMetadata& metadata, QByteArray& pixelData);

    /**
     * @brief Load and parse DICOM data from memory.
     * @param data Raw DICOM data
     * @param metadata Output: extracted metadata
     * @param pixelData Output: raw pixel data
     * @return True if successful
     */
    bool loadDicomData(const QByteArray& data, DicomMetadata& metadata, QByteArray& pixelData);

    /**
     * @brief Check if a file is a valid DICOM file.
     * @param filePath Path to check
     * @return True if the file appears to be DICOM
     */
    static bool isDicomFile(const QString& filePath);

    /**
     * @brief Get the last error message.
     */
    QString lastError() const;

private:
    DicomMetadata extractMetadataFromDataset(DcmDataset* dataset);
    QByteArray extractPixelDataFromDataset(DcmDataset* dataset);

    // Helper methods for extracting specific tags
    QString getStringTag(DcmDataset* dataset, unsigned short group, unsigned short element, const QString& defaultValue = QString());
    int getIntTag(DcmDataset* dataset, unsigned short group, unsigned short element, int defaultValue = 0);
    double getDoubleTag(DcmDataset* dataset, unsigned short group, unsigned short element, double defaultValue = 0.0);
    bool getPixelSpacing(DcmDataset* dataset, double& spacingX, double& spacingY);

    QString m_lastError;
};

} // namespace Etrek::ImageViewer::Service

#endif // DICOMPARSERSERVICE_H
