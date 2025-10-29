#include "DicomRepository.h"
#include <QSqlQuery>
#include <QSqlError>
#include <QVariant>
#include <QRandomGenerator>
#include "AppLoggerFactory.h"

namespace Etrek::Dicom::Repository {

    using Etrek::Specification::Result;
    using Etrek::Core::Data::Entity::Study;
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
                SELECT id, study_instance_uid, study_id, admission_id, accession_number,
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
}

