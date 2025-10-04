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

using namespace Etrek::Device;
using namespace Etrek::Core::Data::Model;
using namespace Etrek::Device::Data::Entity;
using namespace Etrek::Core::Data;
using namespace Etrek::Core::Log;

namespace Etrek::Device::Repository
{
    using Etrek::Specification::Result;

    class DeviceRepository
    {
    public:
        explicit DeviceRepository(std::shared_ptr<DatabaseConnectionSetting> connectionSetting,
            TranslationProvider* tr = nullptr);

        // Generators
        Result<Generator>              getGeneratorById(int id) const;
        Result<QVector<Generator>>     getGeneratorList() const; // all generators
        Result<Generator>              updateGenerator(const Generator& generator);

        // X-ray tubes
        Result<XRayTube>               getXRayTube(int tubeId) const;
        Result<QVector<XRayTube>>      getXRayTubesList() const;
        Result<XRayTube>               updateXRayTube(const XRayTube& tube);
                
        // Detectors
        Result<Detector> updateDetector(const Detector& detector);
        Result<QVector<Detector>> getDetectorList() const;
        Result<Detector> getDetectorById(int detectorId) const;

        // Institutions
        Result<QVector<Institution>> getInstitutionList() const;
        Result<Institution>          getInstitutionById(int id) const;
        Result<Institution>          createInstitution(const Institution& inst);
        Result<Institution>          updateInstitution(const Institution& inst);
        Result<bool>                 deleteInstitution(int id);            // hard delete
        Result<bool>                 deactivateInstitution(int id);        // soft delete (is_active = 0)

        // General equipment
        Result<QVector<GeneralEquipment>> getGeneralEquipmentList() const;
        Result<GeneralEquipment>          getGeneralEquipmentById(int id) const;
        Result<GeneralEquipment>          createGeneralEquipment(const GeneralEquipment& ge);
        Result<GeneralEquipment>          updateGeneralEquipment(const GeneralEquipment& ge);
        Result<bool>                      deleteGeneralEquipment(int id);  // hard delete
        Result<bool>                      deactivateGeneralEquipment(int id); // soft delete


        // Environment settings (single row)
        Result<EnvironmentSetting> getEnvironmentSettings() const;
        Result<EnvironmentSetting> updateEnvironmentSettings(
            const EnvironmentSetting& s);


        ~DeviceRepository();

    private:
        QSqlDatabase createConnection(const QString& connectionName) const;

        //bool deactivateOtherActiveDetectors(const QString& detectorOrder, int excludeId, QSqlDatabase& db, QString& errorMessage) const;
        bool deactivateOtherActiveGenerators(int outputNumber, int excludeGeneratorId, QSqlDatabase& db, QString& errorMessage) const;

        std::shared_ptr<DatabaseConnectionSetting> m_connectionSetting;
        TranslationProvider* translator;
        std::shared_ptr<AppLogger>                  logger;
    };
}
