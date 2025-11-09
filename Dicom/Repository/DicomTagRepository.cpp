#include "DicomTagRepository.h"
#include <QSqlQuery>
#include <QSqlError>
#include <QVariant>
#include <QRandomGenerator>
#include "AppLoggerFactory.h"

namespace Etrek::Dicom::Repository {

    using Etrek::Specification::Result;
    using Etrek::Worklist::Data::Entity::DicomTag;
    using Etrek::Core::Data::Model::DatabaseConnectionSetting;
    using Etrek::Core::Globalization::TranslationProvider;
    using Etrek::Core::Log::AppLoggerFactory;
    using Etrek::Core::Log::LoggerProvider;

    static inline QString kRepoName() { return "DicomTagRepository"; }

    DicomTagRepository::DicomTagRepository(
        std::shared_ptr<DatabaseConnectionSetting> connectionSetting,
        TranslationProvider* tr)
        : m_connectionSetting(std::move(connectionSetting))
        , translator(tr ? tr : &TranslationProvider::Instance())
    {
        AppLoggerFactory factory(LoggerProvider::Instance(), translator);
        logger = factory.CreateLogger(kRepoName());
    }

    DicomTagRepository::~DicomTagRepository() = default;

    QSqlDatabase DicomTagRepository::createConnection(const QString& connectionName) const
    {
        QSqlDatabase db = QSqlDatabase::addDatabase("QMYSQL", connectionName);
        db.setHostName(m_connectionSetting->getHostName());
        db.setDatabaseName(m_connectionSetting->getDatabaseName());
        db.setUserName(m_connectionSetting->getEtrekUserName());
        db.setPassword(m_connectionSetting->getPassword());
        db.setPort(m_connectionSetting->getPort());
        return db;
    }

    QString DicomTagRepository::makeTagKey(uint16_t group, uint16_t element)
    {
        return QString("%1,%2")
            .arg(group, 4, 16, QChar('0'))
            .arg(element, 4, 16, QChar('0'));
    }

    Result<bool> DicomTagRepository::loadCache()
    {
        if (cacheLoaded) {
            return Result<bool>::Success(true);
        }

        const QString cx = "dicom_tag_load_cache_" + QString::number(QRandomGenerator::global()->generate());
        {
            QSqlDatabase db = createConnection(cx);
            if (!db.open()) {
                const auto err = QString("Failed to open database: %1").arg(db.lastError().text());
                logger->LogError(err);
                return Result<bool>::Failure(err);
            }

            QSqlQuery q(db);
            q.prepare(R"(
                SELECT id, name, display_name, group_hex, element_hex,
                       pgroup_hex, pelement_hex, is_active, is_retired
                FROM dicom_tags
                WHERE is_active = TRUE
                ORDER BY name
            )");

            if (!q.exec()) {
                const auto err = QString("Failed to load DICOM tags: %1").arg(q.lastError().text());
                logger->LogError(err);
                db.close();
                QSqlDatabase::removeDatabase(cx);
                return Result<bool>::Failure(err);
            }

            keywordCache.clear();
            tagCache.clear();

            while (q.next()) {
                auto tag = std::make_shared<DicomTag>();
                tag->Id = q.value("id").toInt();
                tag->Name = q.value("name").toString();
                tag->DisplayName = q.value("display_name").toString();
                tag->GroupHex = static_cast<uint16_t>(q.value("group_hex").toUInt());
                tag->ElementHex = static_cast<uint16_t>(q.value("element_hex").toUInt());
                tag->PgroupHex = static_cast<uint16_t>(q.value("pgroup_hex").toUInt());
                tag->PelementHex = static_cast<uint16_t>(q.value("pelement_hex").toUInt());
                tag->IsActive = q.value("is_active").toBool();
                tag->IsRetired = q.value("is_retired").toBool();

                // Store in keyword cache
                keywordCache[tag->Name] = tag;

                // Store in tag cache
                QString tagKey = makeTagKey(tag->GroupHex, tag->ElementHex);
                tagCache[tagKey] = tag;
            }

            db.close();
        }
        QSqlDatabase::removeDatabase(cx);

        cacheLoaded = true;
        logger->LogInfo(QString("Loaded %1 DICOM tags into cache").arg(keywordCache.size()));
        return Result<bool>::Success(true);
    }

    Result<std::shared_ptr<DicomTag>> DicomTagRepository::getByKeyword(const QString& keyword)
    {
        // Ensure cache is loaded
        auto loadResult = loadCache();
        if (!loadResult.isSuccess) {
            return Result<std::shared_ptr<DicomTag>>::Failure(loadResult.message);
        }

        // Lookup in cache
        if (keywordCache.contains(keyword)) {
            return Result<std::shared_ptr<DicomTag>>::Success(keywordCache[keyword]);
        }

        // Not found
        const auto err = QString("DICOM tag with keyword '%1' not found").arg(keyword);
        logger->LogWarning(err);
        return Result<std::shared_ptr<DicomTag>>::Failure(err);
    }

    Result<std::shared_ptr<DicomTag>> DicomTagRepository::getByTag(uint16_t group, uint16_t element)
    {
        // Ensure cache is loaded
        auto loadResult = loadCache();
        if (!loadResult.isSuccess) {
            return Result<std::shared_ptr<DicomTag>>::Failure(loadResult.message);
        }

        // Lookup in cache
        QString tagKey = makeTagKey(group, element);
        if (tagCache.contains(tagKey)) {
            return Result<std::shared_ptr<DicomTag>>::Success(tagCache[tagKey]);
        }

        // Not found
        const auto err = QString("DICOM tag (%1,%2) not found")
            .arg(group, 4, 16, QChar('0'))
            .arg(element, 4, 16, QChar('0'));
        logger->LogWarning(err);
        return Result<std::shared_ptr<DicomTag>>::Failure(err);
    }

    void DicomTagRepository::clearCache()
    {
        keywordCache.clear();
        tagCache.clear();
        cacheLoaded = false;
        logger->LogInfo("DICOM tag cache cleared");
    }

} // namespace Etrek::Dicom::Repository
