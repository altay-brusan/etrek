#ifndef PATIENTMODEL_H
#define PATIENTMODEL_H

#include <QString>
#include <QDate>
#include <QList>
#include "AnatomicRegion.h"
#include "BodyPart.h"
#include "ScanProtocolUtil.h"

namespace Etrek::ScanProtocol::Data::Model {

    /**
     * @brief Represents a selected body part with its anatomic region
     */
    struct BodyPartSelection {
        Etrek::ScanProtocol::Data::Entity::AnatomicRegion region;
        Etrek::ScanProtocol::Data::Entity::BodyPart bodyPart;

        bool isValid() const {
            return region.Id >= 0 && bodyPart.Id >= 0;
        }
    };

    /**
     * @brief Patient data model for new patient registration
     *
     * Contains patient demographics and selected body parts for imaging.
     * Each body part will typically result in a separate DICOM study.
     */
    class PatientModel {
    public:
        QString firstName;
        QString middleName;
        QString lastName;
        QString patientId;
        QDate   dateOfBirth;
        int     age = 0;
        Etrek::ScanProtocol::Gender gender = Etrek::ScanProtocol::Gender::Male;
        QString referringPhysician;
        QString patientLocation;
        QString admissionNumber;
        QString accessionNumber;

        // List of selected body parts (each may result in a separate study)
        QList<BodyPartSelection> selectedBodyParts;

        bool isValid() const {
            return !firstName.isEmpty() &&
                   !lastName.isEmpty() &&
                   !patientId.isEmpty() &&
                   dateOfBirth.isValid() &&
                   !selectedBodyParts.isEmpty();
        }
    };

} // namespace Etrek::ScanProtocol::Data::Model

#endif // PATIENTMODEL_H

