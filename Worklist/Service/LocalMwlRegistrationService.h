#ifndef ETREK_WORKLIST_SERVICE_LOCALMWLREGISTRATIONSERVICE_H
#define ETREK_WORKLIST_SERVICE_LOCALMWLREGISTRATIONSERVICE_H

#include <memory>
#include <QString>
#include <QSqlDatabase>

#include "Result.h"
#include "DatabaseConnectionSetting.h"
#include "TranslationProvider.h"
#include "AppLogger.h"
#include "PatientModel.h"
#include "WorklistEntry.h"
#include "DicomRepository.h"
#include "DicomTagRepository.h"

namespace Etrek::Worklist::Service {

    /**
     * @brief Service for registering new patients into the local MWL (Modality Worklist)
     *
     * This service handles the complete workflow of creating MWL entries from patient data:
     * - Creates mwl_entries with source='LOCAL' and status='PENDING'
     * - Creates mwl_attributes for all patient demographics
     * - Resolves DICOM tag IDs via DicomTagRepository
     * - Executes all operations in a single database transaction (atomic)
     *
     * DICOM Hierarchy (for default settings: 1 Study, 1 Series, N Images):
     * Patient → Study (one per body part) → Series (one per study) → Images (multiple)
     */
    class LocalMwlRegistrationService
    {
    public:
        explicit LocalMwlRegistrationService(
            std::shared_ptr<Etrek::Dicom::Repository::DicomRepository> dicomRepo,
            std::shared_ptr<Etrek::Dicom::Repository::DicomTagRepository> tagRepo,
            Etrek::Core::Globalization::TranslationProvider* tr = nullptr);

        /**
         * @brief Register a new patient into the local MWL system
         *
         * Creates MWL entry with attributes for patient demographics.
         * Each selected body part will result in a separate MWL entry (future: separate studies).
         *
         * Transaction flow:
         * 1. BEGIN TRANSACTION
         * 2. For each body part:
         *    a. Create WorklistEntry (source=LOCAL, status=PENDING)
         *    b. Build attribute list from PatientModel
         *    c. Resolve DICOM tag IDs
         *    d. Insert WorklistAttributes
         * 3. COMMIT (or ROLLBACK on error)
         *
         * @param patientData Patient demographics and selected body parts
         * @return Result containing list of created WorklistEntry objects, or error message
         */
        Etrek::Specification::Result<QVector<Etrek::Worklist::Data::Entity::WorklistEntry>>
            registerPatient(const Etrek::ScanProtocol::Data::Model::PatientModel& patientData);

        ~LocalMwlRegistrationService();

    private:
        /**
         * @brief Create attributes list from patient data
         * @param patientData Source patient model
         * @param tagRepo Repository for resolving tag IDs
         * @return Result containing list of attributes ready for insert
         */
        Etrek::Specification::Result<QVector<Etrek::Worklist::Data::Entity::WorklistAttribute>>
            buildAttributesFromPatient(const Etrek::ScanProtocol::Data::Model::PatientModel& patientData);

        /**
         * @brief Resolve DICOM tag by keyword
         * @param keyword Tag name (e.g., "PatientName")
         * @return Result containing tag ID, or error
         */
        Etrek::Specification::Result<int> resolveTagId(const QString& keyword);

        /**
         * @brief Add attribute to list if value is not empty
         * @param attributes Target list
         * @param keyword DICOM tag keyword
         * @param value Tag value
         * @return true if attribute was added successfully
         */
        bool addAttributeIfNotEmpty(QVector<Etrek::Worklist::Data::Entity::WorklistAttribute>& attributes,
                                    const QString& keyword,
                                    const QString& value);

        std::shared_ptr<Etrek::Dicom::Repository::DicomRepository> dicomRepository;
        std::shared_ptr<Etrek::Dicom::Repository::DicomTagRepository> tagRepository;
        Etrek::Core::Globalization::TranslationProvider* translator = nullptr;
        std::shared_ptr<Etrek::Core::Log::AppLogger> logger;
    };

} // namespace Etrek::Worklist::Service

#endif // ETREK_WORKLIST_SERVICE_LOCALMWLREGISTRATIONSERVICE_H
