#include "DicomRepository.h"
#include <QSqlQuery>
#include <QSqlError>
#include <QVariant>
#include <QRandomGenerator>
#include "AppLoggerFactory.h"

namespace Etrek::Dicom::Repository {

    using Etrek::Specification::Result;
    using Etrek::Dicom::Data::Entity::Study;
    using Etrek::Dicom::Data::Entity::Patient;
    using Etrek::Core::Data::Model::DatabaseConnectionSetting;
    using Etrek::Core::Globalization::TranslationProvider;
    using Etrek::Core::Log::AppLoggerFactory;
    using Etrek::Core::Log::LoggerProvider;

    static inline QString kRepoName() { return "DicomRepository"; }

    DicomRepository::DicomRepository(std::shared_ptr<DatabaseConnectionSetting> connectionSetting,
        TranslationProvider* tr)
        : m_connectionSetting(std::move(connectionSetting))
        , translator(tr ? tr : &TranslationProvider::Instance())
    {
        AppLoggerFactory factory(LoggerProvider::Instance(), translator);
        logger = factory.CreateLogger(kRepoName());
    }

    DicomRepository::~DicomRepository() = default;

    QSqlDatabase DicomRepository::createConnection(const QString& connectionName) const
    {
        QSqlDatabase db = QSqlDatabase::addDatabase("QMYSQL", connectionName);
        db.setHostName(m_connectionSetting->getHostName());
        db.setDatabaseName(m_connectionSetting->getDatabaseName());
        db.setUserName(m_connectionSetting->getEtrekUserName());
        db.setPassword(m_connectionSetting->getPassword());
        db.setPort(m_connectionSetting->getPort());
        return db;
    }

    Result<QVector<Study>> DicomRepository::getStudiesByAdmissionId(const QString& admissionId) const
    {
        QVector<Study> rows;
        const QString cx = "dicom_conn_studies_by_adm_" + QString::number(QRandomGenerator::global()->generate());
        {
            QSqlDatabase db = createConnection(cx);
            if (!db.open()) {
                const auto err = QString("Failed to open database: %1").arg(db.lastError().text());
                logger->LogError(err);
                return Result<QVector<Study>>::Failure(err);
            }

            QSqlQuery q(db);
            q.prepare(R"(
                SELECT id, patient_id, study_instance_uid, study_id, admission_id, accession_number,
                       issuer_of_accession_number, referring_physician_name,
                       study_date, study_time, study_description,
                       patient_age, patient_size, allergy
                FROM studies
                WHERE (:adm IS NULL AND admission_id IS NULL) OR admission_id = :adm
                ORDER BY id
            )");
            const QString trimmed = admissionId.trimmed();
            q.bindValue(":adm", trimmed.isEmpty() ? QVariant(QVariant::String) : QVariant(trimmed));
            if (!q.exec()) {
                const auto err = QString("Query failed: %1").arg(q.lastError().text());
                logger->LogError(err);
                return Result<QVector<Study>>::Failure(err);
            }
            while (q.next()) {
                Study s;
                s.Id = q.value("id").toInt();
                s.PatientId = q.value("patient_id").toInt();
                s.StudyInstanceUID = q.value("study_instance_uid").toString();
                s.StudyId = q.value("study_id").toString();
                s.AdmissionId = q.value("admission_id").toString();
                s.AccessionNumber = q.value("accession_number").toString();
                s.IssuerOfAccessionNumber = q.value("issuer_of_accession_number").toString();
                s.ReferringPhysicianName = q.value("referring_physician_name").toString();
                s.StudyDate = q.value("study_date").toString();
                s.StudyTime = q.value("study_time").toString();
                s.StudyDescription = q.value("study_description").toString();
                s.PatientAge = q.value("patient_age").toInt();
                s.PatientSize = q.value("patient_size").toInt();
                s.Allergy = q.value("allergy").toString();
                rows.push_back(std::move(s));
            }
        }
        return Result<QVector<Study>>::Success(rows);
    }

    Result<Patient> DicomRepository::insertPatient(Patient& patient)
    {
        const QString cx = "dicom_conn_insert_patient_" + QString::number(QRandomGenerator::global()->generate());
        {
            QSqlDatabase db = createConnection(cx);
            if (!db.open()) {
                const auto err = QString("Failed to open database: %1").arg(db.lastError().text());
                logger->LogError(err);
                return Result<Patient>::Failure(err);
            }

            QSqlQuery q(db);
            q.prepare(R"(
                INSERT INTO patients (
                    patient_name, patient_id, issuer_of_patient_id, type_of_patient_id,
                    issuer_of_patient_id_qualifiers, other_patient_id, patient_sex,
                    patient_birth_date, patient_comments, patient_allergies,
                    requesting_physician, patient_address
                ) VALUES (
                    :patient_name, :patient_id, :issuer_of_patient_id, :type_of_patient_id,
                    :issuer_of_patient_id_qualifiers, :other_patient_id, :patient_sex,
                    :patient_birth_date, :patient_comments, :patient_allergies,
                    :requesting_physician, :patient_address
                )
            )");

            q.bindValue(":patient_name", patient.PatientName.isEmpty() ? QVariant(QVariant::String) : patient.PatientName);
            q.bindValue(":patient_id", patient.PatientId);
            q.bindValue(":issuer_of_patient_id", patient.IssuerOfPatientId.isEmpty() ? QVariant(QVariant::String) : patient.IssuerOfPatientId);
            q.bindValue(":type_of_patient_id", patient.TypeOfPatientId.isEmpty() ? QVariant(QVariant::String) : patient.TypeOfPatientId);
            q.bindValue(":issuer_of_patient_id_qualifiers", patient.IssuerOfPatientIdQualifiers.isEmpty() ? QVariant(QVariant::String) : patient.IssuerOfPatientIdQualifiers);
            q.bindValue(":other_patient_id", patient.OtherPatientId.isEmpty() ? QVariant(QVariant::String) : patient.OtherPatientId);
            q.bindValue(":patient_sex", patient.PatientSex.isEmpty() ? QVariant(QVariant::String) : patient.PatientSex);
            q.bindValue(":patient_birth_date", patient.PatientBirthDate.isValid() ? patient.PatientBirthDate : QVariant(QVariant::Date));
            q.bindValue(":patient_comments", patient.PatientComments.isEmpty() ? QVariant(QVariant::String) : patient.PatientComments);
            q.bindValue(":patient_allergies", patient.PatientAllergies.isEmpty() ? QVariant(QVariant::String) : patient.PatientAllergies);
            q.bindValue(":requesting_physician", patient.RequestingPhysician.isEmpty() ? QVariant(QVariant::String) : patient.RequestingPhysician);
            q.bindValue(":patient_address", patient.PatientAddress.isEmpty() ? QVariant(QVariant::String) : patient.PatientAddress);

            if (!q.exec()) {
                const auto err = QString("Failed to insert patient: %1").arg(q.lastError().text());
                logger->LogError(err);
                return Result<Patient>::Failure(err);
            }

            patient.Id = q.lastInsertId().toInt();
            db.close();
        }
        QSqlDatabase::removeDatabase(cx);
        return Result<Patient>::Success(patient);
    }

    Result<bool> DicomRepository::updatePatient(const Patient& patient)
    {
        if (patient.Id < 0) {
            const auto err = QString("Cannot update patient: invalid ID");
            logger->LogError(err);
            return Result<bool>::Failure(err);
        }

        const QString cx = "dicom_conn_update_patient_" + QString::number(QRandomGenerator::global()->generate());
        {
            QSqlDatabase db = createConnection(cx);
            if (!db.open()) {
                const auto err = QString("Failed to open database: %1").arg(db.lastError().text());
                logger->LogError(err);
                return Result<bool>::Failure(err);
            }

            QSqlQuery q(db);
            q.prepare(R"(
                UPDATE patients SET
                    patient_name = :patient_name,
                    patient_id = :patient_id,
                    issuer_of_patient_id = :issuer_of_patient_id,
                    type_of_patient_id = :type_of_patient_id,
                    issuer_of_patient_id_qualifiers = :issuer_of_patient_id_qualifiers,
                    other_patient_id = :other_patient_id,
                    patient_sex = :patient_sex,
                    patient_birth_date = :patient_birth_date,
                    patient_comments = :patient_comments,
                    patient_allergies = :patient_allergies,
                    requesting_physician = :requesting_physician,
                    patient_address = :patient_address
                WHERE id = :id
            )");

            q.bindValue(":id", patient.Id);
            q.bindValue(":patient_name", patient.PatientName.isEmpty() ? QVariant(QVariant::String) : patient.PatientName);
            q.bindValue(":patient_id", patient.PatientId);
            q.bindValue(":issuer_of_patient_id", patient.IssuerOfPatientId.isEmpty() ? QVariant(QVariant::String) : patient.IssuerOfPatientId);
            q.bindValue(":type_of_patient_id", patient.TypeOfPatientId.isEmpty() ? QVariant(QVariant::String) : patient.TypeOfPatientId);
            q.bindValue(":issuer_of_patient_id_qualifiers", patient.IssuerOfPatientIdQualifiers.isEmpty() ? QVariant(QVariant::String) : patient.IssuerOfPatientIdQualifiers);
            q.bindValue(":other_patient_id", patient.OtherPatientId.isEmpty() ? QVariant(QVariant::String) : patient.OtherPatientId);
            q.bindValue(":patient_sex", patient.PatientSex.isEmpty() ? QVariant(QVariant::String) : patient.PatientSex);
            q.bindValue(":patient_birth_date", patient.PatientBirthDate.isValid() ? patient.PatientBirthDate : QVariant(QVariant::Date));
            q.bindValue(":patient_comments", patient.PatientComments.isEmpty() ? QVariant(QVariant::String) : patient.PatientComments);
            q.bindValue(":patient_allergies", patient.PatientAllergies.isEmpty() ? QVariant(QVariant::String) : patient.PatientAllergies);
            q.bindValue(":requesting_physician", patient.RequestingPhysician.isEmpty() ? QVariant(QVariant::String) : patient.RequestingPhysician);
            q.bindValue(":patient_address", patient.PatientAddress.isEmpty() ? QVariant(QVariant::String) : patient.PatientAddress);

            if (!q.exec()) {
                const auto err = QString("Failed to update patient: %1").arg(q.lastError().text());
                logger->LogError(err);
                return Result<bool>::Failure(err);
            }

            db.close();
        }
        QSqlDatabase::removeDatabase(cx);
        return Result<bool>::Success(true);
    }

    Result<std::optional<Patient>> DicomRepository::findPatientByIdAndIssuer(
        const QString& patientId, const QString& issuerOfPatientId) const
    {
        const QString cx = "dicom_conn_find_patient_" + QString::number(QRandomGenerator::global()->generate());
        {
            QSqlDatabase db = createConnection(cx);
            if (!db.open()) {
                const auto err = QString("Failed to open database: %1").arg(db.lastError().text());
                logger->LogError(err);
                return Result<std::optional<Patient>>::Failure(err);
            }

            QSqlQuery q(db);
            q.prepare(R"(
                SELECT id, patient_name, patient_id, issuer_of_patient_id, type_of_patient_id,
                       issuer_of_patient_id_qualifiers, other_patient_id, patient_sex,
                       patient_birth_date, patient_comments, patient_allergies,
                       requesting_physician, patient_address
                FROM patients
                WHERE patient_id = :patient_id
                  AND ((:issuer IS NULL AND issuer_of_patient_id IS NULL) OR issuer_of_patient_id = :issuer)
                LIMIT 1
            )");

            q.bindValue(":patient_id", patientId);
            q.bindValue(":issuer", issuerOfPatientId.isEmpty() ? QVariant(QVariant::String) : issuerOfPatientId);

            if (!q.exec()) {
                const auto err = QString("Failed to find patient: %1").arg(q.lastError().text());
                logger->LogError(err);
                return Result<std::optional<Patient>>::Failure(err);
            }

            if (q.next()) {
                Patient patient;
                patient.Id = q.value("id").toInt();
                patient.PatientName = q.value("patient_name").toString();
                patient.PatientId = q.value("patient_id").toString();
                patient.IssuerOfPatientId = q.value("issuer_of_patient_id").toString();
                patient.TypeOfPatientId = q.value("type_of_patient_id").toString();
                patient.IssuerOfPatientIdQualifiers = q.value("issuer_of_patient_id_qualifiers").toString();
                patient.OtherPatientId = q.value("other_patient_id").toString();
                patient.PatientSex = q.value("patient_sex").toString();
                patient.PatientBirthDate = q.value("patient_birth_date").toDate();
                patient.PatientComments = q.value("patient_comments").toString();
                patient.PatientAllergies = q.value("patient_allergies").toString();
                patient.RequestingPhysician = q.value("requesting_physician").toString();
                patient.PatientAddress = q.value("patient_address").toString();

                db.close();
                QSqlDatabase::removeDatabase(cx);
                return Result<std::optional<Patient>>::Success(patient);
            }

            db.close();
        }
        QSqlDatabase::removeDatabase(cx);
        return Result<std::optional<Patient>>::Success(std::nullopt);
    }

    Result<Patient> DicomRepository::upsertPatient(Patient& patient)
    {
        // First, try to find existing patient
        auto findResult = findPatientByIdAndIssuer(patient.PatientId, patient.IssuerOfPatientId);
        if (!findResult.IsSuccess()) {
            return Result<Patient>::Failure(findResult.GetErrorMessage());
        }

        auto existingPatient = findResult.GetValue();
        if (existingPatient.has_value()) {
            // Patient exists - update it
            patient.Id = existingPatient->Id;
            auto updateResult = updatePatient(patient);
            if (!updateResult.IsSuccess()) {
                return Result<Patient>::Failure(updateResult.GetErrorMessage());
            }
            return Result<Patient>::Success(patient);
        } else {
            // Patient doesn't exist - insert new
            return insertPatient(patient);
        }
    }
}

