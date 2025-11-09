#ifndef DICOMREPOSITORY_H
#define DICOMREPOSITORY_H

#include <QSqlDatabase>
#include <QVector>
#include <memory>
#include <optional>

#include "Result.h"
#include "DatabaseConnectionSetting.h"
#include "TranslationProvider.h"
#include "AppLogger.h"
#include "Study.h"
#include "Patient.h"
#include "EntityStatus.h"
#include "WorklistEntry.h"  // From Common/Include/Worklist/Data/Entity/
#include "WorklistAttribute.h"  // From Common/Include/Worklist/Data/Entity/

namespace Etrek::Dicom::Repository {

    namespace wle = Etrek::Worklist::Data::Entity;
    namespace wls = Etrek::Worklist::Specification;

    class DicomRepository
    {
    public:
        explicit DicomRepository(std::shared_ptr<Etrek::Core::Data::Model::DatabaseConnectionSetting> connectionSetting,
            Etrek::Core::Globalization::TranslationProvider* tr = nullptr);

        Etrek::Specification::Result<QVector<Etrek::Dicom::Data::Entity::Study>>
            getStudiesByAdmissionId(const QString& admissionId) const;

        Etrek::Specification::Result<Etrek::Dicom::Data::Entity::Patient>
            insertPatient(Etrek::Dicom::Data::Entity::Patient& patient);

        Etrek::Specification::Result<bool>
            updatePatient(const Etrek::Dicom::Data::Entity::Patient& patient);

        Etrek::Specification::Result<std::optional<Etrek::Dicom::Data::Entity::Patient>>
            findPatientByIdAndIssuer(const QString& patientId, const QString& issuerOfPatientId) const;

        Etrek::Specification::Result<Etrek::Dicom::Data::Entity::Patient>
            upsertPatient(Etrek::Dicom::Data::Entity::Patient& patient);

        // Status management methods
        Etrek::Specification::Result<Etrek::Dicom::Data::Entity::EntityStatus>
            insertEntityStatus(Etrek::Dicom::Data::Entity::EntityStatus& status);

        Etrek::Specification::Result<std::optional<Etrek::Dicom::Data::Entity::EntityStatus>>
            getCurrentStatus(Etrek::Dicom::Data::Entity::EntityType entityType, int entityId) const;

        Etrek::Specification::Result<QVector<Etrek::Dicom::Data::Entity::EntityStatus>>
            getStatusHistory(Etrek::Dicom::Data::Entity::EntityType entityType, int entityId) const;

        Etrek::Specification::Result<QVector<Etrek::Dicom::Data::Entity::EntityStatus>>
            getEntitiesByStatus(Etrek::Dicom::Data::Entity::EntityType entityType,
                                Etrek::Dicom::Data::Entity::WorkflowStatus status) const;

        Etrek::Specification::Result<QVector<Etrek::Dicom::Data::Entity::EntityStatus>>
            getAssignedEntities(int userId, Etrek::Dicom::Data::Entity::WorkflowStatus status) const;

        // Modality Worklist methods
        Etrek::Specification::Result<wle::WorklistEntry>
            insertWorklistEntry(wle::WorklistEntry& entry);

        Etrek::Specification::Result<wle::WorklistAttribute>
            insertWorklistAttribute(wle::WorklistAttribute& attribute);

        Etrek::Specification::Result<QVector<wle::WorklistAttribute>>
            insertWorklistAttributes(int mwlEntryId, const QVector<wle::WorklistAttribute>& attributes);

        ~DicomRepository();

    private:
        QSqlDatabase createConnection(const QString& connectionName) const;

        std::shared_ptr<Etrek::Core::Data::Model::DatabaseConnectionSetting> m_connectionSetting;
        Etrek::Core::Globalization::TranslationProvider* translator = nullptr;
        std::shared_ptr<Etrek::Core::Log::AppLogger> logger;
    };
}

#endif // DICOMREPOSITORY_H

