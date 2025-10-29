#ifndef PATIENT_H
#define PATIENT_H

/**
 * @file Patient.h
 * @brief Declares the Patient entity representing DICOM Patient Module data.
 *
 * This header defines a lightweight value type representing patient demographics
 * and identification as per the DICOM Patient Module. It is Qt meta-type enabled
 * for use in QVariant, queued signal/slots, and model/view code.
 */

#include <QString>
#include <QDate>
#include <QDateTime>
#include <QMetaType>

/// Top-level namespace for Etrek components.
/**
 * @namespace Etrek::Dicom::Data::Entity
 * @brief Data-layer DICOM entities (persistable domain objects).
 */
namespace Etrek::Dicom::Data::Entity {

    /**
     * @class Patient
     * @brief Persistable entity representing a patient in the DICOM Patient Module.
     *
     * Stores patient demographics and identification information aligned with
     * DICOM Patient Module attributes. The equality operator compares by
     * primary key (`Id`) only.
     */
    class Patient {
    public:
        /**
         * @brief Surrogate primary key.
         *
         * Defaults to `-1` to indicate an unsaved/transient instance.
         */
        int Id = -1;

        /**
         * @brief Patient's Name (0010,0010)
         *
         * Full name of the patient.
         */
        QString PatientName;

        /**
         * @brief Patient ID (0010,0020)
         *
         * Primary identifier for the patient assigned by an issuer.
         */
        QString PatientId;

        /**
         * @brief Issuer of Patient ID (0010,0021)
         *
         * Identifier of the organization that issued the Patient ID.
         */
        QString IssuerOfPatientId;

        /**
         * @brief Type of Patient ID (0010,0022)
         *
         * Defined Terms: TEXT, RFID, BARCODE
         */
        QString TypeOfPatientId;

        /**
         * @brief Issuer of Patient ID Qualifiers Sequence (0010,0024)
         *
         * Stored as JSON string representing the sequence.
         */
        QString IssuerOfPatientIdQualifiers;

        /**
         * @brief Other Patient IDs (0010,1000)
         *
         * Stored as JSON array of alternative patient identifiers.
         */
        QString OtherPatientId;

        /**
         * @brief Patient's Sex (0010,0040)
         *
         * Defined Terms: M (Male), F (Female), O (Other), U (Unknown)
         */
        QString PatientSex;

        /**
         * @brief Patient's Birth Date (0010,0030)
         *
         * Date of birth in DICOM DA format, stored as QDate.
         */
        QDate PatientBirthDate;

        /**
         * @brief Patient Comments (0010,4000)
         *
         * Free-text comments about the patient.
         */
        QString PatientComments;

        /**
         * @brief Allergies (0010,2110)
         *
         * Patient allergies and contraindications.
         */
        QString PatientAllergies;

        /**
         * @brief Requesting Physician (0032,1032)
         *
         * Name of the physician requesting the procedure.
         */
        QString RequestingPhysician;

        /**
         * @brief Patient's Address (0010,1040)
         *
         * Mailing address of the patient.
         */
        QString PatientAddress;

        /**
         * @brief Timestamp for when the entity was created.
         */
        QDateTime CreateDate;

        /**
         * @brief Timestamp for the last modification.
         */
        QDateTime UpdateDate;

        /**
         * @brief Default constructor.
         */
        Patient() = default;

        /**
         * @brief Equality operator comparing by primary key.
         * @param other The other instance to compare against.
         * @return `true` if both objects have the same @ref Id; otherwise `false`.
         *
         * @note Payload fields are not compared.
         */
        bool operator==(const Patient& other) const noexcept {
            return Id == other.Id;
        }

        /**
         * @brief Inequality operator.
         * @param other The other instance to compare against.
         * @return `true` if objects have different @ref Id; otherwise `false`.
         */
        bool operator!=(const Patient& other) const noexcept {
            return !(*this == other);
        }
    };

} // namespace Etrek::Dicom::Data::Entity

/**
 * @brief Enables Patient to be used with Qt's meta-object system.
 *
 * Allows use in QVariant, queued signal/slot connections, and model/view roles.
 */
Q_DECLARE_METATYPE(Etrek::Dicom::Data::Entity::Patient)

#endif // PATIENT_H
