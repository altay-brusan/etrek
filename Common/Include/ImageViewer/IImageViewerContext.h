#ifndef IIMAGEVIEWERCONTEXT_H
#define IIMAGEVIEWERCONTEXT_H

#include <QString>
#include <QVector>
#include <QByteArray>
#include <memory>
#include "ImageViewerTypes.h"

namespace Etrek::ImageViewer {

/**
 * @enum ImageSourceType
 * @brief Source type for image viewer.
 */
enum class ImageSourceType {
    NONE,
    FILE,         ///< Load from file path
    STUDY,        ///< Load from database study ID
    SERIES,       ///< Load from database series ID
    RAW_DATA      ///< Direct image data (from ExamPage)
};

/**
 * @class IImageViewerContext
 * @brief Interface for passing image data to the Image Viewer page.
 *
 * This context is used for inter-page communication, allowing
 * ExamPage or other pages to open the ImageViewer with specific data.
 */
class IImageViewerContext {
public:
    virtual ~IImageViewerContext() = default;

    /**
     * @brief Get the source type.
     */
    virtual ImageSourceType sourceType() const = 0;

    /**
     * @brief Set file path for FILE source type.
     */
    virtual void setFilePath(const QString& filePath) = 0;

    /**
     * @brief Get file path.
     */
    virtual QString filePath() const = 0;

    /**
     * @brief Set study ID for STUDY source type.
     */
    virtual void setStudyId(int studyId) = 0;

    /**
     * @brief Get study ID.
     */
    virtual int studyId() const = 0;

    /**
     * @brief Set series IDs for SERIES source type.
     */
    virtual void setSeriesIds(const QVector<int>& seriesIds) = 0;

    /**
     * @brief Get series IDs.
     */
    virtual QVector<int> seriesIds() const = 0;

    /**
     * @brief Set raw image data for RAW_DATA source type.
     * Used when ExamPage passes acquired image directly.
     */
    virtual void setRawImageData(const QByteArray& data, const DicomMetadata& metadata) = 0;

    /**
     * @brief Get raw image data.
     */
    virtual QByteArray rawImageData() const = 0;

    /**
     * @brief Get metadata for raw image data.
     */
    virtual DicomMetadata rawImageMetadata() const = 0;

    /**
     * @brief Check if context has valid data.
     */
    virtual bool isValid() const = 0;

    /**
     * @brief Clear all data.
     */
    virtual void clear() = 0;
};

/**
 * @class ImageViewerContext
 * @brief Default implementation of IImageViewerContext.
 */
class ImageViewerContext : public IImageViewerContext {
public:
    ImageViewerContext() = default;
    ~ImageViewerContext() override = default;

    ImageSourceType sourceType() const override {
        return m_sourceType;
    }

    void setFilePath(const QString& filePath) override {
        m_filePath = filePath;
        m_sourceType = ImageSourceType::FILE;
    }

    QString filePath() const override {
        return m_filePath;
    }

    void setStudyId(int studyId) override {
        m_studyId = studyId;
        m_sourceType = ImageSourceType::STUDY;
    }

    int studyId() const override {
        return m_studyId;
    }

    void setSeriesIds(const QVector<int>& seriesIds) override {
        m_seriesIds = seriesIds;
        m_sourceType = ImageSourceType::SERIES;
    }

    QVector<int> seriesIds() const override {
        return m_seriesIds;
    }

    void setRawImageData(const QByteArray& data, const DicomMetadata& metadata) override {
        m_rawData = data;
        m_rawMetadata = metadata;
        m_sourceType = ImageSourceType::RAW_DATA;
    }

    QByteArray rawImageData() const override {
        return m_rawData;
    }

    DicomMetadata rawImageMetadata() const override {
        return m_rawMetadata;
    }

    bool isValid() const override {
        switch (m_sourceType) {
            case ImageSourceType::FILE:
                return !m_filePath.isEmpty();
            case ImageSourceType::STUDY:
                return m_studyId > 0;
            case ImageSourceType::SERIES:
                return !m_seriesIds.isEmpty();
            case ImageSourceType::RAW_DATA:
                return !m_rawData.isEmpty();
            default:
                return false;
        }
    }

    void clear() override {
        m_sourceType = ImageSourceType::NONE;
        m_filePath.clear();
        m_studyId = -1;
        m_seriesIds.clear();
        m_rawData.clear();
        m_rawMetadata = DicomMetadata();
    }

private:
    ImageSourceType m_sourceType = ImageSourceType::NONE;
    QString m_filePath;
    int m_studyId = -1;
    QVector<int> m_seriesIds;
    QByteArray m_rawData;
    DicomMetadata m_rawMetadata;
};

} // namespace Etrek::ImageViewer

#endif // IIMAGEVIEWERCONTEXT_H
