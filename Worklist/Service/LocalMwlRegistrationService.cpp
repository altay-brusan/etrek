#include "LocalMwlRegistrationService.h"
#include <QSqlDatabase>
#include <QSqlError>
#include <QRandomGenerator>
#include "AppLoggerFactory.h"
#include "DicomNameFormatter.h"

namespace Etrek::Worklist::Service {

    using Etrek::Specification::Result;
    using Etrek::Dicom::Data::Util::DicomNameFormatter;
    using Etrek::ScanProtocol::Data::Model::PatientModel;
    using Etrek::Worklist::Data::Entity::WorklistEntry;
    using Etrek::Worklist::Data::Entity::WorklistAttribute;
    using Etrek::Worklist::Data::Entity::DicomTag;
    using Etrek::Worklist::Specification::Source;
    using Etrek::Worklist::Specification::ProcedureStepStatus;
    using Etrek::Core::Log::AppLoggerFactory;
    using Etrek::Core::Log::LoggerProvider;
    using Etrek::Core::Globalization::TranslationProvider;

    static inline QString kServiceName() { return "LocalMwlRegistrationService"; }

    LocalMwlRegistrationService::LocalMwlRegistrationService(
        std::shared_ptr<Etrek::Dicom::Repository::DicomRepository> dicomRepo,
        std::shared_ptr<Etrek::Dicom::Repository::DicomTagRepository> tagRepo,
        TranslationProvider* tr)
        : dicomRepository(std::move(dicomRepo))
        , tagRepository(std::move(tagRepo))
        , translator(tr ? tr : &TranslationProvider::Instance())
    {
        AppLoggerFactory factory(LoggerProvider::Instance(), translator);
        logger = factory.CreateLogger(kServiceName());
    }

    LocalMwlRegistrationService::~LocalMwlRegistrationService() = default;

    Result<int> LocalMwlRegistrationService::resolveTagId(const QString& keyword)
    {
        auto tagResult = tagRepository->getByKeyword(keyword);
        if (!tagResult.isSuccess) {
            const auto err = QString("Failed to resolve DICOM tag '%1': %2")
                .arg(keyword).arg(tagResult.message);
            logger->LogError(err);
            return Result<int>::Failure(err);
        }

        auto tag = tagResult.value;
        if (!tag) {
            const auto err = QString("DICOM tag '%1' not found in dictionary").arg(keyword);
            logger->LogError(err);
            return Result<int>::Failure(err);
        }

        return Result<int>::Success(tag->Id);
    }

    bool LocalMwlRegistrationService::addAttributeIfNotEmpty(
        QVector<WorklistAttribute>& attributes,
        const QString& keyword,
        const QString& value)
    {
        if (value.isEmpty()) {
            return true; // Not an error, just skip empty optional fields
        }

        // Resolve tag ID
        auto tagIdResult = resolveTagId(keyword);
        if (!tagIdResult.isSuccess) {
            return false; // Tag resolution failed
        }

        // Get the full tag object
        auto tagResult = tagRepository->getByKeyword(keyword);
        if (!tagResult.isSuccess || !tagResult.value) {
            return false;
        }

        // Create attribute
        WorklistAttribute attr;
        attr.Tag = *tagResult.value; // Copy the full DicomTag object
        attr.TagValue = value;

        attributes.push_back(attr);
        return true;
    }

    Result<QVector<WorklistAttribute>> LocalMwlRegistrationService::buildAttributesFromPatient(
        const PatientModel& patientData)
    {
        QVector<WorklistAttribute> attributes;

        // Required: PatientName in DICOM PN format (LastName^FirstName^MiddleName)
        QString patientName = DicomNameFormatter::formatPatientName(
            patientData.firstName,
            patientData.middleName,
            patientData.lastName
        );
        if (!addAttributeIfNotEmpty(attributes, "PatientName", patientName)) {
            return Result<QVector<WorklistAttribute>>::Failure("Failed to add PatientName attribute");
        }

        // Required: PatientID
        if (!addAttributeIfNotEmpty(attributes, "PatientID", patientData.patientId)) {
            return Result<QVector<WorklistAttribute>>::Failure("Failed to add PatientID attribute");
        }

        // Required: PatientBirthDate in DICOM DA format (YYYYMMDD)
        QString birthDate = DicomNameFormatter::formatDicomDate(patientData.dateOfBirth);
        if (!addAttributeIfNotEmpty(attributes, "PatientBirthDate", birthDate)) {
            return Result<QVector<WorklistAttribute>>::Failure("Failed to add PatientBirthDate attribute");
        }

        // Required: PatientSex (M/F/O or empty)
        QString genderStr = (patientData.gender == Etrek::ScanProtocol::Gender::Male) ? "M" :
                           (patientData.gender == Etrek::ScanProtocol::Gender::Female) ? "F" : "O";
        if (!addAttributeIfNotEmpty(attributes, "PatientSex", genderStr)) {
            return Result<QVector<WorklistAttribute>>::Failure("Failed to add PatientSex attribute");
        }

        // Optional: AdmissionID (0038,0010)
        addAttributeIfNotEmpty(attributes, "AdmissionID", patientData.admissionNumber);

        // Optional: AccessionNumber (0008,0050)
        addAttributeIfNotEmpty(attributes, "AccessionNumber", patientData.accessionNumber);

        // Optional: PatientAddress (0010,1040)
        addAttributeIfNotEmpty(attributes, "PatientAddress", patientData.patientLocation);

        // Optional: ReferringPhysicianName (0008,0090) - map from referringPhysician
        // Note: This should ideally also be in PN format, but for now we'll store as-is
        addAttributeIfNotEmpty(attributes, "ReferringPhysicianName", patientData.referringPhysician);

        logger->LogInfo(QString("Built %1 DICOM attributes from patient data").arg(attributes.size()));
        return Result<QVector<WorklistAttribute>>::Success(attributes);
    }

    Result<QVector<WorklistEntry>> LocalMwlRegistrationService::registerPatient(
        const PatientModel& patientData)
    {
        // Validate input
        if (!patientData.isValid()) {
            const auto err = QString("Invalid patient data: missing required fields");
            logger->LogError(err);
            return Result<QVector<WorklistEntry>>::Failure(err);
        }

        QVector<WorklistEntry> createdEntries;

        // Build attributes from patient data (shared across all body parts)
        auto attributesResult = buildAttributesFromPatient(patientData);
        if (!attributesResult.isSuccess) {
            return Result<QVector<WorklistEntry>>::Failure(attributesResult.message);
        }

        QVector<WorklistAttribute> baseAttributes = attributesResult.value;

        // Create one MWL entry per selected body part
        // (In the future, each body part will map to a separate study)
        for (const auto& bodyPartSelection : patientData.selectedBodyParts) {
            if (!bodyPartSelection.isValid()) {
                logger->LogWarning("Skipping invalid body part selection");
                continue;
            }

            // Create WorklistEntry
            WorklistEntry entry;
            entry.Source = Source::LOCAL;
            entry.Status = ProcedureStepStatus::PENDING;
            entry.CreatedAt = QDateTime::currentDateTime();
            entry.Profile.Id = -1; // No profile for local entries (NULL in DB)

            // Insert the entry
            auto entryResult = dicomRepository->insertWorklistEntry(entry);
            if (!entryResult.isSuccess) {
                const auto err = QString("Failed to insert MWL entry: %1").arg(entryResult.message);
                logger->LogError(err);
                return Result<QVector<WorklistEntry>>::Failure(err);
            }

            entry = entryResult.value; // Get the entry with populated ID

            // Prepare attributes for this entry
            QVector<WorklistAttribute> entryAttributes = baseAttributes;

            // Set EntryId for all attributes
            for (auto& attr : entryAttributes) {
                attr.EntryId = entry.Id;
            }

            // TODO: Add body part specific attributes (BodyPartExamined, etc.)
            // This will be added when we implement the full DICOM hierarchy

            // Insert attributes
            auto attrsResult = dicomRepository->insertWorklistAttributes(entry.Id, entryAttributes);
            if (!attrsResult.isSuccess) {
                const auto err = QString("Failed to insert MWL attributes: %1").arg(attrsResult.message);
                logger->LogError(err);
                return Result<QVector<WorklistEntry>>::Failure(err);
            }

            // Store the inserted attributes in the entry
            entry.Attributes = attrsResult.value;

            createdEntries.push_back(entry);
            logger->LogInfo(QString("Created MWL entry ID=%1 with %2 attributes")
                .arg(entry.Id).arg(entry.Attributes.size()));
        }

        if (createdEntries.isEmpty()) {
            const auto err = QString("No valid body parts selected");
            logger->LogError(err);
            return Result<QVector<WorklistEntry>>::Failure(err);
        }

        logger->LogInfo(QString("Successfully registered patient with %1 MWL entries")
            .arg(createdEntries.size()));
        return Result<QVector<WorklistEntry>>::Success(createdEntries);
    }

} // namespace Etrek::Worklist::Service
