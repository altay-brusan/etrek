#ifndef PATIENTMODEL_H
#define PATIENTMODEL_H

#include <QString>
#include <QDate>
#include "AnatomicRegion.h"
#include "BodyPart.h"
#include "ScanProtocolUtil.h"

namespace Etrek::ScanProtocol::Data::Model {

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
        Etrek::ScanProtocol::Data::Entity::AnatomicRegion selectedRegion;
        Etrek::ScanProtocol::Data::Entity::BodyPart      selectedBodyPart;

        bool isValid() const {
            return !firstName.isEmpty() &&
                   !lastName.isEmpty() &&
                   !patientId.isEmpty() &&
                   dateOfBirth.isValid();
        }
    };

} // namespace Etrek::ScanProtocol::Data::Model

#endif // PATIENTMODEL_H

