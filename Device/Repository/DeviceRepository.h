#pragma once

#include <memory>
#include <QString>
#include <QVector>
#include <QMap>
#include <QSqlDatabase>

#include "Result.h"
#include "Device/DevicePosition.h"
#include "Device/Data/Entity/XRayTube.h"
#include "Device/Data/Entity/Generator.h"
#include "Device/Data/Entity/Detector.h"
#include "Device/Data/Entity/GeneralEquipment.h"
#include "Device/Data/Entity/Institution.h"
#include "Device/Data/Entity/EnvironmentSetting.h"
#include "Device/EnvironmentSettingUtils.h"



#include "DatabaseConnectionSetting.h"
#include "TranslationProvider.h"
#include "AppLogger.h"

namespace Etrek::Device::Repository
{

    class DeviceRepository
    {
    public:
        explicit DeviceRepository(std::shared_ptr<Etrek::Core::Data::Model::DatabaseConnectionSetting> connectionSetting,
                                 Etrek::Core::Globalization::TranslationProvider* tr = nullptr);

        // Generators
        Etrek::Specification::Result<Etrek::Device::Data::Entity::Generator>              getGeneratorById(int id) const;
        Etrek::Specification::Result<QVector<Etrek::Device::Data::Entity::Generator>>     getGeneratorList() const; // all generators
        Etrek::Specification::Result<Etrek::Device::Data::Entity::Generator>              updateGenerator(const Etrek::Device::Data::Entity::Generator& generator);

        // X-ray tubes
        Etrek::Specification::Result<Etrek::Device::Data::Entity::XRayTube>               getXRayTube(int tubeId) const;
        Etrek::Specification::Result<QVector<Etrek::Device::Data::Entity::XRayTube>>      getXRayTubesList() const;
        Etrek::Specification::Result<Etrek::Device::Data::Entity::XRayTube>               updateXRayTube(const Etrek::Device::Data::Entity::XRayTube& tube);

        // Detectors
        Etrek::Specification::Result<Etrek::Device::Data::Entity::Detector> updateDetector(const Etrek::Device::Data::Entity::Detector& detector);
        Etrek::Specification::Result<QVector<Etrek::Device::Data::Entity::Detector>> getDetectorList() const;
        Etrek::Specification::Result<Etrek::Device::Data::Entity::Detector> getDetectorById(int detectorId) const;

        // Institutions
        Etrek::Specification::Result<QVector<Etrek::Device::Data::Entity::Institution>> getInstitutionList() const;
        Etrek::Specification::Result<Etrek::Device::Data::Entity::Institution>          getInstitutionById(int id) const;
        Etrek::Specification::Result<Etrek::Device::Data::Entity::Institution>          createInstitution(const Etrek::Device::Data::Entity::Institution& inst);
        Etrek::Specification::Result<Etrek::Device::Data::Entity::Institution>          updateInstitution(const Etrek::Device::Data::Entity::Institution& inst);
        Etrek::Specification::Result<bool>                      deleteInstitution(int id);            // hard delete
        Etrek::Specification::Result<bool>                      deactivateInstitution(int id);        // soft delete (is_active = 0)

        // General equipment
        Etrek::Specification::Result<QVector<Etrek::Device::Data::Entity::GeneralEquipment>> getGeneralEquipmentList() const;
        Etrek::Specification::Result<Etrek::Device::Data::Entity::GeneralEquipment>          getGeneralEquipmentById(int id) const;
        Etrek::Specification::Result<Etrek::Device::Data::Entity::GeneralEquipment>          createGeneralEquipment(const Etrek::Device::Data::Entity::GeneralEquipment& ge);
        Etrek::Specification::Result<Etrek::Device::Data::Entity::GeneralEquipment>          updateGeneralEquipment(const Etrek::Device::Data::Entity::GeneralEquipment& ge);
        Etrek::Specification::Result<bool>                           deleteGeneralEquipment(int id);  // hard delete
        Etrek::Specification::Result<bool>                           deactivateGeneralEquipment(int id); // soft delete


        // Environment settings (single row)
        Etrek::Specification::Result<Etrek::Device::Data::Entity::EnvironmentSetting> getEnvironmentSettings() const;
        Etrek::Specification::Result<Etrek::Device::Data::Entity::EnvironmentSetting> updateEnvironmentSettings(
            const Etrek::Device::Data::Entity::EnvironmentSetting& s);


        ~DeviceRepository();

    private:
        QSqlDatabase createConnection(const QString& connectionName) const;

        //bool deactivateOtherActiveDetectors(const QString& detectorOrder, int excludeId, QSqlDatabase& db, QString& errorMessage) const;
        bool deactivateOtherActiveGenerators(int outputNumber, int excludeGeneratorId, QSqlDatabase& db, QString& errorMessage) const;

        std::shared_ptr<Etrek::Core::Data::Model::DatabaseConnectionSetting> m_connectionSetting;
        Etrek::Core::Globalization::TranslationProvider* translator; // non-owning
        std::shared_ptr<Etrek::Core::Log::AppLogger> logger;
    };
}
