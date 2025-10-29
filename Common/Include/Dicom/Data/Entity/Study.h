#ifndef STUDY_H
#define STUDY_H

/**
 * @file Study.h
 * @brief Declares the Study entity representing DICOM Study-level metadata.
 *
 * This header defines a lightweight value type representing a DICOM study
 * with reference to the associated patient. It is Qt meta-type enabled
 * for use in QVariant, queued signal/slots, and model/view code.
 */

#include <QString>
#include <QDate>
#include <QTime>
#include <QDateTime>
#include <QMetaType>

/// Top-level namespace for Etrek components.
/**
 * @namespace Etrek::Dicom::Data::Entity
 * @brief Data-layer DICOM entities (persistable domain objects).
 */
namespace Etrek::Dicom::Data::Entity {

    /**
     * @class Study
     * @brief Persistable entity representing a DICOM study.
     *
     * Stores study-level metadata and links to the associated patient.
     * The equality operator compares by primary key (`Id`) only.
     */
    class Study {
    public:
        /**
         * @brief Surrogate primary key.
         *
         * Defaults to `-1` to indicate an unsaved/transient instance.
         */
        int Id = -1;

        /**
         * @brief Foreign key reference to patients table.
         *
         * Links this study to a patient record. Defaults to `-1` (unset).
         */
        int PatientRefId = -1;

        /**
         * @brief Study Instance UID (0020,000D)
         *
         * Unique identifier for the study.
         */
        QString StudyInstanceUid;

        /**
         * @brief Study ID (0020,0010)
         *
         * User or equipment generated study identifier.
         */
        QString StudyId;

        /**
         * @brief Accession Number (0008,0050)
         *
         * Identifier of the imaging service request.
         */
        QString AccessionNumber;

        /**
         * @brief Issuer of Accession Number (0008,0051)
         *
         * Identifier of the organization that issued the accession number.
         */
        QString IssuerOfAccessionNumber;

        /**
         * @brief Referring Physician's Name (0008,0090)
         *
         * Name of the patient's referring physician.
         */
        QString ReferringPhysicianName;

        /**
         * @brief Study Date (0008,0020)
         *
         * Date the study started.
         */
        QDate StudyDate;

        /**
         * @brief Study Time (0008,0030)
         *
         * Time the study started.
         */
        QTime StudyTime;

        /**
         * @brief Study Description (0008,1030)
         *
         * Institution-generated description of the study.
         */
        QString StudyDescription;

        /**
         * @brief Patient's Age (0010,1010)
         *
         * Age of the patient at the time of the study.
         */
        int PatientAge = -1;

        /**
         * @brief Patient's Size (0010,1020)
         *
         * Patient's height in meters.
         */
        int PatientSize = -1;

        /**
         * @brief Allergies (0010,2110)
         *
         * Patient allergies at the time of study (legacy field, may be moved to patient).
         */
        QString Allergy;

        /**
         * @brief Default constructor.
         */
        Study() = default;

        /**
         * @brief Equality operator comparing by primary key.
         * @param other The other instance to compare against.
         * @return `true` if both objects have the same @ref Id; otherwise `false`.
         *
         * @note Payload fields are not compared.
         */
        bool operator==(const Study& other) const noexcept {
            return Id == other.Id;
        }

        /**
         * @brief Inequality operator.
         * @param other The other instance to compare against.
         * @return `true` if objects have different @ref Id; otherwise `false`.
         */
        bool operator!=(const Study& other) const noexcept {
            return !(*this == other);
        }
    };

} // namespace Etrek::Dicom::Data::Entity

/**
 * @brief Enables Study to be used with Qt's meta-object system.
 *
 * Allows use in QVariant, queued signal/slot connections, and model/view roles.
 */
Q_DECLARE_METATYPE(Etrek::Dicom::Data::Entity::Study)

#endif // STUDY_H
