#ifndef WORKLISTREPOSITORY_H
#define WORKLISTREPOSITORY_H

#include <QList>
#include <memory>
#include <QString>
#include <QDateTime>
#include <QSqlDatabase>
#include "Result.h"
#include "DicomTag.h"
#include "DatabaseConnectionSetting.h"
#include "WorklistEntry.h"
#include "WorklistAttribute.h"
#include "WorklistProfile.h"
#include "TranslationProvider.h"
#include "AppLogger.h"

namespace Etrek::Worklist::Repository {

    namespace spc = Etrek::Specification;
    namespace ent = Etrek::Worklist::Data::Entity;
    namespace mdl = Etrek::Core::Data::Model;
    namespace lg = Etrek::Core::Log;
    namespace glob = Etrek::Core::Globalization;

    /**
     * @class WorklistRepository
     * @brief Provides data access and management for DICOM worklist entries, profiles, and attributes.
     *
     * WorklistRepository encapsulates all database operations related to DICOM worklist management,
     * including CRUD operations for worklist entries, profiles, DICOM tags, and attributes.
     * It supports querying by profile, status, source, and time, and provides signals for entry changes.
     * Logging and translation support are integrated for error handling and diagnostics.
     */
    class WorklistRepository : public QObject {
        Q_OBJECT

    public:
        /**
         * @brief Constructs a WorklistRepository with the given database connection settings.
         * @param connectionSetting Shared pointer to the database connection settings.
         * @param parent Optional QObject parent.
         */
        explicit WorklistRepository(std::shared_ptr<mdl::DatabaseConnectionSetting> connectionSetting, QObject* parent = nullptr);

        /**
         * @brief Retrieves all worklist profiles.
         * @return Result containing a list of WorklistProfile objects.
         */
        virtual spc::Result<QList<ent::WorklistProfile>> getProfiles() const;

        /**
         * @brief Retrieves all DICOM tags associated with a profile.
         * @param profileId The profile ID.
         * @return Result containing a list of DicomTag objects.
         */
        virtual spc::Result<QList<ent::DicomTag>> getTagsByProfile(int profileId) const;

        /**
         * @brief Retrieves identifier tags for a profile.
         * @param profileId The profile ID.
         * @return Result containing a list of DicomTag objects.
         */
        spc::Result<QList<ent::DicomTag>> getIdentifiersByProfile(int profileId) const;

        /**
         * @brief Retrieves a worklist entry by its ID.
         * @param entryId The entry ID.
         * @return Result containing the WorklistEntry object.
         */
        virtual spc::Result<ent::WorklistEntry> getWorklistEntryById(int entryId) const;

        /**
         * @brief Retrieves worklist entries within a date/time range.
         * @param from Start date/time (optional).
         * @param to End date/time (optional).
         * @return Result containing a list of WorklistEntry objects.
         */
        spc::Result<QList<ent::WorklistEntry>> getWorklistEntries(const QDateTime* from, const QDateTime* to) const;

        /**
         * @brief Retrieves worklist entries by source.
         * @param source The source (e.g., LOCAL, RIS).
         * @return Result containing a list of WorklistEntry objects.
         */
        spc::Result<QList<ent::WorklistEntry>> getWorklistEntries(Source source) const;

        /**
         * @brief Retrieves worklist entries by procedure step status.
         * @param status The procedure step status.
         * @return Result containing a list of WorklistEntry objects.
         */
        spc::Result<QList<ent::WorklistEntry>> getWorklistEntries(ProcedureStepStatus status) const;

        /**
         * @brief Updates the status of a worklist entry.
         * @param entryId The entry ID.
         * @param newStatus The new procedure step status.
         * @return Result containing a status message.
         */
        spc::Result<QString> updateWorklistStatus(int entryId, ProcedureStepStatus newStatus);

        /**
         * @brief Deletes worklist entries before a specified date.
         * @param beforeDate The cutoff date.
         * @return Result indicating success or failure.
         */
        spc::Result<bool> deleteWorklistEntries(const QDateTime& beforeDate);

        /**
         * @brief Deletes worklist entries by their IDs.
         * @param entryIds List of entry IDs to delete.
         * @return Result indicating success or failure.
         */
        spc::Result<bool> deleteWorklistEntries(const QList<int>& entryIds);

        /**
         * @brief Retrieves a worklist entry matching the given entry object.
         * @param entry The WorklistEntry to match.
         * @return Result containing the matching WorklistEntry.
         */
        spc::Result<ent::WorklistEntry> getWorklistEntry(const ent::WorklistEntry& entry) const;

        /**
         * @brief Retrieves a worklist entry matching the given entry and profile.
         * @param entry The WorklistEntry to match.
         * @param profile The WorklistProfile to match.
         * @return Result containing the matching WorklistEntry.
         */
        spc::Result<ent::WorklistEntry> getWorklistEntry(const ent::WorklistEntry& entry, const ent::WorklistProfile& profile) const;

        /**
         * @brief Finds a worklist entry by profile and tag values.
         * @param profileId The profile ID.
         * @param tagIdToValue Map of tag IDs to their values.
         * @return Result containing the entry ID if found.
         */
        spc::Result<int> findWorklistEntryByIdentifiers(int profileId, const QMap<int, QString>& tagIdToValue) const;

        /**
         * @brief Creates a new worklist entry.
         * @param entry The WorklistEntry to create.
         * @return Result containing the new entry ID.
         */
        virtual spc::Result<int> createWorklistEntry(const ent::WorklistEntry& entry);

        /**
         * @brief Updates an existing worklist entry.
         * @param entry The WorklistEntry to update.
         * @return Result containing the updated entry ID.
         */
        virtual spc::Result<int> updateWorklistEntry(const ent::WorklistEntry& entry);

        /**
         * @brief Adds a new DICOM tag.
         * @param tag The DicomTag to add.
         * @return Result containing the new tag ID.
         */
        spc::Result<int> addDicomTag(const ent::DicomTag& tag);

        /**
         * @brief Updates the active status of a DICOM tag.
         * @param tagId The tag ID.
         * @param isActive True to set active, false to set inactive.
         * @return Result indicating success or failure.
         */
        spc::Result<bool> updateDicomTagActiveStatus(int tagId, bool isActive);

        /**
         * @brief Updates the retired status of a DICOM tag.
         * @param tagId The tag ID.
         * @param isRetired True to set retired, false to set not retired.
         * @return Result indicating success or failure.
         */
        spc::Result<bool> updateDicomTagRetiredStatus(int tagId, bool isRetired);

        /**
         * @brief Retrieves active identifier tags for a profile.
         * @param profileId The profile ID.
         * @return Result containing a list of DicomTag objects.
         */
        spc::Result<QList<ent::DicomTag>> getActiveIdentifierTags(int profileId) const;

        /**
         * @brief Retrieves mandatory identifier tags for a profile.
         * @param profileId The profile ID.
         * @return Result containing a list of DicomTag objects.
         */
        spc::Result<QList<ent::DicomTag>> getMandatoryIdentifierTags(int profileId) const;

        /**
         * @brief Updates identifier flags for a tag in a profile.
         * @param profileId The profile ID.
         * @param tagId The tag ID.
         * @param isPotential True if the tag is a potential identifier.
         * @param isActive True if the tag is active.
         * @return Result indicating success or failure.
         */
        spc::Result<bool> updateIdentifierFlags(int profileId, int tagId, bool isPotential, bool isActive);

        /**
         * @brief Destructor for WorklistRepository.
         */
        virtual ~WorklistRepository() = default;

    signals:
        /**
         * @brief Emitted when a worklist entry is created.
         * @param entry The created WorklistEntry.
         */
        void worklistEntryCreated(const ent::WorklistEntry& entry);

        /**
         * @brief Emitted when a worklist entry is updated.
         * @param entry The updated WorklistEntry.
         */
        void worklistEntryUpdated(const ent::WorklistEntry& entry);

        /**
         * @brief Emitted when a worklist entry is deleted.
         * @param entryId The ID of the deleted entry.
         */
        void worklistEntryDeleted(int entryId);

    private:
        /**
         * @brief Loads attributes for the given entry IDs from the database.
         * @param entryIds List of entry IDs.
         * @param db The database connection.
         * @return Map of entry IDs to their attributes.
         */
        QMap<int, QList<ent::WorklistAttribute>> loadAttributesForEntries(const QList<int>& entryIds, QSqlDatabase& db) const;

        /**
         * @brief Retrieves detailed information for a worklist entry.
         * @param entryId The entry ID.
         * @return Result containing the WorklistEntry details.
         */
        spc::Result<ent::WorklistEntry> getWorklistEntryDetails(int entryId) const;

        /**
         * @brief Creates a new database connection with the specified name.
         * @param connectionName The name for the connection.
         * @return The QSqlDatabase object.
         */
        QSqlDatabase createConnection(const QString& connectionName) const;

        std::shared_ptr<mdl::DatabaseConnectionSetting> m_connectionSetting;
        glob::TranslationProvider* translator;
        std::shared_ptr<lg::AppLogger> logger;
    };

} // namespace Etrek::Repository

#endif // WORKLISTREPOSITORY_H
