#include <QSqlQuery>
#include <QSqlError>
#include <QVariant>
#include <QSet>
#include <QRandomGenerator>
#include "DeviceRepository.h"
#include "AppLoggerFactory.h"
#include "MessageKey.h"
#include "DetectorUtils.h"


using namespace Etrek::Device::Repository;

static inline QString kRepoName() { return "DeviceRepository"; }

DeviceRepository::DeviceRepository(std::shared_ptr<DatabaseConnectionSetting> connectionSetting,
    TranslationProvider* tr)
    : m_connectionSetting(std::move(connectionSetting)),
    translator(tr ? tr : &TranslationProvider::Instance())
{
    AppLoggerFactory factory(LoggerProvider::Instance(), translator);
    logger = factory.CreateLogger(kRepoName());
}

DeviceRepository::~DeviceRepository() = default;

QSqlDatabase DeviceRepository::createConnection(const QString& connectionName) const
{
    QSqlDatabase db = QSqlDatabase::addDatabase("QMYSQL", connectionName);
    db.setHostName(m_connectionSetting->getHostName());
    db.setDatabaseName(m_connectionSetting->getDatabaseName());
    db.setUserName(m_connectionSetting->getEtrekUserName());
    db.setPassword(m_connectionSetting->getPassword());
    db.setPort(m_connectionSetting->getPort());
    return db;
}

// -------- generators --------

Etrek::Specification::Result<QVector<Generator>> DeviceRepository::getGeneratorList() const
{
    QVector<Generator> generators;
    const QString connectionName = "dev_conn_gen_list_" + QString::number(QRandomGenerator::global()->generate());

    {
        // Scoped connection
        QSqlDatabase db = createConnection(connectionName);
        if (!db.open()) {
            const auto err = translator->getErrorMessage(FAILED_TO_OPEN_DB_MSG)
                .arg(db.lastError().text());
            logger->LogError(err);
            return Etrek::Specification::Result<QVector<Generator>>::Failure(err);
        }

        // Query: load all Generator fields
        QSqlQuery query(db);
        query.prepare(R"(
            SELECT 
                id,
                manufacturer,
                model_number,
                part_number,
                serial_number,
                type_number,
                technical_specifications,
                manufacture_date,
                installation_date,
                calibration_date,
                output1,
                output2,
                is_active,
                is_output1_active,
                is_output2_active,
                create_date,
                update_date
            FROM generators
            ORDER BY id
        )");

        if (!query.exec()) {
            const auto err = translator->getCriticalMessage(QUERY_FAILED_ERROR_MSG)
                .arg(query.lastError().text());
            logger->LogError(err);
            return Etrek::Specification::Result<QVector<Generator>>::Failure(err);
        }

        // Iterate query results
        while (query.next()) {
            Generator gen;

            gen.Id = query.value("id").toInt();
            gen.Manufacturer = query.value("manufacturer").toString();
            gen.ModelNumber = query.value("model_number").toString();
            gen.PartNumber = query.value("part_number").toString();
            gen.SerialNumber = query.value("serial_number").toString();
            gen.TypeNumber = query.value("type_number").toString();
            gen.TechnicalSpecifications = query.value("technical_specifications").toString();
            gen.ManufactureDate = query.value("manufacture_date").toDate();
            gen.InstallationDate = query.value("installation_date").toDate();
            gen.CalibrationDate = query.value("calibration_date").toDate();

            gen.Output1 = query.value("output1").toInt();
            gen.Output2 = query.value("output2").toInt();

            gen.IsActive = query.value("is_active").toBool();
            gen.IsOutput1Active = query.value("is_output1_active").toBool();
            gen.IsOutput2Active = query.value("is_output2_active").toBool();

            gen.CreateDate = query.value("create_date").toDateTime();
            gen.UpdateDate = query.value("update_date").toDateTime();

            generators.append(gen);
        }
    } // <--- Scoped connection ends here, db will be destroyed

    QSqlDatabase::removeDatabase(connectionName);
    return Etrek::Specification::Result<QVector<Generator>>::Success(generators);
}

Etrek::Specification::Result<Generator> DeviceRepository::getGeneratorById(int id) const
{
    const QString connectionName = QString("dev_conn_gen_%1").arg(id);
    QSqlDatabase db = createConnection(connectionName);
    if (!db.open()) {
        const auto err = translator->getErrorMessage(FAILED_TO_OPEN_DB_MSG)
            .arg(db.lastError().text());
        logger->LogError(err);
        return Etrek::Specification::Result<Generator>::Failure(err);
    }

    QSqlQuery query(db);
    query.prepare(R"(
        SELECT 
            id,
            manufacturer,
            model_number,
            part_number,
            serial_number,
            type_number,
            technical_specifications,
            manufacture_date,
            installation_date,
            calibration_date,
            output1,
            output2,
            is_active,
            is_output1_active,
            is_output2_active,
            create_date,
            update_date
        FROM generators
        WHERE id = :id
        LIMIT 1
    )");

    query.bindValue(":id", id);

    if (!query.exec()) {
        const auto err = translator->getCriticalMessage(QUERY_FAILED_ERROR_MSG)
            .arg(query.lastError().text());
        logger->LogError(err);
        QSqlDatabase::removeDatabase(connectionName);
        return Etrek::Specification::Result<Generator>::Failure(err);
    }

    if (!query.next()) {
        QSqlDatabase::removeDatabase(connectionName);
        return Etrek::Specification::Result<Generator>::Failure(QString("Generator with id %1 not found").arg(id));
    }

    Generator gen;
    gen.Id = query.value("id").toInt();
    gen.Manufacturer = query.value("manufacturer").toString();
    gen.ModelNumber = query.value("model_number").toString();
    gen.PartNumber = query.value("part_number").toString();
    gen.SerialNumber = query.value("serial_number").toString();
    gen.TypeNumber = query.value("type_number").toString();
    gen.TechnicalSpecifications = query.value("technical_specifications").toString();
    gen.ManufactureDate = query.value("manufacture_date").toDate();
    gen.InstallationDate = query.value("installation_date").toDate();
    gen.CalibrationDate = query.value("calibration_date").toDate();

    gen.Output1 = query.value("output1").toInt();
    gen.Output2 = query.value("output2").toInt();

    gen.IsActive = query.value("is_active").toBool();
    gen.IsOutput1Active = query.value("is_output1_active").toBool();
    gen.IsOutput2Active = query.value("is_output2_active").toBool();

    gen.CreateDate = query.value("create_date").toDateTime();
    gen.UpdateDate = query.value("update_date").toDateTime();

    QSqlDatabase::removeDatabase(connectionName);
    return Etrek::Specification::Result<Generator>::Success(gen);
}

Etrek::Specification::Result<Generator> DeviceRepository::updateGenerator(const Generator& generator)
{
    if (generator.Id == -1) {
        return Etrek::Specification::Result<Generator>::Failure("Cannot update generator: invalid Id (-1).");
    }

    const QString connectionName = QString("dev_conn_update_gen_%1").arg(generator.Id);
    {
        QSqlDatabase db = createConnection(connectionName);
        if (!db.open()) {
            const auto err = translator->getErrorMessage(FAILED_TO_OPEN_DB_MSG)
                .arg(db.lastError().text());
            logger->LogError(err);
            return Etrek::Specification::Result<Generator>::Failure(err);
        }

        // === Precondition: deactivate other active generators if needed ===
        QString preconditionError;
        if (generator.IsOutput1Active && generator.Output1 != -1) {
            if (!deactivateOtherActiveGenerators(1, generator.Id, db, preconditionError))
                return Etrek::Specification::Result<Generator>::Failure(preconditionError);
        }

        if (generator.IsOutput2Active && generator.Output2 != -1) {
            if (!deactivateOtherActiveGenerators(2, generator.Id, db, preconditionError))
                return Etrek::Specification::Result<Generator>::Failure(preconditionError);
        }

        // === Update the generator itself ===
        QSqlQuery query(db);
        query.prepare(R"(
        UPDATE generators SET
            category = :category,
            manufacturer = :manufacturer,
            model_number = :model_number,
            part_number = :part_number,
            serial_number = :serial_number,
            type_number = :type_number,
            technical_specifications = :technical_specifications,
            manufacture_date = :manufacture_date,
            installation_date = :installation_date,
            calibration_date = :calibration_date,
            output1 = :output1,
            output2 = :output2,
            is_active = :is_active,
            is_output1_active = :is_output1_active,
            is_output2_active = :is_output2_active,
            update_date = :update_date
        WHERE id = :id
        )");

        query.bindValue(":manufacturer", generator.Manufacturer);
        query.bindValue(":model_number", generator.ModelNumber);
        query.bindValue(":part_number", generator.PartNumber);
        query.bindValue(":serial_number", generator.SerialNumber);
        query.bindValue(":type_number", generator.TypeNumber);
        query.bindValue(":technical_specifications", generator.TechnicalSpecifications);
        query.bindValue(":manufacture_date", generator.ManufactureDate);
        query.bindValue(":installation_date", generator.InstallationDate);
        query.bindValue(":calibration_date", generator.CalibrationDate);
        query.bindValue(":output1", generator.Output1);
        query.bindValue(":output2", generator.Output2);
        query.bindValue(":is_active", generator.IsActive);
        query.bindValue(":is_output1_active", generator.IsOutput1Active);
        query.bindValue(":is_output2_active", generator.IsOutput2Active);
        query.bindValue(":update_date", QDateTime::currentDateTime());
        query.bindValue(":id", generator.Id);

        if (!query.exec()) {
            const auto err = translator->getCriticalMessage(QUERY_FAILED_ERROR_MSG)
                .arg(query.lastError().text());
            logger->LogError(err);
            QSqlDatabase::removeDatabase(connectionName);
            return Etrek::Specification::Result<Generator>::Failure(err);
        }
    }
    QSqlDatabase::removeDatabase(connectionName);
    return Etrek::Specification::Result<Generator>::Success(generator);
}

bool DeviceRepository::deactivateOtherActiveGenerators(int outputNumber, int excludeGeneratorId, QSqlDatabase& db, QString& errorMessage) const
{
    if (outputNumber != 1 && outputNumber != 2) {
        errorMessage = "Invalid output number for deactivation.";
        return false;
    }

    QSqlQuery query(db);
    QString field = (outputNumber == 1) ? "is_output1_active" : "is_output2_active";

    query.prepare(QString("UPDATE generators SET %1 = 0 WHERE id != ? AND %2 != 0").arg(field, field));
    query.addBindValue(excludeGeneratorId);

    if (!query.exec()) {
        errorMessage = QString("Failed to deactivate other generators on output%1: %2").arg(outputNumber).arg(query.lastError().text());
        logger->LogError(errorMessage);
        return false;
    }

    return true;
}


// -------- tubes --------

Etrek::Specification::Result<XRayTube> DeviceRepository::getXRayTube(int tubeId) const
{
    const QString connectionName = "dev_conn_xray_single_" + QString::number(QRandomGenerator::global()->generate());

    XRayTube tube;

    {
        // Scoped connection
        QSqlDatabase db = createConnection(connectionName);
        if (!db.open()) {
            const auto err = translator->getErrorMessage(FAILED_TO_OPEN_DB_MSG)
                .arg(db.lastError().text());
            logger->LogError(err);
            return Etrek::Specification::Result<XRayTube>::Failure(err);
        }

        QSqlQuery query(db);
        query.prepare(R"(
            SELECT id,
                   manufacturer,
                   model_number,
                   part_number,
                   serial_number,
                   type_number,
                   focal_spot,
                   anode_heat_capacity,
                   cooling_rate,
                   max_voltage,
                   max_current,
                   power_range,
                   tube_filter,
                   is_active,
                   manufacture_date,
                   installation_date,
                   create_date,
                   update_date
            FROM xray_tubes
            WHERE id = :tubeId
            LIMIT 1
        )");
        query.bindValue(":tubeId", tubeId);

        if (!query.exec()) {
            const auto err = translator->getCriticalMessage(QUERY_FAILED_ERROR_MSG)
                .arg(query.lastError().text());
            logger->LogError(err);
            return Etrek::Specification::Result<XRayTube>::Failure(err);
        }

        if (!query.next()) {
            const auto err = QString("XRayTube with id %1 not found").arg(tubeId);
            return Etrek::Specification::Result<XRayTube>::Failure(err);
        }

        tube.Id = query.value("id").toInt();
        tube.Manufacturer = query.value("manufacturer").toString();
        tube.ModelNumber = query.value("model_number").toString();
        tube.PartNumber = query.value("part_number").toString();
        tube.SerialNumber = query.value("serial_number").toString();
        tube.TypeNumber = query.value("type_number").toString();
        tube.FocalSpot = query.value("focal_spot").toString();
        tube.AnodeHeatCapacity = query.value("anode_heat_capacity").toInt();
        tube.CoolingRate = query.value("cooling_rate").toInt();
        tube.MaxVoltage = query.value("max_voltage").toInt();
        tube.MaxCurrent = query.value("max_current").toInt();
        tube.PowerRange = query.value("power_range").toString();

        // Use helper functions
        tube.TubeFilter = query.value("tube_filter").toString();

        tube.IsActive = query.value("is_active").toBool();
        tube.ManufactureDate = query.value("manufacture_date").toDate();
        tube.InstallationDate = query.value("installation_date").toDate();
        tube.CreateDate = query.value("create_date").toDateTime();
        tube.UpdateDate = query.value("update_date").toDateTime();
    }

    QSqlDatabase::removeDatabase(connectionName);
    return Etrek::Specification::Result<XRayTube>::Success(tube);
}

Etrek::Specification::Result<QVector<XRayTube>> DeviceRepository::getXRayTubesList() const
{
    QVector<XRayTube> tubes;
    const QString connectionName = "dev_conn_xray_list_" + QString::number(QRandomGenerator::global()->generate());

    {
        // Scoped DB connection
        QSqlDatabase db = createConnection(connectionName);
        if (!db.open()) {
            const auto err = translator->getErrorMessage(FAILED_TO_OPEN_DB_MSG)
                .arg(db.lastError().text());
            logger->LogError(err);
            return Etrek::Specification::Result<QVector<XRayTube>>::Failure(err);
        }

        QSqlQuery query(db);
        query.prepare(R"(
                   SELECT id,
                   manufacturer,
                   model_number,
                   part_number,
                   serial_number,
                   type_number,
                   focal_spot,
                   anode_heat_capacity,
                   cooling_rate,
                   max_voltage,
                   max_current,
                   power_range,
                   tube_filter,
                   is_active,
                   manufacture_date,
                   installation_date,
                   create_date,
                   update_date
            FROM xray_tubes
            ORDER BY manufacturer, model_number
        )");

        if (!query.exec()) {
            QString err = translator->getCriticalMessage(QUERY_FAILED_ERROR_MSG)
                .arg(query.lastError().text());
            logger->LogError(err);
            return Etrek::Specification::Result<QVector<XRayTube>>::Failure(err);
        }

        while (query.next()) {
            XRayTube tube;

            tube.Id = query.value("id").toInt();
            tube.Manufacturer = query.value("manufacturer").toString();
            tube.ModelNumber = query.value("model_number").toString();
            tube.PartNumber = query.value("part_number").toString();
            tube.SerialNumber = query.value("serial_number").toString();
            tube.TypeNumber = query.value("type_number").toString();
            tube.FocalSpot = query.value("focal_spot").toString();
            tube.AnodeHeatCapacity = query.value("anode_heat_capacity").toInt();
            tube.CoolingRate = query.value("cooling_rate").toInt();
            tube.MaxVoltage = query.value("max_voltage").toInt();
            tube.MaxCurrent = query.value("max_current").toInt();
            tube.PowerRange = query.value("power_range").toString();

            tube.Position = ParseDevicePosition(query.value("position").toString());
            tube.TubeFilter = query.value("tube_filter").toString();

            tube.IsActive = query.value("is_active").toBool();
            tube.ManufactureDate = query.value("manufacture_date").toDate();
            tube.InstallationDate = query.value("installation_date").toDate();
            tube.CreateDate = query.value("create_date").toDateTime();
            tube.UpdateDate = query.value("update_date").toDateTime();

            tubes.append(tube);
        }
    } // Scoped connection ends here

    QSqlDatabase::removeDatabase(connectionName);
    return Etrek::Specification::Result<QVector<XRayTube>>::Success(tubes);
}

Etrek::Specification::Result<XRayTube> DeviceRepository::updateXRayTube(const XRayTube& tube)
{
    const QString connectionName = "dev_conn_update_xray_" + QString::number(QRandomGenerator::global()->generate());

    {
        QSqlDatabase db = createConnection(connectionName);
        if (!db.open()) {
            const auto err = translator->getErrorMessage(FAILED_TO_OPEN_DB_MSG)
                .arg(db.lastError().text());
            logger->LogError(err);
            return Etrek::Specification::Result<XRayTube>::Failure(err);
        }

        QSqlQuery query(db);
        query.prepare(R"(
            UPDATE xray_tubes
            SET
                manufacturer = :manufacturer,
                model_number = :modelNumber,
                part_number = :partNumber,
                serial_number = :serialNumber,
                type_number = :typeNumber,
                focal_spot = :focalSpot,
                anode_heat_capacity = :anodeHeatCapacity,
                cooling_rate = :coolingRate,
                max_voltage = :maxVoltage,
                max_current = :maxCurrent,
                power_range = :powerRange,
                position = :position,
                tube_filter = :tubeFilter,
                is_active = :isActive,
                manufacture_date = :manufactureDate,
                installation_date = :installationDate,
                calibration_date = :calibrationDate,
                update_date = CURRENT_TIMESTAMP
            WHERE id = :id
        )");

        // Bind values
        query.bindValue(":id", tube.Id);
        query.bindValue(":manufacturer", tube.Manufacturer);
        query.bindValue(":modelNumber", tube.ModelNumber);
        query.bindValue(":partNumber", tube.PartNumber);
        query.bindValue(":serialNumber", tube.SerialNumber);
        query.bindValue(":typeNumber", tube.TypeNumber);
        query.bindValue(":focalSpot", tube.FocalSpot);
        query.bindValue(":anodeHeatCapacity", tube.AnodeHeatCapacity);
        query.bindValue(":coolingRate", tube.CoolingRate);
        query.bindValue(":maxVoltage", tube.MaxVoltage);
        query.bindValue(":maxCurrent", tube.MaxCurrent);
        query.bindValue(":powerRange", tube.PowerRange);
        query.bindValue(":tubeName", tube.TubeName);
        query.bindValue(":tubeFilter", tube.TubeFilter);

        // Use helper functions for enum fields
        query.bindValue(":position", DevicePositionToString(tube.Position));

        query.bindValue(":isActive", tube.IsActive);
        query.bindValue(":manufactureDate", tube.ManufactureDate.isValid() ? tube.ManufactureDate : QVariant(QVariant::Date));
        query.bindValue(":installationDate", tube.InstallationDate.isValid() ? tube.InstallationDate : QVariant(QVariant::Date));
        query.bindValue(":calibrationDate", tube.CalibrationDate.isValid() ? tube.CalibrationDate : QVariant(QVariant::Date));

        if (!query.exec()) {
            const auto err = translator->getCriticalMessage(QUERY_FAILED_ERROR_MSG)
                .arg(query.lastError().text());
            logger->LogError(err);
            return Etrek::Specification::Result<XRayTube>::Failure(err);
        }
    } // db destroyed here

    return getXRayTube(tube.Id);
}


// -------- detector --------

Etrek::Specification::Result<QVector<Detector>> DeviceRepository::getDetectorList() const
{
    QVector<Detector> detectors;

    const QString connectionName = "detector_list_" + QString::number(QRandomGenerator::global()->generate());
    {
        QSqlDatabase db = createConnection(connectionName);

        if (!db.open()) {
            const auto err = translator->getErrorMessage(FAILED_TO_OPEN_DB_MSG)
                .arg(db.lastError().text());
            logger->LogError(err);
            return Etrek::Specification::Result<QVector<Detector>>::Failure(err);
        }

        QSqlQuery query(db);
        query.prepare(R"(
        SELECT id,
               manufacturer,
               model_name,
               resolution,
               size,
               pixel_width,
               pixel_height,
               sensitivity,
               imager_pixel_spacing,
               identifier,
               space_lut_reference,
               spatial_resolution,
               scan_options,
               width,
               height,
               width_shift,
               height_shift,
               saturation_value,
               is_active,
               create_date,
               update_date
        FROM detectors
        ORDER BY manufacturer, model_name
    )");

        if (!query.exec()) {
            const auto err = translator->getCriticalMessage(QUERY_FAILED_ERROR_MSG)
                .arg(query.lastError().text());
            logger->LogError(err);
            QSqlDatabase::removeDatabase(connectionName);
            return Etrek::Specification::Result<QVector<Etrek::Device::Data::Entity::Detector>>::Failure(err);
        }

        while (query.next()) {
            Detector d;

            d.Id = query.value("id").toInt();
            d.Manufacturer = query.value("manufacturer").toString();
            d.ModelName = query.value("model_name").toString();
            d.Resolution = query.value("resolution").toString();
            d.Size = query.value("size").toString();
            d.PixelWidth = query.value("pixel_width").toDouble();
            d.PixelHeight = query.value("pixel_height").toDouble();
            d.Sensitivity = query.value("sensitivity").toDouble();
            d.ImagerPixelSpacing = query.value("imager_pixel_spacing").toString();
            d.Identifier = query.value("identifier").toString();
            d.SpaceLUTReference = query.value("space_lut_reference").toString();
            d.SpatialResolution = query.value("spatial_resolution").toDouble();
            d.ScanOptions = query.value("scan_options").toString();
            d.Width = query.value("width").toInt();
            d.Height = query.value("height").toInt();
            d.WidthShift = query.value("width_shift").toInt();
            d.HeightShift = query.value("height_shift").toInt();
            d.SaturationValue = query.value("saturation_value").toInt();

            d.IsActive = query.value("is_active").toBool();
            d.CreateDate = query.value("create_date").toDateTime();
            d.UpdateDate = query.value("update_date").toDateTime();

            detectors.append(d);
        }

    }
    
    QSqlDatabase::removeDatabase(connectionName);
    return Etrek::Specification::Result<QVector<Detector>>::Success(detectors);
}

Etrek::Specification::Result<Detector> DeviceRepository::getDetectorById(int detectorId) const
{
    const QString connectionName = "detector_by_id_" + QString::number(QRandomGenerator::global()->generate());
    Detector d;

    {
        // Scoped database connection
        QSqlDatabase db = createConnection(connectionName);

        if (!db.open()) {
            const auto err = translator->getErrorMessage(FAILED_TO_OPEN_DB_MSG)
                .arg(db.lastError().text());
            logger->LogError(err);
            return Etrek::Specification::Result<Detector>::Failure(err);
        }

        QSqlQuery query(db);
        query.prepare(R"(
            SELECT id,
                   manufacturer,
                   model_name,
                   resolution,
                   size,
                   pixel_width,
                   pixel_height,
                   sensitivity,
                   imager_pixel_spacing,
                   identifier,
                   space_lut_reference,
                   spatial_resolution,
                   scan_options,
                   width,
                   height,
                   width_shift,
                   height_shift,
                   saturation_value,
                   horizontal_flip,
                   crop_mod,
                   save_raw_data,
                   last_calibration_date,
                   detector_order,
                   is_active,
                   create_date,
                   update_date
            FROM detectors
            WHERE id = ?
        )");
        query.addBindValue(detectorId);

        if (!query.exec()) {
            const auto err = translator->getCriticalMessage(QUERY_FAILED_ERROR_MSG)
                .arg(query.lastError().text());
            logger->LogError(err);
            return Etrek::Specification::Result<Etrek::Device::Data::Entity::Detector>::Failure(err);
        }

        if (query.next()) {
            d.Id = query.value("id").toInt();
            d.Manufacturer = query.value("manufacturer").toString();
            d.ModelName = query.value("model_name").toString();
            d.Resolution = query.value("resolution").toString();
            d.Size = query.value("size").toString();
            d.PixelWidth = query.value("pixel_width").toDouble();
            d.PixelHeight = query.value("pixel_height").toDouble();
            d.Sensitivity = query.value("sensitivity").toDouble();
            d.ImagerPixelSpacing = query.value("imager_pixel_spacing").toString();
            d.Identifier = query.value("identifier").toString();
            d.SpaceLUTReference = query.value("space_lut_reference").toString();
            d.SpatialResolution = query.value("spatial_resolution").toDouble();
            d.ScanOptions = query.value("scan_options").toString();
            d.Width = query.value("width").toInt();
            d.Height = query.value("height").toInt();
            d.WidthShift = query.value("width_shift").toInt();
            d.HeightShift = query.value("height_shift").toInt();
            d.SaturationValue = query.value("saturation_value").toInt();

            d.IsActive = query.value("is_active").toBool();
            d.CreateDate = query.value("create_date").toDateTime();
            d.UpdateDate = query.value("update_date").toDateTime();

            return Etrek::Specification::Result<Detector>::Success(d);
        }
    } // <- db goes out of scope here

    QSqlDatabase::removeDatabase(connectionName);
    return Etrek::Specification::Result<Detector>::Failure("Detector not found");
}

Etrek::Specification::Result<Detector> DeviceRepository::updateDetector(const Detector& detector)
{
    const QString cx = "update_detector_" + QString::number(QRandomGenerator::global()->generate());
    {
        QSqlDatabase db = createConnection(cx);
        if (!db.open()) {
            auto err = QString("Failed to open database: %1").arg(db.lastError().text());
            logger->LogError(err);
            return Etrek::Specification::Result<Detector>::Failure(err);
        }

        // === Update the detector itself ===
        QSqlQuery query(db);
        query.prepare(R"(
            UPDATE detectors
            SET manufacturer = ?,
                model_name = ?,
                resolution = ?,
                size = ?,
                pixel_width = ?,
                pixel_height = ?,
                sensitivity = ?,
                imager_pixel_spacing = ?,
                identifier = ?,
                space_lut_reference = ?,
                spatial_resolution = ?,
                scan_options = ?,
                width = ?,
                height = ?,
                width_shift = ?,
                height_shift = ?,
                saturation_value = ?,
                horizontal_flip = ?,
                crop_mod = ?,
                save_raw_data = ?,
                last_calibration_date = ?,
                detector_order = ?,
                is_active = ?,
                update_date = CURRENT_TIMESTAMP
            WHERE id = ?
        )");

        query.addBindValue(detector.Manufacturer);
        query.addBindValue(detector.ModelName);
        query.addBindValue(detector.Resolution);
        query.addBindValue(detector.Size);
        query.addBindValue(detector.PixelWidth);
        query.addBindValue(detector.PixelHeight);
        query.addBindValue(detector.Sensitivity);
        query.addBindValue(detector.ImagerPixelSpacing);
        query.addBindValue(detector.Identifier);
        query.addBindValue(detector.SpaceLUTReference);
        query.addBindValue(detector.SpatialResolution);
        query.addBindValue(detector.ScanOptions);
        query.addBindValue(detector.Width);
        query.addBindValue(detector.Height);
        query.addBindValue(detector.WidthShift);
        query.addBindValue(detector.HeightShift);
        query.addBindValue(detector.SaturationValue);

        query.addBindValue(detector.IsActive);

        query.addBindValue(detector.Id);

        if (!query.exec()) {
            auto err = QString("Failed to update detector: %1").arg(query.lastError().text());
            logger->LogError(err);
            return Etrek::Specification::Result<Detector>::Failure(err);
        }
    } // db goes out of scope

    QSqlDatabase::removeDatabase(cx);
    return Etrek::Specification::Result<Detector>::Success(detector);
}


// -------------------- Institutions --------------------

Etrek::Specification::Result<QVector<Institution>> DeviceRepository::getInstitutionList() const
{
    QVector<Institution> vec;
    const QString cx = "inst_list_" + QString::number(QRandomGenerator::global()->generate());
    {
        QSqlDatabase db = createConnection(cx);
        if (!db.open()) {
            const auto err = translator->getErrorMessage(FAILED_TO_OPEN_DB_MSG).arg(db.lastError().text());
            logger->LogError(err);
            return Etrek::Specification::Result<QVector<Institution>>::Failure(err);
        }

        QSqlQuery q(db);
        q.prepare(R"(
            SELECT id, name, institution_type, institution_id, department_name,
                   address, contact_information, is_active, create_date, update_date
            FROM institutions
            ORDER BY name
        )");

        if (!q.exec()) {
            const auto err = translator->getCriticalMessage(QUERY_FAILED_ERROR_MSG).arg(q.lastError().text());
            logger->LogError(err);
            return Etrek::Specification::Result<QVector<Institution>>::Failure(err);
        }

        while (q.next()) {
            Institution inst;
            inst.Id = q.value("id").toInt();
            inst.Name = q.value("name").toString();
            inst.InstitutionType = q.value("institution_type").toString();
            inst.InstitutionId = q.value("institution_id").toString();
            inst.DepartmentName = q.value("department_name").toString();
            inst.Address = q.value("address").toString();
            inst.ContactInformation = q.value("contact_information").toString();
            inst.IsActive = q.value("is_active").toBool();
            vec.append(inst);
        }
    }
    QSqlDatabase::removeDatabase(cx);
    return Etrek::Specification::Result<QVector<Institution>>::Success(vec);
}

Etrek::Specification::Result<Institution> DeviceRepository::getInstitutionById(int id) const
{
    const QString cx = "inst_by_id_" + QString::number(id);
    {
        QSqlDatabase db = createConnection(cx);
        if (!db.open()) {
            const auto err = translator->getErrorMessage(FAILED_TO_OPEN_DB_MSG).arg(db.lastError().text());
            logger->LogError(err);
            return Etrek::Specification::Result<Institution>::Failure(err);
        }

        QSqlQuery q(db);
        q.prepare(R"(
            SELECT id, name, institution_type, institution_id, department_name,
                   address, contact_information, is_active, create_date, update_date
            FROM institutions WHERE id = :id LIMIT 1
        )");
        q.bindValue(":id", id);

        if (!q.exec()) {
            const auto err = translator->getCriticalMessage(QUERY_FAILED_ERROR_MSG).arg(q.lastError().text());
            logger->LogError(err);
            return Etrek::Specification::Result<Institution>::Failure(err);
        }

        if (!q.next()) {
            return Etrek::Specification::Result<Institution>::Failure(QString("Institution with id %1 not found").arg(id));
        }

        Institution inst;
        inst.Id = q.value("id").toInt();
        inst.Name = q.value("name").toString();
        inst.InstitutionType = q.value("institution_type").toString();
        inst.InstitutionId = q.value("institution_id").toString();
        inst.DepartmentName = q.value("department_name").toString();
        inst.Address = q.value("address").toString();
        inst.ContactInformation = q.value("contact_information").toString();
        inst.IsActive = q.value("is_active").toBool();

        QSqlDatabase::removeDatabase(cx);
        return Etrek::Specification::Result<Institution>::Success(inst);
    }
}

Etrek::Specification::Result<Institution> DeviceRepository::createInstitution(const Institution& inst)
{
    const QString cx = "inst_create_" + QString::number(QRandomGenerator::global()->generate());
    int newId = -1;
    {
        QSqlDatabase db = createConnection(cx);
        if (!db.open()) {
            const auto err = translator->getErrorMessage(FAILED_TO_OPEN_DB_MSG).arg(db.lastError().text());
            logger->LogError(err);
            return Etrek::Specification::Result<Institution>::Failure(err);
        }

        QSqlQuery q(db);
        q.prepare(R"(
            INSERT INTO institutions
            (name, institution_type, institution_id, department_name, address, contact_information, is_active, create_date, update_date)
            VALUES
            (:name, :type, :instid, :dept, :addr, :contact, :active, CURRENT_TIMESTAMP, CURRENT_TIMESTAMP)
        )");
        q.bindValue(":name", inst.Name);
        q.bindValue(":type", inst.InstitutionType);
        q.bindValue(":instid", inst.InstitutionId);
        q.bindValue(":dept", inst.DepartmentName);
        q.bindValue(":addr", inst.Address);
        q.bindValue(":contact", inst.ContactInformation);
        q.bindValue(":active", inst.IsActive);

        if (!q.exec()) {
            const auto err = translator->getCriticalMessage(QUERY_FAILED_ERROR_MSG).arg(q.lastError().text());
            logger->LogError(err);
            return Etrek::Specification::Result<Institution>::Failure(err);
        }
        newId = q.lastInsertId().toInt();
    }
    QSqlDatabase::removeDatabase(cx);
    return getInstitutionById(newId);
}

Etrek::Specification::Result<Institution> DeviceRepository::updateInstitution(const Institution& inst)
{
    if (inst.Id <= 0) return Etrek::Specification::Result<Institution>::Failure("Invalid institution Id.");

    const QString cx = "inst_update_" + QString::number(inst.Id);
    {
        QSqlDatabase db = createConnection(cx);
        if (!db.open()) {
            const auto err = translator->getErrorMessage(FAILED_TO_OPEN_DB_MSG).arg(db.lastError().text());
            logger->LogError(err);
            return Etrek::Specification::Result<Institution>::Failure(err);
        }

        QSqlQuery q(db);
        q.prepare(R"(
            UPDATE institutions SET
                name = :name,
                institution_type = :type,
                institution_id = :instid,
                department_name = :dept,
                address = :addr,
                contact_information = :contact,
                is_active = :active,
                update_date = CURRENT_TIMESTAMP
            WHERE id = :id
        )");
        q.bindValue(":name", inst.Name);
        q.bindValue(":type", inst.InstitutionType);
        q.bindValue(":instid", inst.InstitutionId);
        q.bindValue(":dept", inst.DepartmentName);
        q.bindValue(":addr", inst.Address);
        q.bindValue(":contact", inst.ContactInformation);
        q.bindValue(":active", inst.IsActive);
        q.bindValue(":id", inst.Id);

        if (!q.exec()) {
            const auto err = translator->getCriticalMessage(QUERY_FAILED_ERROR_MSG).arg(q.lastError().text());
            logger->LogError(err);
            return Etrek::Specification::Result<Institution>::Failure(err);
        }
    }
    QSqlDatabase::removeDatabase(cx);
    return getInstitutionById(inst.Id);
}

Etrek::Specification::Result<bool> DeviceRepository::deleteInstitution(int id)
{
    const QString cx = "inst_delete_" + QString::number(id);
    {
        QSqlDatabase db = createConnection(cx);
        if (!db.open()) {
            const auto err = translator->getErrorMessage(FAILED_TO_OPEN_DB_MSG).arg(db.lastError().text());
            logger->LogError(err);
            return Etrek::Specification::Result<bool>::Failure(err);
        }

        QSqlQuery q(db);
        q.prepare("DELETE FROM institutions WHERE id = :id");
        q.bindValue(":id", id);

        if (!q.exec()) {
            const auto err = translator->getCriticalMessage(QUERY_FAILED_ERROR_MSG).arg(q.lastError().text());
            logger->LogError(err);
            return Etrek::Specification::Result<bool>::Failure(err);
        }
    }
    QSqlDatabase::removeDatabase(cx);
    return Etrek::Specification::Result<bool>::Success(true);
}

Etrek::Specification::Result<bool> DeviceRepository::deactivateInstitution(int id)
{
    const QString cx = "inst_deactivate_" + QString::number(id);
    {
        QSqlDatabase db = createConnection(cx);
        if (!db.open()) {
            const auto err = translator->getErrorMessage(FAILED_TO_OPEN_DB_MSG).arg(db.lastError().text());
            logger->LogError(err);
            return Etrek::Specification::Result<bool>::Failure(err);
        }

        QSqlQuery q(db);
        q.prepare(R"(UPDATE institutions SET is_active = 0, update_date = CURRENT_TIMESTAMP WHERE id = :id)");
        q.bindValue(":id", id);

        if (!q.exec()) {
            const auto err = translator->getCriticalMessage(QUERY_FAILED_ERROR_MSG).arg(q.lastError().text());
            logger->LogError(err);
            return Etrek::Specification::Result<bool>::Failure(err);
        }
    }
    QSqlDatabase::removeDatabase(cx);
    return Etrek::Specification::Result<bool>::Success(true);
}


// -------------------- General Equipment --------------------

Etrek::Specification::Result<QVector<GeneralEquipment>> DeviceRepository::getGeneralEquipmentList() const
{
    QVector<GeneralEquipment> vec;
    const QString cx = "ge_list_" + QString::number(QRandomGenerator::global()->generate());
    {
        QSqlDatabase db = createConnection(cx);
        if (!db.open()) {
            const auto err = translator->getErrorMessage(FAILED_TO_OPEN_DB_MSG).arg(db.lastError().text());
            logger->LogError(err);
            return Etrek::Specification::Result<QVector<GeneralEquipment>>::Failure(err);
        }

        QSqlQuery q(db);
        q.prepare(R"(
            SELECT ge.id,
                   ge.device_serial_number,
                   ge.device_uid,
                   ge.manufacturer,
                   ge.model_name,
                   ge.station_name,
                   ge.institution_id,
                   ge.department_name,
                   ge.date_of_last_calibration,
                   ge.time_of_last_calibration,
                   ge.date_of_manufacture,
                   ge.software_versions,
                   ge.gantry_id,
                   ge.is_active,
                   i.name AS inst_name,
                   i.institution_type AS inst_type,
                   i.institution_id AS inst_external_id,
                   i.department_name AS inst_dept,
                   i.address AS inst_addr,
                   i.contact_information AS inst_contact,
                   i.is_active AS inst_is_active
            FROM general_equipments ge
            LEFT JOIN institutions i ON ge.institution_id = i.id
            ORDER BY ge.manufacturer, ge.model_name, ge.device_serial_number
        )");

        if (!q.exec()) {
            const auto err = translator->getCriticalMessage(QUERY_FAILED_ERROR_MSG).arg(q.lastError().text());
            logger->LogError(err);
            return Etrek::Specification::Result<QVector<GeneralEquipment>>::Failure(err);
        }

        while (q.next()) {
            GeneralEquipment ge;
            ge.Id = q.value("id").toInt();
            ge.DeviceSerialNumber = q.value("device_serial_number").toString();
            ge.DeviceUID = q.value("device_uid").toString();
            ge.Manufacturer = q.value("manufacturer").toString();
            ge.ModelName = q.value("model_name").toString();
            ge.StationName = q.value("station_name").toString();
            ge.DepartmentName = q.value("department_name").toString();
            ge.DateOfLastCalibration = q.value("date_of_last_calibration").toDate();
            ge.TimeOfLastCalibration = q.value("time_of_last_calibration").toTime();
            ge.DateOfManufacture = q.value("date_of_manufacture").toDate();
            ge.SoftwareVersions = q.value("software_versions").toString();
            ge.GantryId = q.value("gantry_id").toString();
            ge.IsActive = q.value("is_active").toBool();

            // Institution (embedded)
            ge.Institution.Id = q.value("institution_id").isNull() ? -1 : q.value("institution_id").toInt();
            ge.Institution.Name = q.value("inst_name").toString();
            ge.Institution.InstitutionType = q.value("inst_type").toString();
            ge.Institution.InstitutionId = q.value("inst_external_id").toString();
            ge.Institution.DepartmentName = q.value("inst_dept").toString();
            ge.Institution.Address = q.value("inst_addr").toString();
            ge.Institution.ContactInformation = q.value("inst_contact").toString();
            ge.Institution.IsActive = q.value("inst_is_active").toBool();

            vec.append(ge);
        }
    }
    QSqlDatabase::removeDatabase(cx);
    return Etrek::Specification::Result<QVector<GeneralEquipment>>::Success(vec);
}

Etrek::Specification::Result<GeneralEquipment> DeviceRepository::getGeneralEquipmentById(int id) const
{
    const QString cx = "ge_by_id_" + QString::number(id);
    {
        QSqlDatabase db = createConnection(cx);
        if (!db.open()) {
            const auto err = translator->getErrorMessage(FAILED_TO_OPEN_DB_MSG).arg(db.lastError().text());
            logger->LogError(err);
            return Etrek::Specification::Result<GeneralEquipment>::Failure(err);
        }

        QSqlQuery q(db);
        q.prepare(R"(
            SELECT ge.id,
                   ge.device_serial_number,
                   ge.device_uid,
                   ge.manufacturer,
                   ge.model_name,
                   ge.station_name,
                   ge.institution_id,
                   ge.department_name,
                   ge.date_of_last_calibration,
                   ge.time_of_last_calibration,
                   ge.date_of_manufacture,
                   ge.software_versions,
                   ge.gantry_id,
                   ge.is_active,
                   i.name AS inst_name,
                   i.institution_type AS inst_type,
                   i.institution_id AS inst_external_id,
                   i.department_name AS inst_dept,
                   i.address AS inst_addr,
                   i.contact_information AS inst_contact,
                   i.is_active AS inst_is_active
            FROM general_equipments ge
            LEFT JOIN institutions i ON ge.institution_id = i.id
            WHERE ge.id = :id
            LIMIT 1
        )");
        q.bindValue(":id", id);

        if (!q.exec()) {
            const auto err = translator->getCriticalMessage(QUERY_FAILED_ERROR_MSG).arg(q.lastError().text());
            logger->LogError(err);
            return Etrek::Specification::Result<GeneralEquipment>::Failure(err);
        }

        if (!q.next()) {
            return Etrek::Specification::Result<GeneralEquipment>::Failure(QString("GeneralEquipment with id %1 not found").arg(id));
        }

        GeneralEquipment ge;
        ge.Id = q.value("id").toInt();
        ge.DeviceSerialNumber = q.value("device_serial_number").toString();
        ge.DeviceUID = q.value("device_uid").toString();
        ge.Manufacturer = q.value("manufacturer").toString();
        ge.ModelName = q.value("model_name").toString();
        ge.StationName = q.value("station_name").toString();
        ge.DepartmentName = q.value("department_name").toString();
        ge.DateOfLastCalibration = q.value("date_of_last_calibration").toDate();
        ge.TimeOfLastCalibration = q.value("time_of_last_calibration").toTime();
        ge.DateOfManufacture = q.value("date_of_manufacture").toDate();
        ge.SoftwareVersions = q.value("software_versions").toString();
        ge.GantryId = q.value("gantry_id").toString();
        ge.IsActive = q.value("is_active").toBool();

        ge.Institution.Id = q.value("institution_id").isNull() ? -1 : q.value("institution_id").toInt();
        ge.Institution.Name = q.value("inst_name").toString();
        ge.Institution.InstitutionType = q.value("inst_type").toString();
        ge.Institution.InstitutionId = q.value("inst_external_id").toString();
        ge.Institution.DepartmentName = q.value("inst_dept").toString();
        ge.Institution.Address = q.value("inst_addr").toString();
        ge.Institution.ContactInformation = q.value("inst_contact").toString();
        ge.Institution.IsActive = q.value("inst_is_active").toBool();

        QSqlDatabase::removeDatabase(cx);
        return Etrek::Specification::Result<GeneralEquipment>::Success(ge);
    }
}

Etrek::Specification::Result<GeneralEquipment> DeviceRepository::createGeneralEquipment(const GeneralEquipment& ge)
{
    const QString cx = "ge_create_" + QString::number(QRandomGenerator::global()->generate());
    int newId = -1;
    {
        QSqlDatabase db = createConnection(cx);
        if (!db.open()) {
            const auto err = translator->getErrorMessage(FAILED_TO_OPEN_DB_MSG).arg(db.lastError().text());
            logger->LogError(err);
            return Etrek::Specification::Result<GeneralEquipment>::Failure(err);
        }

        QSqlQuery q(db);
        q.prepare(R"(
            INSERT INTO general_equipments
            (device_serial_number, device_uid, manufacturer, model_name,
             station_name, institution_id, department_name,
             date_of_last_calibration, time_of_last_calibration, date_of_manufacture,
             software_versions, gantry_id, is_active, create_date, update_date)
            VALUES
            (:serial, :uid, :man, :model,
             :station, :inst_id, :dept,
             :date_cal, :time_cal, :date_manu,
             :sw, :gantry, :active, CURRENT_TIMESTAMP, CURRENT_TIMESTAMP)
        )");

        q.bindValue(":serial", ge.DeviceSerialNumber);
        q.bindValue(":uid", ge.DeviceUID);
        q.bindValue(":man", ge.Manufacturer);
        q.bindValue(":model", ge.ModelName);
        q.bindValue(":station", ge.StationName);
        q.bindValue(":inst_id", ge.Institution.Id > 0 ? ge.Institution.Id : QVariant(QVariant::Int));
        q.bindValue(":dept", ge.DepartmentName);

        q.bindValue(":date_cal", ge.DateOfLastCalibration.isValid() ? ge.DateOfLastCalibration : QVariant(QVariant::Date));
        q.bindValue(":time_cal", ge.TimeOfLastCalibration.isValid() ? ge.TimeOfLastCalibration : QVariant(QVariant::Time));
        q.bindValue(":date_manu", ge.DateOfManufacture.isValid() ? ge.DateOfManufacture : QVariant(QVariant::Date));

        q.bindValue(":sw", ge.SoftwareVersions);
        q.bindValue(":gantry", ge.GantryId);
        q.bindValue(":active", ge.IsActive);

        if (!q.exec()) {
            const auto err = translator->getCriticalMessage(QUERY_FAILED_ERROR_MSG).arg(q.lastError().text());
            logger->LogError(err);
            return Etrek::Specification::Result<GeneralEquipment>::Failure(err);
        }
        newId = q.lastInsertId().toInt();
    }
    QSqlDatabase::removeDatabase(cx);
    return getGeneralEquipmentById(newId);
}

Etrek::Specification::Result<GeneralEquipment> DeviceRepository::updateGeneralEquipment(const GeneralEquipment& ge)
{
    if (ge.Id <= 0) return Etrek::Specification::Result<GeneralEquipment>::Failure("Invalid GeneralEquipment Id.");

    const QString cx = "ge_update_" + QString::number(ge.Id);
    {
        QSqlDatabase db = createConnection(cx);
        if (!db.open()) {
            const auto err = translator->getErrorMessage(FAILED_TO_OPEN_DB_MSG).arg(db.lastError().text());
            logger->LogError(err);
            return Etrek::Specification::Result<GeneralEquipment>::Failure(err);
        }

        QSqlQuery q(db);
        q.prepare(R"(
            UPDATE general_equipments SET
                device_serial_number     = :serial,
                device_uid               = :uid,
                manufacturer             = :man,
                model_name = :model,
                station_name             = :station,
                institution_id           = :inst_id,
                department_name          = :dept,
                date_of_last_calibration = :date_cal,
                time_of_last_calibration = :time_cal,
                date_of_manufacture      = :date_manu,
                software_versions        = :sw,
                gantry_id                = :gantry,
                is_active                = :active,
                update_date              = CURRENT_TIMESTAMP
            WHERE id = :id
        )");

        q.bindValue(":id", ge.Id);
        q.bindValue(":serial", ge.DeviceSerialNumber);
        q.bindValue(":uid", ge.DeviceUID);
        q.bindValue(":man", ge.Manufacturer);
        q.bindValue(":model", ge.ModelName);
        q.bindValue(":station", ge.StationName);
        q.bindValue(":inst_id", ge.Institution.Id > 0 ? ge.Institution.Id : QVariant(QVariant::Int));
        q.bindValue(":dept", ge.DepartmentName);

        q.bindValue(":date_cal", ge.DateOfLastCalibration.isValid() ? ge.DateOfLastCalibration : QVariant(QVariant::Date));
        q.bindValue(":time_cal", ge.TimeOfLastCalibration.isValid() ? ge.TimeOfLastCalibration : QVariant(QVariant::Time));
        q.bindValue(":date_manu", ge.DateOfManufacture.isValid() ? ge.DateOfManufacture : QVariant(QVariant::Date));

        q.bindValue(":sw", ge.SoftwareVersions);
        q.bindValue(":gantry", ge.GantryId);
        q.bindValue(":active", ge.IsActive);

        if (!q.exec()) {
            const auto err = translator->getCriticalMessage(QUERY_FAILED_ERROR_MSG).arg(q.lastError().text());
            logger->LogError(err);
            return Etrek::Specification::Result<GeneralEquipment>::Failure(err);
        }
    }
    QSqlDatabase::removeDatabase(cx);
    return getGeneralEquipmentById(ge.Id);
}

Etrek::Specification::Result<bool> DeviceRepository::deleteGeneralEquipment(int id)
{
    const QString cx = "ge_delete_" + QString::number(id);
    {
        QSqlDatabase db = createConnection(cx);
        if (!db.open()) {
            const auto err = translator->getErrorMessage(FAILED_TO_OPEN_DB_MSG).arg(db.lastError().text());
            logger->LogError(err);
            return Etrek::Specification::Result<bool>::Failure(err);
        }

        QSqlQuery q(db);
        q.prepare("DELETE FROM general_equipments WHERE id = :id");
        q.bindValue(":id", id);

        if (!q.exec()) {
            const auto err = translator->getCriticalMessage(QUERY_FAILED_ERROR_MSG).arg(q.lastError().text());
            logger->LogError(err);
            return Etrek::Specification::Result<bool>::Failure(err);
        }
    }
    QSqlDatabase::removeDatabase(cx);
    return Etrek::Specification::Result<bool>::Success(true);
}

Etrek::Specification::Result<bool> DeviceRepository::deactivateGeneralEquipment(int id)
{
    const QString cx = "ge_deactivate_" + QString::number(id);
    {
        QSqlDatabase db = createConnection(cx);
        if (!db.open()) {
            const auto err = translator->getErrorMessage(FAILED_TO_OPEN_DB_MSG).arg(db.lastError().text());
            logger->LogError(err);
            return Etrek::Specification::Result<bool>::Failure(err);
        }

        QSqlQuery q(db);
        q.prepare(R"(UPDATE general_equipments SET is_active = 0, update_date = CURRENT_TIMESTAMP WHERE id = :id)");
        q.bindValue(":id", id);

        if (!q.exec()) {
            const auto err = translator->getCriticalMessage(QUERY_FAILED_ERROR_MSG).arg(q.lastError().text());
            logger->LogError(err);
            return Etrek::Specification::Result<bool>::Failure(err);
        }
    }
    QSqlDatabase::removeDatabase(cx);
    return Etrek::Specification::Result<bool>::Success(true);
}

Etrek::Specification::Result<EnvironmentSetting> DeviceRepository::getEnvironmentSettings() const
{
    const QString cx = "env_settings_get_" + QString::number(QRandomGenerator::global()->generate());
    {
        QSqlDatabase db = createConnection(cx);
        if (!db.open()) {
            const auto err = translator->getErrorMessage(FAILED_TO_OPEN_DB_MSG).arg(db.lastError().text());
            logger->LogError(err);
            return Etrek::Specification::Result<EnvironmentSetting>::Failure(err);
        }

        // Try to read the single row
        QSqlQuery q(db);
        q.prepare(R"(
            SELECT id,
                   study_level,
                   lookup_table,
                   worklist_clear_period_days,
                   worklist_refresh_period_seconds,
                   delete_log_period_days,
                   auto_refresh_worklist,
                   auto_clear_disk_space,
                   enable_mpps,
                   continue_on_echo_fail,
                   create_date,
                   update_date
            FROM environment_settings
            LIMIT 1
        )");

        if (!q.exec()) {
            const auto err = translator->getCriticalMessage(QUERY_FAILED_ERROR_MSG).arg(q.lastError().text());
            logger->LogError(err);
            return Etrek::Specification::Result<EnvironmentSetting>::Failure(err);
        }

        if (q.next()) {
            EnvironmentSetting s;
            s.Id = q.value("id").toInt();
            s.StudyLevelValue = EnvironmentSettingUtils::parseStudyLevel(q.value("study_level").toString());
            s.LookupTableValue = EnvironmentSettingUtils::parseLookupTable(q.value("lookup_table").toString());
            s.WorklistClearPeriodDays = q.value("worklist_clear_period_days").toInt();
            s.WorklistRefreshPeriodSeconds = q.value("worklist_refresh_period_seconds").toInt();
            s.DeleteLogPeriodDays = q.value("delete_log_period_days").toInt();
            s.AutoRefreshWorklist = q.value("auto_refresh_worklist").toBool();
            s.AutoClearDiskSpace = q.value("auto_clear_disk_space").toBool();
            s.EnableMPPS = q.value("enable_mpps").toBool();
            s.ContinueOnEchoFail = q.value("continue_on_echo_fail").toBool();
            s.CreateDate = q.value("create_date").toDateTime();
            s.UpdateDate = q.value("update_date").toDateTime();

            QSqlDatabase::removeDatabase(cx);
            return Etrek::Specification::Result<EnvironmentSetting>::Success(s);
        }

        // If table is empty, create a default row (optional convenience)
        QSqlQuery ins(db);
        ins.prepare(R"(
            INSERT INTO environment_settings
            (study_level, lookup_table,
             worklist_clear_period_days, worklist_refresh_period_seconds, delete_log_period_days,
             auto_refresh_worklist, auto_clear_disk_space, enable_mpps, continue_on_echo_fail,
             create_date, update_date)
            VALUES
            (:study, :lut, :clear_days, :refresh_sec, :del_days,
             :auto_refresh, :auto_clear, :mpps, :echo_fail,
             CURRENT_TIMESTAMP, CURRENT_TIMESTAMP)
        )");
        ins.bindValue(":study", EnvironmentSettingUtils::toString(StudyLevel::MultiSeriesStudy));
        ins.bindValue(":lut", EnvironmentSettingUtils::toString(LookupTable::None));
        ins.bindValue(":clear_days", 30);
        ins.bindValue(":refresh_sec", 60);
        ins.bindValue(":del_days", 90);
        ins.bindValue(":auto_refresh", true);
        ins.bindValue(":auto_clear", true);
        ins.bindValue(":mpps", true);
        ins.bindValue(":echo_fail", true);

        if (!ins.exec()) {
            const auto err = translator->getCriticalMessage(QUERY_FAILED_ERROR_MSG).arg(ins.lastError().text());
            logger->LogError(err);
            return Etrek::Specification::Result<EnvironmentSetting>::Failure(err);
        }
    }
    QSqlDatabase::removeDatabase(cx);
    // Re-read and return the inserted defaults
    return getEnvironmentSettings();
}

Etrek::Specification::Result<EnvironmentSetting> DeviceRepository::updateEnvironmentSettings(const EnvironmentSetting& s)
{
    // Always keep a single row. Default to id=1 if not provided.
    const int targetId = (s.Id > 0) ? s.Id : 1;

    const QString cx = "env_update_" + QString::number(QRandomGenerator::global()->generate());
    {
        QSqlDatabase db = createConnection(cx);
        if (!db.open()) {
            const auto err = translator->getErrorMessage(FAILED_TO_OPEN_DB_MSG).arg(db.lastError().text());
            logger->LogError(err);
            return Etrek::Specification::Result<EnvironmentSetting>::Failure(err);
        }

        // First try UPDATE
        {
            QSqlQuery q(db);
            q.prepare(R"(
                UPDATE environment_settings SET
                    study_level                      = :study_level,
                    lookup_table                     = :lookup_table,
                    worklist_clear_period_days       = :wlp_days,
                    worklist_refresh_period_seconds  = :wlr_secs,
                    delete_log_period_days           = :del_days,
                    auto_refresh_worklist            = :auto_refresh,
                    auto_clear_disk_space            = :auto_clear,
                    enable_mpps                      = :enable_mpps,
                    continue_on_echo_fail            = :cont_echo,
                    update_date                      = CURRENT_TIMESTAMP
                WHERE id = :id
            )");

            q.bindValue(":study_level", EnvironmentSettingUtils::toString(s.StudyLevelValue));
            q.bindValue(":lookup_table", EnvironmentSettingUtils::toString(s.LookupTableValue));
            q.bindValue(":wlp_days", s.WorklistClearPeriodDays);
            q.bindValue(":wlr_secs", s.WorklistRefreshPeriodSeconds);
            q.bindValue(":del_days", s.DeleteLogPeriodDays);
            q.bindValue(":auto_refresh", s.AutoRefreshWorklist);
            q.bindValue(":auto_clear", s.AutoClearDiskSpace);
            q.bindValue(":enable_mpps", s.EnableMPPS);
            q.bindValue(":cont_echo", s.ContinueOnEchoFail);
            q.bindValue(":id", targetId);

            if (!q.exec()) {
                const auto err = translator->getCriticalMessage(QUERY_FAILED_ERROR_MSG).arg(q.lastError().text());
                logger->LogError(err);
                return Etrek::Specification::Result<EnvironmentSetting>::Failure(err);
            }

            if (q.numRowsAffected() == 0) {
                // Row didn't exist—INSERT it (single-row table policy)
                QSqlQuery qi(db);
                qi.prepare(R"(
                    INSERT INTO environment_settings
                        (id, study_level, lookup_table,
                         worklist_clear_period_days, worklist_refresh_period_seconds, delete_log_period_days,
                         auto_refresh_worklist, auto_clear_disk_space, enable_mpps, continue_on_echo_fail,
                         create_date, update_date)
                    VALUES
                        (:id, :study_level, :lookup_table,
                         :wlp_days, :wlr_secs, :del_days,
                         :auto_refresh, :auto_clear, :enable_mpps, :cont_echo,
                         CURRENT_TIMESTAMP, CURRENT_TIMESTAMP)
                )");

                qi.bindValue(":id", targetId);
                qi.bindValue(":study_level", EnvironmentSettingUtils::toString(s.StudyLevelValue));
                qi.bindValue(":lookup_table", EnvironmentSettingUtils::toString(s.LookupTableValue));
                qi.bindValue(":wlp_days", s.WorklistClearPeriodDays);
                qi.bindValue(":wlr_secs", s.WorklistRefreshPeriodSeconds);
                qi.bindValue(":del_days", s.DeleteLogPeriodDays);
                qi.bindValue(":auto_refresh", s.AutoRefreshWorklist);
                qi.bindValue(":auto_clear", s.AutoClearDiskSpace);
                qi.bindValue(":enable_mpps", s.EnableMPPS);
                qi.bindValue(":cont_echo", s.ContinueOnEchoFail);

                if (!qi.exec()) {
                    const auto err = translator->getCriticalMessage(QUERY_FAILED_ERROR_MSG).arg(qi.lastError().text());
                    logger->LogError(err);
                    return Etrek::Specification::Result<EnvironmentSetting>::Failure(err);
                }
            }
        }
    }
    QSqlDatabase::removeDatabase(cx);

    // Return the fresh row (assumes you also have getEnvironmentSettings())
    return getEnvironmentSettings();
}
