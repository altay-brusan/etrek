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
    namespace spc = Etrek::Specification;
    namespace ent = Etrek::Device::Data::Entity;
    namespace mdl = Etrek::Core::Data::Model;
    namespace glob = Etrek::Core::Globalization;
    namespace lg = Etrek::Core::Log;

    class DeviceRepository
    {
    public:
        explicit DeviceRepository(std::shared_ptr<mdl::DatabaseConnectionSetting> connectionSetting,
                                 glob::TranslationProvider* tr = nullptr);

        // Generators
        spc::Result<ent::Generator>              getGeneratorById(int id) const;
        spc::Result<QVector<ent::Generator>>     getGeneratorList() const; // all generators
        spc::Result<ent::Generator>              updateGenerator(const ent::Generator& generator);

        // X-ray tubes
        spc::Result<ent::XRayTube>               getXRayTube(int tubeId) const;
        spc::Result<QVector<ent::XRayTube>>      getXRayTubesList() const;
        spc::Result<ent::XRayTube>               updateXRayTube(const ent::XRayTube& tube);

        // Detectors
        spc::Result<ent::Detector> updateDetector(const ent::Detector& detector);
        spc::Result<QVector<ent::Detector>> getDetectorList() const;
        spc::Result<ent::Detector> getDetectorById(int detectorId) const;

        // Institutions
        spc::Result<QVector<ent::Institution>> getInstitutionList() const;
        spc::Result<ent::Institution>          getInstitutionById(int id) const;
        spc::Result<ent::Institution>          createInstitution(const ent::Institution& inst);
        spc::Result<ent::Institution>          updateInstitution(const ent::Institution& inst);
        spc::Result<bool>                      deleteInstitution(int id);            // hard delete
        spc::Result<bool>                      deactivateInstitution(int id);        // soft delete (is_active = 0)

        // General equipment
        spc::Result<QVector<ent::GeneralEquipment>> getGeneralEquipmentList() const;
        spc::Result<ent::GeneralEquipment>          getGeneralEquipmentById(int id) const;
        spc::Result<ent::GeneralEquipment>          createGeneralEquipment(const ent::GeneralEquipment& ge);
        spc::Result<ent::GeneralEquipment>          updateGeneralEquipment(const ent::GeneralEquipment& ge);
        spc::Result<bool>                           deleteGeneralEquipment(int id);  // hard delete
        spc::Result<bool>                           deactivateGeneralEquipment(int id); // soft delete


        // Environment settings (single row)
        spc::Result<ent::EnvironmentSetting> getEnvironmentSettings() const;
        spc::Result<ent::EnvironmentSetting> updateEnvironmentSettings(
            const ent::EnvironmentSetting& s);


        ~DeviceRepository();

    private:
        QSqlDatabase createConnection(const QString& connectionName) const;

        //bool deactivateOtherActiveDetectors(const QString& detectorOrder, int excludeId, QSqlDatabase& db, QString& errorMessage) const;
        bool deactivateOtherActiveGenerators(int outputNumber, int excludeGeneratorId, QSqlDatabase& db, QString& errorMessage) const;

        std::shared_ptr<mdl::DatabaseConnectionSetting> m_connectionSetting;
        glob::TranslationProvider* translator; // non-owning
        std::shared_ptr<lg::AppLogger> logger;
    };
}
