#ifndef ETREK_DICOM_DATA_ENTITY_PATIENT_H
#define ETREK_DICOM_DATA_ENTITY_PATIENT_H

#include <QString>
#include <QDate>
#include <QDateTime>

namespace Etrek::Dicom::Data::Entity {

    /**
     * @brief Represents a DICOM Patient entity aligned with DICOM Patient Module (PS3.3 C.7.1.1)
     *
     * This class maps to the patients database table and contains patient-level attributes
     * from the DICOM Patient Module. Multi-valued attributes (issuer_of_patient_id_qualifiers,
     * other_patient_id) are stored as JSON strings in the database.
     */
    class Patient {
    public:
        int Id = -1;
        QString PatientName;                      // (0010,0010): Patient's Name
        QString PatientId;                        // (0010,0020): Primary identifier for the patient
        QString IssuerOfPatientId;                // (0010,0021): Organization that issued the Patient ID
        QString TypeOfPatientId;                  // (0010,0022): Type of identifier (e.g., text, barcode, RFID)
        QString IssuerOfPatientIdQualifiers;      // (0010,0024): JSON - Sequence of issuer qualifiers
        QString OtherPatientId;                   // (0010,1000): JSON - Array of other patient identifiers
        QString PatientSex;                       // (0010,0040): Patient's sex (M, F, O, U)
        QDate PatientBirthDate;                   // (0010,0030): Patient's date of birth
        QString PatientComments;                  // (0010,4000): Additional patient information
        QString PatientAllergies;                 // (0010,2110): Patient allergies or adverse reactions
        QString RequestingPhysician;              // (0032,1032): Requesting/referring physician
        QString PatientAddress;                   // (0010,1040): Patient's mailing address

        /**
         * @brief Default constructor
         */
        Patient() = default;

        /**
         * @brief Checks if this patient entity is valid (has a valid database ID)
         * @return true if the patient has been persisted to the database (Id >= 0)
         */
        bool IsValid() const { return Id >= 0; }
    };

} // namespace Etrek::Dicom::Data::Entity

#endif // ETREK_DICOM_DATA_ENTITY_PATIENT_H
