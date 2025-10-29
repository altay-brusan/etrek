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

namespace Etrek::Dicom::Repository {

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

        ~DicomRepository();

    private:
        QSqlDatabase createConnection(const QString& connectionName) const;

        std::shared_ptr<Etrek::Core::Data::Model::DatabaseConnectionSetting> m_connectionSetting;
        Etrek::Core::Globalization::TranslationProvider* translator = nullptr;
        std::shared_ptr<Etrek::Core::Log::AppLogger> logger;
    };
}

#endif // DICOMREPOSITORY_H

