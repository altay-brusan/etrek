#ifndef ETREK_DICOM_REPOSITORY_DICOMTAGREPOSITORY_H
#define ETREK_DICOM_REPOSITORY_DICOMTAGREPOSITORY_H

#include <QSqlDatabase>
#include <QString>
#include <QMap>
#include <memory>

#include "Result.h"
#include "DatabaseConnectionSetting.h"
#include "TranslationProvider.h"
#include "AppLogger.h"
#include "DicomTag.h" // From Common/Include/Worklist/Data/Entity/DicomTag.h

namespace Etrek::Dicom::Repository {

    namespace wle = Etrek::Worklist::Data::Entity;

    /**
     * @brief Repository for DICOM tag dictionary lookups with caching
     *
     * This lightweight repository reads from the dicom_tags table and provides
     * efficient tag lookups by keyword or tag number (group, element).
     * Uses an internal cache to avoid repeated database hits.
     */
    class DicomTagRepository
    {
    public:
        explicit DicomTagRepository(
            std::shared_ptr<Etrek::Core::Data::Model::DatabaseConnectionSetting> connectionSetting,
            Etrek::Core::Globalization::TranslationProvider* tr = nullptr);

        /**
         * @brief Get DICOM tag by standard name/keyword
         * @param keyword Standard tag name (e.g., "PatientName", "AccessionNumber")
         * @return Result containing shared pointer to DicomTag, or nullopt if not found
         */
        Etrek::Specification::Result<std::shared_ptr<wle::DicomTag>>
            getByKeyword(const QString& keyword);

        /**
         * @brief Get DICOM tag by group and element numbers
         * @param group Group part of DICOM tag (e.g., 0x0008, 0x0010)
         * @param element Element part of DICOM tag (e.g., 0x0050, 0x0010)
         * @return Result containing shared pointer to DicomTag, or nullopt if not found
         */
        Etrek::Specification::Result<std::shared_ptr<wle::DicomTag>>
            getByTag(uint16_t group, uint16_t element);

        /**
         * @brief Clear the internal cache
         * Call this if you need to force reload from database
         */
        void clearCache();

        ~DicomTagRepository();

    private:
        QSqlDatabase createConnection(const QString& connectionName) const;

        /**
         * @brief Load all tags from database into cache
         * @return Result indicating success or failure
         */
        Etrek::Specification::Result<bool> loadCache();

        /**
         * @brief Get tag key for group/element lookup
         */
        static QString makeTagKey(uint16_t group, uint16_t element);

        std::shared_ptr<Etrek::Core::Data::Model::DatabaseConnectionSetting> m_connectionSetting;
        Etrek::Core::Globalization::TranslationProvider* translator = nullptr;
        std::shared_ptr<Etrek::Core::Log::AppLogger> logger;

        // Cache: keyword -> DicomTag
        QMap<QString, std::shared_ptr<wle::DicomTag>> keywordCache;

        // Cache: "GGGG,EEEE" -> DicomTag
        QMap<QString, std::shared_ptr<wle::DicomTag>> tagCache;

        bool cacheLoaded = false;
    };

} // namespace Etrek::Dicom::Repository

#endif // ETREK_DICOM_REPOSITORY_DICOMTAGREPOSITORY_H
