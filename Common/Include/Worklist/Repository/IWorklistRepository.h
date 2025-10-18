#ifndef IWORKLISTREPOSITORY_H
#define IWORKLISTREPOSITORY_H

#include <QList>
#include <QString>
#include <QDateTime>
#include <memory>
#include "Specification/Result.h"
#include "Worklist/Data/Entity/DicomTag.h"
#include "Worklist/Data/Entity/WorklistEntry.h"
#include "Worklist/Data/Entity/WorklistAttribute.h"
#include "Worklist/Data/Entity/WorklistProfile.h"
#include "Worklist/Specification/WorklistEnum.h"

class DatabaseConnectionSetting;

namespace Etrek::Worklist::Repository {

    namespace spc = Etrek::Specification;
    namespace ent = Etrek::Worklist::Data::Entity;

    /**
     * @class WorklistRepository
     * @brief Provides data access and management for DICOM worklist entries, profiles, and attributes.
     *
     * WorklistRepository encapsulates all database operations related to DICOM worklist management,
     * including CRUD operations for worklist entries, profiles, DICOM tags, and attributes.
     * It supports querying by profile, status, source, and time, and provides signals for entry changes.
     * Logging and translation support are integrated for error handling and diagnostics.
     */
    class IWorklistRepository : public QObject {
        Q_OBJECT

    public:

        /**
         * @brief Retrieves all worklist profiles.
         * @return Result containing a list of WorklistProfile objects.
         */
        virtual spc::Result<QList<ent::WorklistProfile>> getProfiles() const = 0;

        /**
         * @brief Retrieves all DICOM tags associated with a profile.
         * @param profileId The profile ID.
         * @return Result containing a list of DicomTag objects.
         */
        virtual spc::Result<QList<ent::DicomTag>> getTagsByProfile(int profileId) const = 0;

        /**
         * @brief Retrieves identifier tags for a profile.
         * @param profileId The profile ID.
         * @return Result containing a list of DicomTag objects.
         */
        virtual spc::Result<QList<ent::DicomTag>> getIdentifiersByProfile(int profileId) const = 0;

        /**
         * @brief Retrieves a worklist entry by its ID.
         * @param entryId The entry ID.
         * @return Result containing the WorklistEntry object.
         */
        virtual spc::Result<ent::WorklistEntry> getWorklistEntryById(int entryId) const = 0;

        /**
         * @brief Retrieves worklist entries within a date/time range.
         * @param from Start date/time (optional).
         * @param to End date/time (optional).
         * @return Result containing a list of WorklistEntry objects.
         */
        virtual spc::Result<QList<ent::WorklistEntry>> getWorklistEntries(const QDateTime* from, const QDateTime* to) const = 0;

        /**
         * @brief Retrieves worklist entries by source.
         * @param source The source (e.g., LOCAL, RIS).
         * @return Result containing a list of WorklistEntry objects.
         */
        virtual spc::Result<QList<ent::WorklistEntry>> getWorklistEntries(Source source) const = 0;

        /**
         * @brief Retrieves worklist entries by procedure step status.
         * @param status The procedure step status.
         * @return Result containing a list of WorklistEntry objects.
         */
        virtual spc::Result<QList<ent::WorklistEntry>> getWorklistEntries(ProcedureStepStatus status) const = 0;

        /**
         * @brief Updates the status of a worklist entry.
         * @param entryId The entry ID.
         * @param newStatus The new procedure step status.
         * @return Result containing a status message.
         */
        virtual spc::Result<QString> updateWorklistStatus(int entryId, ProcedureStepStatus newStatus) = 0;

        /**
         * @brief Deletes worklist entries before a specified date.
         * @param beforeDate The cutoff date.
         * @return Result indicating success or failure.
         */
        virtual spc::Result<bool> deleteWorklistEntries(const QDateTime& beforeDate) = 0;

        /**
         * @brief Deletes worklist entries by their IDs.
         * @param entryIds List of entry IDs to delete.
         * @return Result indicating success or failure.
         */
        virtual spc::Result<bool> deleteWorklistEntries(const QList<int>& entryIds) = 0;

        /**
         * @brief Retrieves a worklist entry matching the given entry object.
         * @param entry The WorklistEntry to match.
         * @return Result containing the matching WorklistEntry.
         */
        virtual spc::Result<ent::WorklistEntry> getWorklistEntry(const ent::WorklistEntry& entry) const = 0;

        /**
         * @brief Retrieves a worklist entry matching the given entry and profile.
         * @param entry The WorklistEntry to match.
         * @param profile The WorklistProfile to match.
         * @return Result containing the matching WorklistEntry.
         */
        virtual spc::Result<ent::WorklistEntry> getWorklistEntry(const ent::WorklistEntry& entry, const ent::WorklistProfile& profile) const = 0;

        /**
         * @brief Finds a worklist entry by profile and tag values.
         * @param profileId The profile ID.
         * @param tagIdToValue Map of tag IDs to their values.
         * @return Result containing the entry ID if found.
         */
        virtual spc::Result<int> findWorklistEntryByIdentifiers(int profileId, const QMap<int, QString>& tagIdToValue) const = 0;

        /**
         * @brief Creates a new worklist entry.
         * @param entry The WorklistEntry to create.
         * @return Result containing the new entry ID.
         */
        virtual spc::Result<int> createWorklistEntry(const ent::WorklistEntry& entry) = 0;

        /**
         * @brief Updates an existing worklist entry.
         * @param entry The WorklistEntry to update.
         * @return Result containing the updated entry ID.
         */
        virtual spc::Result<int> updateWorklistEntry(const ent::WorklistEntry& entry) = 0;

        /**
         * @brief Adds a new DICOM tag.
         * @param tag The DicomTag to add.
         * @return Result containing the new tag ID.
         */
        virtual spc::Result<int> addDicomTag(const ent::DicomTag& tag) = 0;

        /**
         * @brief Updates the active status of a DICOM tag.
         * @param tagId The tag ID.
         * @param isActive True to set active, false to set inactive.
         * @return Result indicating success or failure.
         */
        virtual spc::Result<bool> updateDicomTagActiveStatus(int tagId, bool isActive) = 0;

        /**
         * @brief Updates the retired status of a DICOM tag.
         * @param tagId The tag ID.
         * @param isRetired True to set retired, false to set not retired.
         * @return Result indicating success or failure.
         */
        virtual spc::Result<bool> updateDicomTagRetiredStatus(int tagId, bool isRetired) = 0;

        /**
         * @brief Retrieves active identifier tags for a profile.
         * @param profileId The profile ID.
         * @return Result containing a list of DicomTag objects.
         */
        virtual spc::Result<QList<ent::DicomTag>> getActiveIdentifierTags(int profileId) const = 0;

        /**
         * @brief Retrieves mandatory identifier tags for a profile.
         * @param profileId The profile ID.
         * @return Result containing a list of DicomTag objects.
         */
        virtual spc::Result<QList<ent::DicomTag>> getMandatoryIdentifierTags(int profileId) const = 0;

        /**
         * @brief Updates identifier flags for a tag in a profile.
         * @param profileId The profile ID.
         * @param tagId The tag ID.
         * @param isPotential True if the tag is a potential identifier.
         * @param isActive True if the tag is active.
         * @return Result indicating success or failure.
         */
        virtual spc::Result<bool> updateIdentifierFlags(int profileId, int tagId, bool isPotential, bool isActive) = 0;

        /**
         * @brief Destructor for IWorklistRepository.
         */
        virtual ~IWorklistRepository() = default;

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


    };

} // namespace Etrek::Repository

#endif // WORKLISTREPOSITORY_H
