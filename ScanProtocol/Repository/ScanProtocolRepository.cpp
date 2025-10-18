#include "ScanProtocolRepository.h"

#include <QSqlError>
#include <QSqlQuery>
#include <QSqlRecord>
#include <QVariant>
#include <QRandomGenerator>

#include "AppLoggerFactory.h"
#include "ScanProtocolUtil.h"
#include "IWorklistRepository.h"
#include "Result.h"


namespace Etrek::ScanProtocol::Repository 
{

    static const char* FAILED_TO_OPEN_DB_MSG = "Failed to open database: %1";
    static const char* QUERY_FAILED_ERROR_MSG = "Query failed: %1";
    static inline QString kRepoName() { return "ScanProtocolRepository"; }

    using namespace Etrek::Core::Log;
    using namespace Etrek::ScanProtocol::Data::Entity;
    using namespace Etrek::ScanProtocol;
    using namespace Etrek::ScanProtocol::Data::Entity;
    using namespace Etrek::Core::Log;
    using namespace Etrek::Core::Globalization;
    using namespace Etrek::Specification;

	using Etrek::Core::Log::AppLoggerFactory;
    using Etrek::Core::Globalization::TranslationProvider;
    using Etrek::ScanProtocol::ScanProtocolUtil;
    using Etrek::ScanProtocol::BodySize;
	using Etrek::ScanProtocol::TechniqueProfile;
	using Etrek::ScanProtocol::ExposureStyle;
    using Etrek::Specification::Result;
	using Etrek::Core::Data::Model::DatabaseConnectionSetting;
	using Etrek::Core::Log::AppLoggerFactory;
	using Etrek::Core::Log::AppLogger;
	using Etrek::Core::Globalization::TranslationProvider;
    using Etrek::Specification::Result;

    // ----------------------------- Local DB helpers ------------------------------
    // --- DB bind helpers ---
    inline QVariant optStrToDb(const QString& s) {
        return s.isEmpty() ? QVariant(QVariant::String) : QVariant(s);
    }
    template<typename EnumT>
    inline QVariant optEnumToDb(const std::optional<EnumT>& e, QString(*toStr)(EnumT)) {
        return e.has_value() ? QVariant(toStr(*e)) : QVariant(QVariant::String);
    }
    inline QVariant optIntToDb(const std::optional<int>& v) {
        return v.has_value() ? QVariant(*v) : QVariant(QVariant::Int);
    }
    inline int boolToTiny(bool b) { return b ? 1 : 0; }

    inline QVariant gridToDb(const std::optional<Etrek::ScanProtocol::GridType>& g) {
        return g.has_value() ? QVariant( ScanProtocolUtil::toString(*g)) : QVariant(QVariant::String);
    }

    // --- Nullable parsers (read side) ---
    using namespace Etrek::ScanProtocol;

    inline std::optional<int> parseIntNullable(const QVariant& v) {
        return v.isNull() ? std::nullopt : std::optional<int>(v.toInt());
    }
    inline std::optional<PatientOrientation> parsePatientOrientationNullable(const QVariant& v) {
        return v.isNull() ? std::nullopt : ScanProtocolUtil::parsePatientOrientation(v.toString());
    }
    inline std::optional<PatientPosition> parsePatientPositionNullable(const QVariant& v) {
        return v.isNull() ? std::nullopt : ScanProtocolUtil::parsePatientPosition(v.toString());
    }
    inline std::optional<ViewPosition> parseViewPositionNullable(const QVariant& v) {
        return v.isNull() ? std::nullopt : ScanProtocolUtil::parseViewPosition(v.toString());
    }
    inline std::optional<ImageLaterality> parseImageLateralityNullable(const QVariant& v) {
        return v.isNull() ? std::nullopt : ScanProtocolUtil::parseImageLaterality(v.toString());
    }
    inline std::optional<LabelMark> parseLabelMarkNullable(const QVariant& v) {
        return v.isNull() ? std::nullopt : ScanProtocolUtil::parseLabelMark(v.toString());
    }
    inline std::optional<LabelPosition> parseLabelPositionNullable(const QVariant& v) {
        return v.isNull() ? std::nullopt : ScanProtocolUtil::parseLabelPosition(v.toString());
    }

    // --- Row mapper for views ---
    inline Etrek::ScanProtocol::Data::Entity::View mapViewRow(QSqlQuery& q) {
        using namespace Etrek::ScanProtocol::Data::Entity;
        View v;

        v.Id = q.value("id").toInt();
        v.Name = q.value("name").toString();
        v.Description = q.value("description").toString();

        v.BodyPart.Id = q.value("body_part_id").toInt();
        v.BodyPart.Name = q.value("bp_name").toString();
        v.BodyPart.CodeValue = q.value("bp_code_value").toString();
        v.BodyPart.CodingSchema = q.value("bp_coding_scheme").toString();
        v.BodyPart.Description = q.value("bp_description").toString();
        v.BodyPart.IsActive = q.value("bp_is_active").toBool();

        v.PatientOrientationRow = parsePatientOrientationNullable(q.value("patient_orientation_row"));
        v.PatientOrientationCol = parsePatientOrientationNullable(q.value("patient_orientation_col"));
        v.PatientPosition = parsePatientPositionNullable(q.value("patient_position"));

        // parseProjectionProfile returns a concrete value (not optional) in your utils
        v.ProjectionProfile = ScanProtocolUtil::parseProjectionProfile(q.value("projection_profile").toString());

        v.ViewPosition = parseViewPositionNullable(q.value("view_position"));
        v.ImageLaterality = parseImageLateralityNullable(q.value("image_laterality"));
        v.ImageRotate = parseIntNullable(q.value("image_rotate"));
        v.IconFileLocation = q.value("icon_file_location").toString();
        v.CollimatorSize = q.value("collimator_size").toString();
        v.ImageProcessingAlgorithm = q.value("image_processing_algorithm").toString();
        v.ImageHorizontalFlip = q.value("image_horizontal_flip").toBool();
        v.LabelMark = parseLabelMarkNullable(q.value("label_mark"));
        v.LabelPosition = parseLabelPositionNullable(q.value("label_position"));
        v.PositionName = q.value("position_name").toString();
        v.IsActive = q.value("is_active").toBool();

        return v;
    }

    // --- Row mapper for procedures ---
    inline Etrek::ScanProtocol::Data::Entity::Procedure mapProcedureRow(QSqlQuery& q)
    {
        using namespace Etrek::ScanProtocol;
        using namespace Etrek::ScanProtocol::Data::Entity;

        Procedure p;

        p.Id = q.value("id").toInt();
        p.Name = q.value("name").toString();
        p.CodeValue = q.value("code_value").toString();
        p.CodingSchema = q.value("coding_scheme").toString();
        p.CodeMeaning = q.value("code_meaning").toString();

        p.IsTrueSize = q.value("is_true_size").toBool();
        p.IsActive = q.value("is_active").toBool();

        // Optional enums
        {
            const QVariant po = q.value("print_orientation");
            if (!po.isNull()) p.PrintOrientation = ScanProtocolUtil::parsePrintOrientation(po.toString());
            const QVariant pf = q.value("print_format");
            if (!pf.isNull()) p.PrintFormat = ScanProtocolUtil::parsePrintFormat(pf.toString());
        }

        // Optional FK: anatomic_region_id (joined columns are nullable)
        if (!q.value("ar_id").isNull()) {
            AnatomicRegion ar;
            ar.Id = q.value("ar_id").toInt();
            ar.Name = q.value("ar_name").toString();
            ar.CodeValue = q.value("ar_code_value").toString();
            ar.CodingSchema = q.value("ar_coding_scheme").toString();
            ar.CodeMeaning = q.value("ar_code_meaning").toString();
            ar.Description = q.value("ar_description").toString();
            ar.DisplayOrder = q.value("ar_display_order").toInt();
            ar.IsActive = true; // no is_active on regions
            p.Region = std::move(ar);
        }

        // Optional FK: body_part_id (joined columns are nullable)
        if (!q.value("bp_id").isNull()) {
            BodyPart bp;
            bp.Id = q.value("bp_id").toInt();
            bp.Name = q.value("bp_name").toString();
            bp.CodeValue = q.value("bp_code_value").toString();
            bp.CodingSchema = q.value("bp_coding_scheme").toString();
            bp.Description = q.value("bp_description").toString();
            bp.IsActive = q.value("bp_is_active").toBool();
            p.BodyPart = std::move(bp);
        }

        return p;
    }


    // -------------------------------- Ctor / Dtor --------------------------------

    ScanProtocolRepository::ScanProtocolRepository(std::shared_ptr<DatabaseConnectionSetting> connectionSetting,
        TranslationProvider* tr)
        : m_connectionSetting(connectionSetting)
        , translator(tr ? tr : &TranslationProvider::Instance())
    {
        AppLoggerFactory factory(LoggerProvider::Instance(), translator);
        logger = factory.CreateLogger(kRepoName());
    }

    ScanProtocolRepository::~ScanProtocolRepository() = default;

    // ------------------------------- Connection ----------------------------------

    QSqlDatabase ScanProtocolRepository::createConnection(const QString& connectionName) const
    {
        QSqlDatabase db = QSqlDatabase::addDatabase("QMYSQL", connectionName);
        db.setHostName(m_connectionSetting->getHostName());
        db.setDatabaseName(m_connectionSetting->getDatabaseName());
        db.setUserName(m_connectionSetting->getEtrekUserName());
        db.setPassword(m_connectionSetting->getPassword());
        db.setPort(m_connectionSetting->getPort());
        return db;
    }

    // --------------------------------- Regions -----------------------------------

    Result<QVector<AnatomicRegion>> ScanProtocolRepository::getAllAnatomicRegions() const
    {
        QVector<AnatomicRegion> rows;
        const QString cx = "anatomic_regions_" + QString::number(QRandomGenerator::global()->generate());
        {
            QSqlDatabase db = createConnection(cx);
            if (!db.open()) {
                const auto err = translator->getErrorMessage(FAILED_TO_OPEN_DB_MSG).arg(db.lastError().text());
                logger->LogError(err);
                return Result<QVector<AnatomicRegion>>::Failure(err);
            }

            QSqlQuery q(db);
            q.prepare(R"(
            SELECT
                id, name, code_value, coding_scheme, code_meaning,
                description, display_order
            FROM anatomic_regions
            ORDER BY display_order, name
        )");

            if (!q.exec()) {
                const auto err = translator->getCriticalMessage(QUERY_FAILED_ERROR_MSG).arg(q.lastError().text());
                logger->LogError(err);
                QSqlDatabase::removeDatabase(cx);
                return Result<QVector<AnatomicRegion>>::Failure(err);
            }

            while (q.next()) {
                const QSqlRecord rec = q.record();
                AnatomicRegion ar;
                ar.Id = rec.value("id").toInt();
                ar.Name = rec.value("name").toString();
                ar.CodeValue = rec.value("code_value").toString();
                ar.CodingSchema = rec.value("coding_scheme").toString();
                ar.CodeMeaning = rec.value("code_meaning").toString();
                ar.Description = rec.value("description").toString();
                ar.DisplayOrder = rec.value("display_order").toInt();
                ar.IsActive = true; // no is_active column
                rows.push_back(std::move(ar));
            }
        }
        QSqlDatabase::removeDatabase(cx);
        return Result<QVector<AnatomicRegion>>::Success(rows);
    }

    // -------------------------------- Body Parts ---------------------------------

    Result<QVector<BodyPart>> ScanProtocolRepository::getAllBodyParts() const
    {
        QVector<BodyPart> rows;
        const QString cx = "body_parts_" + QString::number(QRandomGenerator::global()->generate());
        {
            QSqlDatabase db = createConnection(cx);
            if (!db.open()) {
                const auto err = translator->getErrorMessage(FAILED_TO_OPEN_DB_MSG).arg(db.lastError().text());
                logger->LogError(err);
                return Result<QVector<BodyPart>>::Failure(err);
            }

            QSqlQuery q(db);
            q.prepare(R"(
            SELECT
                bp.id               AS bp_id,
                bp.name             AS bp_name,
                bp.code_value       AS bp_code_value,
                bp.coding_scheme    AS bp_coding_scheme,
                bp.description      AS bp_description,
                bp.anatomic_region_id,
                bp.is_active        AS bp_is_active,

                ar.id               AS ar_id,
                ar.name             AS ar_name,
                ar.code_value       AS ar_code_value,
                ar.coding_scheme    AS ar_coding_scheme,
                ar.code_meaning     AS ar_code_meaning,
                ar.description      AS ar_description,
                ar.display_order    AS ar_display_order
            FROM body_parts bp
            JOIN anatomic_regions ar ON ar.id = bp.anatomic_region_id
            ORDER BY ar.display_order, ar.name, bp.name
        )");

            if (!q.exec()) {
                const auto err = translator->getCriticalMessage(QUERY_FAILED_ERROR_MSG).arg(q.lastError().text());
                logger->LogError(err);
                QSqlDatabase::removeDatabase(cx);
                return Result<QVector<BodyPart>>::Failure(err);
            }

            while (q.next()) {
                BodyPart bp;
                bp.Id = q.value("bp_id").toInt();
                bp.Name = q.value("bp_name").toString();
                bp.CodeValue = q.value("bp_code_value").toString();
                bp.CodingSchema = q.value("bp_coding_scheme").toString();
                bp.Description = q.value("bp_description").toString();
                bp.IsActive = q.value("bp_is_active").toBool();

                AnatomicRegion ar;
                ar.Id = q.value("ar_id").toInt();
                ar.Name = q.value("ar_name").toString();
                ar.CodeValue = q.value("ar_code_value").toString();
                ar.CodingSchema = q.value("ar_coding_scheme").toString();
                ar.CodeMeaning = q.value("ar_code_meaning").toString();
                ar.Description = q.value("ar_description").toString();
                ar.DisplayOrder = q.value("ar_display_order").toInt();
                ar.IsActive = true;

                bp.Region = std::move(ar);
                rows.push_back(std::move(bp));
            }
        }
        QSqlDatabase::removeDatabase(cx);
        return Result<QVector<BodyPart>>::Success(rows);
    }

    // -------------------------- Technique Parameters -----------------------------

    Result<QVector<TechniqueParameter>> ScanProtocolRepository::getAllTechniqueParameters() const
    {
        QVector<TechniqueParameter> rows;
        const QString cx = "tech_params_" + QString::number(QRandomGenerator::global()->generate());
        {
            QSqlDatabase db = createConnection(cx);
            if (!db.open()) {
                const auto err = translator->getErrorMessage(FAILED_TO_OPEN_DB_MSG).arg(db.lastError().text());
                logger->LogError(err);
                return Result<QVector<TechniqueParameter>>::Failure(err);
            }

            QSqlQuery q(db);
            q.prepare(R"(
            SELECT
                tp.id,
                tp.body_part_id,
                tp.size,
                tp.technique_profile,
                tp.kvp,
                tp.ma,
                tp.ms,
                tp.fkvp,
                tp.fma,
                tp.focal_spot,
                tp.sid_min,
                tp.sid_max,
                tp.grid_type,
                tp.grid_ratio,
                tp.grid_speed,
                tp.exposure_style,
                tp.aec_field,
                tp.aec_density,

                bp.name          AS bp_name,
                bp.code_value    AS bp_code_value,
                bp.coding_scheme AS bp_coding_scheme,
                bp.description   AS bp_description,
                bp.is_active     AS bp_is_active

            FROM technique_parameters tp
            LEFT JOIN body_parts bp ON bp.id = tp.body_part_id
            ORDER BY tp.body_part_id, tp.size, tp.technique_profile, tp.id
        )");

            if (!q.exec()) {
                const auto err = translator->getCriticalMessage(QUERY_FAILED_ERROR_MSG).arg(q.lastError().text());
                logger->LogError(err);
                QSqlDatabase::removeDatabase(cx);
                return Result<QVector<TechniqueParameter>>::Failure(err);
            }

            while (q.next()) {
                TechniqueParameter tp;
                tp.Id = q.value("id").toInt();

                tp.BodyPart.Id = q.value("body_part_id").toInt();
                tp.BodyPart.Name = q.value("bp_name").toString();
                tp.BodyPart.CodeValue = q.value("bp_code_value").toString();
                tp.BodyPart.CodingSchema = q.value("bp_coding_scheme").toString();
                tp.BodyPart.Description = q.value("bp_description").toString();
                tp.BodyPart.IsActive = q.value("bp_is_active").toBool();

                tp.Size = ScanProtocolUtil::parseSize(q.value("size").toString());
                tp.Profile = ScanProtocolUtil::parseProfile(q.value("technique_profile").toString());

                tp.Kvp = q.value("kvp").toInt();
                tp.Ma = q.value("ma").toInt();
                tp.Ms = q.value("ms").toInt();
                tp.FKvp = q.value("fkvp").toInt();
                tp.FMa = q.value("fma").toDouble();
                tp.FocalSpotSize = q.value("focal_spot").isNull() ? 0 : q.value("focal_spot").toInt();
                tp.SIDMin = q.value("sid_min").isNull() ? 0.0 : q.value("sid_min").toDouble();
                tp.SIDMax = q.value("sid_max").isNull() ? 0.0 : q.value("sid_max").toDouble();

                const auto gridStr = q.value("grid_type").toString();
                if (gridStr.isEmpty()) tp.GridType.reset();
                else tp.GridType = ScanProtocolUtil::parseGridType(gridStr);

                tp.GridRatio = q.value("grid_ratio").toString();
                tp.GridSpeed = q.value("grid_speed").toString();
                tp.ExposureStyle = ScanProtocolUtil::parseExposureStyle(q.value("exposure_style").toString());
                tp.AecFields = q.value("aec_field").toString();
                tp.AecDensity = q.value("aec_density").isNull() ? 0 : q.value("aec_density").toInt();

                rows.push_back(std::move(tp));
            }
        }
        QSqlDatabase::removeDatabase(cx);
        return Result<QVector<TechniqueParameter>>::Success(rows);
    }

    // ------------------------ Upsert / Delete Technique --------------------------

    static QString sizeToDb(BodySize s) { return ScanProtocolUtil::toString(s); }
    static QString profToDb(TechniqueProfile p) { return ScanProtocolUtil::toString(p); }
    static QString expToDb(ExposureStyle e) { return ScanProtocolUtil::toString(e); }

    Result<void> ScanProtocolRepository::upsertTechniqueParameter(const TechniqueParameter& tp) const
    {
        const QString cx = "tp_upsert_" + QString::number(QRandomGenerator::global()->generate());
        {
            QSqlDatabase db = createConnection(cx);
            if (!db.open())
                return Result<void>::Failure(translator->getErrorMessage(FAILED_TO_OPEN_DB_MSG).arg(db.lastError().text()));

            QSqlQuery q(db);
            q.prepare(R"(
            UPDATE technique_parameters
               SET kvp=?, ma=?, ms=?, fkvp=?, fma=?, focal_spot=?, sid_min=?, sid_max=?,
                   grid_type=?, grid_ratio=?, grid_speed=?, exposure_style=?, aec_field=?, aec_density=?
             WHERE body_part_id=? AND size=? AND technique_profile=?
        )");
            q.addBindValue(tp.Kvp);
            q.addBindValue(tp.Ma);
            q.addBindValue(tp.Ms);
            q.addBindValue(tp.FKvp);
            q.addBindValue(tp.FMa);
            q.addBindValue(tp.FocalSpotSize);
            q.addBindValue(QVariant::fromValue(tp.SIDMin)); // double
            q.addBindValue(QVariant::fromValue(tp.SIDMax)); // double
            q.addBindValue(gridToDb(tp.GridType));
            q.addBindValue(tp.GridRatio);
            q.addBindValue(tp.GridSpeed);
            q.addBindValue(expToDb(tp.ExposureStyle));
            q.addBindValue(tp.AecFields);
            q.addBindValue(tp.AecDensity);
            q.addBindValue(tp.BodyPart.Id);
            q.addBindValue(sizeToDb(tp.Size));
            q.addBindValue(profToDb(tp.Profile));

            if (!q.exec()) {
                const auto err = translator->getCriticalMessage(QUERY_FAILED_ERROR_MSG).arg(q.lastError().text());
                logger->LogError(err);
                QSqlDatabase::removeDatabase(cx);
                return Result<void>::Failure(err);
            }

            if (q.numRowsAffected() == 0) {
                QSqlQuery ins(db);
                ins.prepare(R"(
                INSERT INTO technique_parameters
                    (body_part_id, size, technique_profile,
                     kvp, ma, ms, fkvp, fma, focal_spot,
                     sid_min, sid_max, grid_type, grid_ratio, grid_speed,
                     exposure_style, aec_field, aec_density)
                VALUES (?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?)
            )");
                ins.addBindValue(tp.BodyPart.Id);
                ins.addBindValue(sizeToDb(tp.Size));
                ins.addBindValue(profToDb(tp.Profile));
                ins.addBindValue(tp.Kvp);
                ins.addBindValue(tp.Ma);
                ins.addBindValue(tp.Ms);
                ins.addBindValue(tp.FKvp);
                ins.addBindValue(tp.FMa);
                ins.addBindValue(tp.FocalSpotSize);
                ins.addBindValue(QVariant::fromValue(tp.SIDMin)); // double
                ins.addBindValue(QVariant::fromValue(tp.SIDMax)); // double
                ins.addBindValue(gridToDb(tp.GridType));
                ins.addBindValue(tp.GridRatio);
                ins.addBindValue(tp.GridSpeed);
                ins.addBindValue(expToDb(tp.ExposureStyle));
                ins.addBindValue(tp.AecFields);
                ins.addBindValue(tp.AecDensity);

                if (!ins.exec()) {
                    const auto err = translator->getCriticalMessage(QUERY_FAILED_ERROR_MSG).arg(ins.lastError().text());
                    logger->LogError(err);
                    QSqlDatabase::removeDatabase(cx);
                    return Result<void>::Failure(err);
                }
            }
        }
        QSqlDatabase::removeDatabase(cx);
        return Result<void>::Success({});
    }

    Result<void> ScanProtocolRepository::deleteTechniqueParameter(int bodyPartId,
        BodySize size,
        TechniqueProfile profile) const
    {
        const QString cx = "tp_delete_" + QString::number(QRandomGenerator::global()->generate());
        {
            QSqlDatabase db = createConnection(cx);
            if (!db.open())
                return Result<void>::Failure(translator->getErrorMessage(FAILED_TO_OPEN_DB_MSG).arg(db.lastError().text()));

            QSqlQuery q(db);
            q.prepare(R"(DELETE FROM technique_parameters
                      WHERE body_part_id=? AND size=? AND technique_profile=?)");
            q.addBindValue(bodyPartId);
            q.addBindValue(sizeToDb(size));
            q.addBindValue(profToDb(profile));

            if (!q.exec()) {
                const auto err = translator->getCriticalMessage(QUERY_FAILED_ERROR_MSG).arg(q.lastError().text());
                logger->LogError(err);
                QSqlDatabase::removeDatabase(cx);
                return Result<void>::Failure(err);
            }
        }
        QSqlDatabase::removeDatabase(cx);
        return Result<void>::Success({});
    }

    // ------------------------------------ Views ----------------------------------

    Result<QVector<View>> ScanProtocolRepository::getAllViews() const
    {
        QVector<View> rows;
        const QString cx = "views_all_" + QString::number(QRandomGenerator::global()->generate());
        {
            QSqlDatabase db = createConnection(cx);
            if (!db.open()) {
                const auto err = translator->getErrorMessage(FAILED_TO_OPEN_DB_MSG).arg(db.lastError().text());
                logger->LogError(err);
                return Result<QVector<View>>::Failure(err);
            }

            QSqlQuery q(db);
            q.prepare(R"(
            SELECT v.*,
                   bp.name          AS bp_name,
                   bp.code_value    AS bp_code_value,
                   bp.coding_scheme AS bp_coding_scheme,
                   bp.description   AS bp_description,
                   bp.is_active     AS bp_is_active
            FROM views v
            LEFT JOIN body_parts bp ON bp.id = v.body_part_id
            ORDER BY v.body_part_id, v.name, v.id
        )");

            if (!q.exec()) {
                const auto err = translator->getCriticalMessage(QUERY_FAILED_ERROR_MSG).arg(q.lastError().text());
                logger->LogError(err);
                QSqlDatabase::removeDatabase(cx);
                return Result<QVector<View>>::Failure(err);
            }

            while (q.next())
                rows.push_back(mapViewRow(q));
        }
        QSqlDatabase::removeDatabase(cx);
        return Result<QVector<View>>::Success(rows);
    }

    Result<QVector<View>> ScanProtocolRepository::getViewsByBodyPart(int bodyPartId) const
    {
        QVector<View> rows;
        const QString cx = "views_bp_" + QString::number(QRandomGenerator::global()->generate());
        {
            QSqlDatabase db = createConnection(cx);
            if (!db.open()) {
                const auto err = translator->getErrorMessage(FAILED_TO_OPEN_DB_MSG).arg(db.lastError().text());
                logger->LogError(err);
                return Result<QVector<View>>::Failure(err);
            }

            QSqlQuery q(db);
            q.prepare(R"(
            SELECT v.*,
                   bp.name          AS bp_name,
                   bp.code_value    AS bp_code_value,
                   bp.coding_scheme AS bp_coding_scheme,
                   bp.description   AS bp_description,
                   bp.is_active     AS bp_is_active
            FROM views v
            LEFT JOIN body_parts bp ON bp.id = v.body_part_id
            WHERE v.body_part_id = ?
            ORDER BY v.name, v.id
        )");
            q.addBindValue(bodyPartId);

            if (!q.exec()) {
                const auto err = translator->getCriticalMessage(QUERY_FAILED_ERROR_MSG).arg(q.lastError().text());
                logger->LogError(err);
                QSqlDatabase::removeDatabase(cx);
                return Result<QVector<View>>::Failure(err);
            }

            while (q.next())
                rows.push_back(mapViewRow(q));
        }
        QSqlDatabase::removeDatabase(cx);
        return Result<QVector<View>>::Success(rows);
    }

    Result<View> ScanProtocolRepository::getViewById(int id) const
    {
        const QString cx = "view_id_" + QString::number(QRandomGenerator::global()->generate());
        {
            QSqlDatabase db = createConnection(cx);
            if (!db.open()) {
                const auto err = translator->getErrorMessage(FAILED_TO_OPEN_DB_MSG).arg(db.lastError().text());
                logger->LogError(err);
                return Result<View>::Failure(err);
            }

            QSqlQuery q(db);
            q.prepare(R"(
            SELECT v.*,
                   bp.name          AS bp_name,
                   bp.code_value    AS bp_code_value,
                   bp.coding_scheme AS bp_coding_scheme,
                   bp.description   AS bp_description,
                   bp.is_active     AS bp_is_active
            FROM views v
            LEFT JOIN body_parts bp ON bp.id = v.body_part_id
            WHERE v.id = ?
            LIMIT 1
        )");
            q.addBindValue(id);

            if (!q.exec() || !q.next()) {
                const auto err = translator->getCriticalMessage(QUERY_FAILED_ERROR_MSG).arg(
                    q.lastError().text().isEmpty() ? "not found" : q.lastError().text());
                logger->LogError(err);
                QSqlDatabase::removeDatabase(cx);
                return Result<View>::Failure(err);
            }

            View v = mapViewRow(q);
            QSqlDatabase::removeDatabase(cx);
            return Result<View>::Success(v);
        }
    }

    Result<int> ScanProtocolRepository::createView(const View& v) const
    {
        const QString cx = "view_ins_" + QString::number(QRandomGenerator::global()->generate());
        int newId = -1;
        {
            QSqlDatabase db = createConnection(cx);
            if (!db.open())
                return Result<int>::Failure(translator->getErrorMessage(FAILED_TO_OPEN_DB_MSG).arg(db.lastError().text()));

            QSqlQuery ins(db);
            ins.prepare(R"(
            INSERT INTO views
                (name, description, body_part_id,
                 patient_position, projection_profile,
                 patient_orientation_row, patient_orientation_col,
                 view_position, image_laterality, image_rotate,
                 icon_file_location, collimator_size, image_processing_algorithm,
                 image_horizontal_flip, label_mark, label_position,
                 position_name, is_active)
            VALUES (?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?)
        )");
            ins.addBindValue(v.Name);
            ins.addBindValue(optStrToDb(v.Description));
            ins.addBindValue(v.BodyPart.Id);
            ins.addBindValue(optEnumToDb(v.PatientPosition, ScanProtocolUtil::toString));
            ins.addBindValue(ScanProtocolUtil::toString(v.ProjectionProfile));
            ins.addBindValue(optEnumToDb(v.PatientOrientationRow, ScanProtocolUtil::toString));
            ins.addBindValue(optEnumToDb(v.PatientOrientationCol, ScanProtocolUtil::toString));
            ins.addBindValue(optEnumToDb(v.ViewPosition, ScanProtocolUtil::toString));
            ins.addBindValue(optEnumToDb(v.ImageLaterality, ScanProtocolUtil::toString));
            ins.addBindValue(optIntToDb(v.ImageRotate));
            ins.addBindValue(optStrToDb(v.IconFileLocation));
            ins.addBindValue(optStrToDb(v.CollimatorSize));
            ins.addBindValue(optStrToDb(v.ImageProcessingAlgorithm));
            ins.addBindValue(boolToTiny(v.ImageHorizontalFlip));
            ins.addBindValue(optEnumToDb(v.LabelMark, ScanProtocolUtil::toString));
            ins.addBindValue(optEnumToDb(v.LabelPosition, ScanProtocolUtil::toString));
            ins.addBindValue(optStrToDb(v.PositionName));
            ins.addBindValue(boolToTiny(v.IsActive));

            if (!ins.exec()) {
                const auto err = translator->getCriticalMessage(QUERY_FAILED_ERROR_MSG).arg(ins.lastError().text());
                logger->LogError(err);
                QSqlDatabase::removeDatabase(cx);
                return Result<int>::Failure(err);
            }

            newId = ins.lastInsertId().toInt();
        }
        QSqlDatabase::removeDatabase(cx);
        return Result<int>::Success(newId);
    }

    Result<void> ScanProtocolRepository::updateView(const View& v) const
    {
        if (v.Id <= 0)
            return Result<void>::Failure("updateView requires valid v.Id");

        const QString cx = "view_upd_" + QString::number(QRandomGenerator::global()->generate());
        {
            QSqlDatabase db = createConnection(cx);
            if (!db.open())
                return Result<void>::Failure(translator->getErrorMessage(FAILED_TO_OPEN_DB_MSG).arg(db.lastError().text()));

            QSqlQuery q(db);
            q.prepare(R"(
            UPDATE views
               SET name=?,
                   description=?,
                   body_part_id=?,
                   patient_position=?,
                   projection_profile=?,
                   patient_orientation_row=?,
                   patient_orientation_col=?,
                   view_position=?,
                   image_laterality=?,
                   image_rotate=?,
                   icon_file_location=?,
                   collimator_size=?,
                   image_processing_algorithm=?,
                   image_horizontal_flip=?,
                   label_mark=?,
                   label_position=?,
                   position_name=?,
                   is_active=?
             WHERE id=?
        )");
            q.addBindValue(v.Name);
            q.addBindValue(optStrToDb(v.Description));
            q.addBindValue(v.BodyPart.Id);
            q.addBindValue(optEnumToDb(v.PatientPosition, ScanProtocolUtil::toString));
            q.addBindValue(ScanProtocolUtil::toString(v.ProjectionProfile));
            q.addBindValue(optEnumToDb(v.PatientOrientationRow, ScanProtocolUtil::toString));
            q.addBindValue(optEnumToDb(v.PatientOrientationCol, ScanProtocolUtil::toString));
            q.addBindValue(optEnumToDb(v.ViewPosition, ScanProtocolUtil::toString));
            q.addBindValue(optEnumToDb(v.ImageLaterality, ScanProtocolUtil::toString));
            q.addBindValue(optIntToDb(v.ImageRotate));
            q.addBindValue(optStrToDb(v.IconFileLocation));
            q.addBindValue(optStrToDb(v.CollimatorSize));
            q.addBindValue(optStrToDb(v.ImageProcessingAlgorithm));
            q.addBindValue(boolToTiny(v.ImageHorizontalFlip));
            q.addBindValue(optEnumToDb(v.LabelMark, ScanProtocolUtil::toString));
            q.addBindValue(optEnumToDb(v.LabelPosition, ScanProtocolUtil::toString));
            q.addBindValue(optStrToDb(v.PositionName));
            q.addBindValue(boolToTiny(v.IsActive));
            q.addBindValue(v.Id);

            if (!q.exec()) {
                const auto err = translator->getCriticalMessage(QUERY_FAILED_ERROR_MSG).arg(q.lastError().text());
                logger->LogError(err);
                QSqlDatabase::removeDatabase(cx);
                return Result<void>::Failure(err);
            }
        }
        QSqlDatabase::removeDatabase(cx);
        return Result<void>::Success({});
    }

    Result<void> ScanProtocolRepository::deleteView(int viewId, bool hard) const
    {
        const QString cx = QString("view_del_%1").arg(QRandomGenerator::global()->generate());
        {
            QSqlDatabase db = createConnection(cx);
            if (!db.open())
                return Result<void>::Failure(translator->getErrorMessage(FAILED_TO_OPEN_DB_MSG).arg(db.lastError().text()));

            QSqlQuery q(db);
            if (hard) {
                q.prepare(R"(DELETE FROM views WHERE id = ?)");
                q.addBindValue(viewId);
            }
            else {
                q.prepare(R"(UPDATE views SET is_active = 0 WHERE id = ?)");
                q.addBindValue(viewId);
            }

            if (!q.exec()) {
                const auto err = translator->getCriticalMessage(QUERY_FAILED_ERROR_MSG).arg(q.lastError().text());
                logger->LogError(err);
                QSqlDatabase::removeDatabase(cx);
                return Result<void>::Failure(err);
            }
        }
        QSqlDatabase::removeDatabase(cx);
        return Result<void>::Success({});
    }

    // ---------------------------- View Techniques --------------------------------

    Result<QVector<ViewTechnique>> ScanProtocolRepository::getViewTechniques(int viewId) const
    {
        QVector<ViewTechnique> rows;
        const QString cx = "vt_all_" + QString::number(QRandomGenerator::global()->generate());
        {
            QSqlDatabase db = createConnection(cx);
            if (!db.open()) {
                const auto err = translator->getErrorMessage(FAILED_TO_OPEN_DB_MSG).arg(db.lastError().text());
                logger->LogError(err);
                return Result<QVector<ViewTechnique>>::Failure(err);
            }

            QSqlQuery q(db);
            q.prepare(R"(
            SELECT view_id, technique_parameter_id, seq, role, is_active
            FROM view_techniques
            WHERE view_id = ?
            ORDER BY seq
        )");
            q.addBindValue(viewId);

            if (!q.exec()) {
                const auto err = translator->getCriticalMessage(QUERY_FAILED_ERROR_MSG).arg(q.lastError().text());
                logger->LogError(err);
                QSqlDatabase::removeDatabase(cx);
                return Result<QVector<ViewTechnique>>::Failure(err);
            }

            while (q.next()) {
                ViewTechnique vt;
                vt.view_id = q.value("view_id").toInt();
                vt.technique_parameter_id = q.value("technique_parameter_id").toInt();
                vt.seq = static_cast<quint8>(q.value("seq").toUInt());
                vt.role = ScanProtocolUtil::roleFromDbString(q.value("role").toString());
                vt.IsActive = q.value("is_active").toBool();
                rows.push_back(std::move(vt));
            }
        }
        QSqlDatabase::removeDatabase(cx);
        return Result<QVector<ViewTechnique>>::Success(rows);
    }

    Result<void> ScanProtocolRepository::upsertViewTechnique(
        int viewId, int techniqueParameterId, quint8 seq,
        TechniqueParameterRole role, bool isActive) const
    {
        const QString cx = "vt_upsert_" + QString::number(QRandomGenerator::global()->generate());
        {
            QSqlDatabase db = createConnection(cx);
            if (!db.open())
                return Result<void>::Failure(translator->getErrorMessage(FAILED_TO_OPEN_DB_MSG).arg(db.lastError().text()));

            // UPDATE by PK (view_id, seq)
            QSqlQuery upd(db);
            upd.prepare(R"(
            UPDATE view_techniques
               SET technique_parameter_id = ?, role = ?, is_active = ?
             WHERE view_id = ? AND seq = ?
        )");
            upd.addBindValue(techniqueParameterId);
            upd.addBindValue(ScanProtocolUtil::roleToDbString(role));
            upd.addBindValue(boolToTiny(isActive));
            upd.addBindValue(viewId);
            upd.addBindValue(seq);

            if (!upd.exec()) {
                const auto err = translator->getCriticalMessage(QUERY_FAILED_ERROR_MSG).arg(upd.lastError().text());
                logger->LogError(err);
                QSqlDatabase::removeDatabase(cx);
                return Result<void>::Failure(err);
            }

            if (upd.numRowsAffected() == 0) {
                // INSERT
                QSqlQuery ins(db);
                ins.prepare(R"(
                INSERT INTO view_techniques
                    (view_id, technique_parameter_id, seq, role, is_active)
                VALUES (?,?,?,?,?)
            )");
                ins.addBindValue(viewId);
                ins.addBindValue(techniqueParameterId);
                ins.addBindValue(seq);
                ins.addBindValue(ScanProtocolUtil::roleToDbString(role));
                ins.addBindValue(boolToTiny(isActive));

                if (!ins.exec()) {
                    const auto err = translator->getCriticalMessage(QUERY_FAILED_ERROR_MSG).arg(ins.lastError().text());
                    logger->LogError(err);
                    QSqlDatabase::removeDatabase(cx);
                    return Result<void>::Failure(err);
                }
            }
        }
        QSqlDatabase::removeDatabase(cx);
        return Result<void>::Success({});
    }

    Result<void> ScanProtocolRepository::deleteViewTechnique(int viewId, quint8 seq) const
    {
        const QString cx = "vt_delpk_" + QString::number(QRandomGenerator::global()->generate());
        {
            QSqlDatabase db = createConnection(cx);
            if (!db.open())
                return Result<void>::Failure(translator->getErrorMessage(FAILED_TO_OPEN_DB_MSG).arg(db.lastError().text()));

            QSqlQuery q(db);
            q.prepare(R"(DELETE FROM view_techniques WHERE view_id = ? AND seq = ?)");
            q.addBindValue(viewId);
            q.addBindValue(seq);

            if (!q.exec()) {
                const auto err = translator->getCriticalMessage(QUERY_FAILED_ERROR_MSG).arg(q.lastError().text());
                logger->LogError(err);
                QSqlDatabase::removeDatabase(cx);
                return Result<void>::Failure(err);
            }
        }
        QSqlDatabase::removeDatabase(cx);
        return Result<void>::Success({});
    }

    Result<void> ScanProtocolRepository::deleteViewTechniquesByRole(
        int viewId, TechniqueParameterRole role) const
    {
        const QString cx = "vt_delrole_" + QString::number(QRandomGenerator::global()->generate());
        {
            QSqlDatabase db = createConnection(cx);
            if (!db.open())
                return Result<void>::Failure(translator->getErrorMessage(FAILED_TO_OPEN_DB_MSG).arg(db.lastError().text()));

            QSqlQuery q(db);
            q.prepare(R"(DELETE FROM view_techniques WHERE view_id = ? AND role = ?)");
            q.addBindValue(viewId);
            q.addBindValue(ScanProtocolUtil::roleToDbString(role));

            if (!q.exec()) {
                const auto err = translator->getCriticalMessage(QUERY_FAILED_ERROR_MSG).arg(q.lastError().text());
                logger->LogError(err);
                QSqlDatabase::removeDatabase(cx);
                return Result<void>::Failure(err);
            }
        }
        QSqlDatabase::removeDatabase(cx);
        return Result<void>::Success({});
    }

    Result<void> ScanProtocolRepository::clearViewTechniques(int viewId) const
    {
        const QString cx = "vt_clear_" + QString::number(QRandomGenerator::global()->generate());
        {
            QSqlDatabase db = createConnection(cx);
            if (!db.open())
                return Result<void>::Failure(translator->getErrorMessage(FAILED_TO_OPEN_DB_MSG).arg(db.lastError().text()));

            QSqlQuery q(db);
            q.prepare(R"(DELETE FROM view_techniques WHERE view_id = ?)");
            q.addBindValue(viewId);

            if (!q.exec()) {
                const auto err = translator->getCriticalMessage(QUERY_FAILED_ERROR_MSG).arg(q.lastError().text());
                logger->LogError(err);
                QSqlDatabase::removeDatabase(cx);
                return Result<void>::Failure(err);
            }
        }
        QSqlDatabase::removeDatabase(cx);
        return Result<void>::Success({});
    }


    Result<QVector<Procedure>> ScanProtocolRepository::getAllProcedures() const
    {
        QVector<Procedure> rows;
        const QString cx = "proc_all_" + QString::number(QRandomGenerator::global()->generate());
        {
            QSqlDatabase db = createConnection(cx);
            if (!db.open()) {
                const auto err = translator->getErrorMessage(FAILED_TO_OPEN_DB_MSG).arg(db.lastError().text());
                logger->LogError(err);
                return Result<QVector<Procedure>>::Failure(err);
            }

            QSqlQuery q(db);
            q.prepare(R"(
            SELECT
                p.*,

                ar.id            AS ar_id,
                ar.name          AS ar_name,
                ar.code_value    AS ar_code_value,
                ar.coding_scheme AS ar_coding_scheme,
                ar.code_meaning  AS ar_code_meaning,
                ar.description   AS ar_description,
                ar.display_order AS ar_display_order,

                bp.id            AS bp_id,
                bp.name          AS bp_name,
                bp.code_value    AS bp_code_value,
                bp.coding_scheme AS bp_coding_scheme,
                bp.description   AS bp_description,
                bp.is_active     AS bp_is_active

            FROM procedures p
            LEFT JOIN anatomic_regions ar ON ar.id = p.anatomic_region_id
            LEFT JOIN body_parts      bp ON bp.id = p.body_part_id
            ORDER BY p.name, p.id
        )");

            if (!q.exec()) {
                const auto err = translator->getCriticalMessage(QUERY_FAILED_ERROR_MSG).arg(q.lastError().text());
                logger->LogError(err);
                QSqlDatabase::removeDatabase(cx);
                return Result<QVector<Procedure>>::Failure(err);
            }

            while (q.next())
                rows.push_back(mapProcedureRow(q));
        }
        QSqlDatabase::removeDatabase(cx);
        return Result<QVector<Procedure>>::Success(rows);
    }

    Result<Procedure> ScanProtocolRepository::getProcedureById(int id) const
    {
        const QString cx = "proc_id_" + QString::number(QRandomGenerator::global()->generate());
        {
            QSqlDatabase db = createConnection(cx);
            if (!db.open()) {
                const auto err = translator->getErrorMessage(FAILED_TO_OPEN_DB_MSG).arg(db.lastError().text());
                logger->LogError(err);
                return Result<Procedure>::Failure(err);
            }

            QSqlQuery q(db);
            q.prepare(R"(
            SELECT
                p.*,

                ar.id            AS ar_id,
                ar.name          AS ar_name,
                ar.code_value    AS ar_code_value,
                ar.coding_scheme AS ar_coding_scheme,
                ar.code_meaning  AS ar_code_meaning,
                ar.description   AS ar_description,
                ar.display_order AS ar_display_order,

                bp.id            AS bp_id,
                bp.name          AS bp_name,
                bp.code_value    AS bp_code_value,
                bp.coding_scheme AS bp_coding_scheme,
                bp.description   AS bp_description,
                bp.is_active     AS bp_is_active

            FROM procedures p
            LEFT JOIN anatomic_regions ar ON ar.id = p.anatomic_region_id
            LEFT JOIN body_parts      bp ON bp.id = p.body_part_id
            WHERE p.id = ?
            LIMIT 1
        )");
            q.addBindValue(id);

            if (!q.exec() || !q.next()) {
                const auto err = translator->getCriticalMessage(QUERY_FAILED_ERROR_MSG)
                    .arg(q.lastError().text().isEmpty() ? "not found" : q.lastError().text());
                logger->LogError(err);
                QSqlDatabase::removeDatabase(cx);
                return Result<Procedure>::Failure(err);
            }

            Procedure p = mapProcedureRow(q);
            QSqlDatabase::removeDatabase(cx);
            return Result<Procedure>::Success(p);
        }
    }

    Result<int> ScanProtocolRepository::createProcedure(const Procedure& p) const
    {
        const QString cx = "proc_ins_" + QString::number(QRandomGenerator::global()->generate());
        int newId = -1;
        {
            QSqlDatabase db = createConnection(cx);
            if (!db.open())
                return Result<int>::Failure(translator->getErrorMessage(FAILED_TO_OPEN_DB_MSG).arg(db.lastError().text()));

            QSqlQuery ins(db);
            ins.prepare(R"(
            INSERT INTO procedures
                (name, code_value, coding_scheme, code_meaning,
                 is_true_size, print_orientation, print_format,
                 anatomic_region_id, body_part_id, is_active,
                 created_at, updated_at)
            VALUES (?,?,?,?,?,?,?,?,?,?, NOW(6), NOW(6))
        )");

            ins.addBindValue(p.Name);
            ins.addBindValue(optStrToDb(p.CodeValue));
            ins.addBindValue(p.CodingSchema);
            ins.addBindValue(optStrToDb(p.CodeMeaning));
            ins.addBindValue(boolToTiny(p.IsTrueSize));
            ins.addBindValue(optEnumToDb(p.PrintOrientation, ScanProtocolUtil::toString));
            ins.addBindValue(optEnumToDb(p.PrintFormat, ScanProtocolUtil::toString));
            // Optional FKs
            ins.addBindValue(p.Region.has_value() ? QVariant(p.Region->Id) : QVariant(QVariant::Int));
            ins.addBindValue(p.BodyPart.has_value() ? QVariant(p.BodyPart->Id) : QVariant(QVariant::Int));
            ins.addBindValue(boolToTiny(p.IsActive));

            if (!ins.exec()) {
                const auto err = translator->getCriticalMessage(QUERY_FAILED_ERROR_MSG).arg(ins.lastError().text());
                logger->LogError(err);
                QSqlDatabase::removeDatabase(cx);
                return Result<int>::Failure(err);
            }

            newId = ins.lastInsertId().toInt();
        }
        QSqlDatabase::removeDatabase(cx);
        return Result<int>::Success(newId);
    }

    Result<void> ScanProtocolRepository::updateProcedure(const Procedure& p) const
    {
        if (p.Id <= 0)
            return Result<void>::Failure("updateProcedure requires valid p.Id");

        const QString cx = "proc_upd_" + QString::number(QRandomGenerator::global()->generate());
        {
            QSqlDatabase db = createConnection(cx);
            if (!db.open())
                return Result<void>::Failure(translator->getErrorMessage(FAILED_TO_OPEN_DB_MSG).arg(db.lastError().text()));

            QSqlQuery q(db);
            q.prepare(R"(
            UPDATE procedures
               SET name=?,
                   code_value=?,
                   coding_scheme=?,
                   code_meaning=?,
                   is_true_size=?,
                   print_orientation=?,
                   print_format=?,
                   anatomic_region_id=?,
                   body_part_id=?,
                   is_active=?,
                   updated_at=NOW(6)
             WHERE id=?
        )");

            q.addBindValue(p.Name);
            q.addBindValue(optStrToDb(p.CodeValue));
            q.addBindValue(p.CodingSchema);
            q.addBindValue(optStrToDb(p.CodeMeaning));
            q.addBindValue(boolToTiny(p.IsTrueSize));
            q.addBindValue(optEnumToDb(p.PrintOrientation, ScanProtocolUtil::toString));
            q.addBindValue(optEnumToDb(p.PrintFormat, ScanProtocolUtil::toString));
            q.addBindValue(p.Region.has_value() ? QVariant(p.Region->Id) : QVariant(QVariant::Int));
            q.addBindValue(p.BodyPart.has_value() ? QVariant(p.BodyPart->Id) : QVariant(QVariant::Int));
            q.addBindValue(boolToTiny(p.IsActive));
            q.addBindValue(p.Id);

            if (!q.exec()) {
                const auto err = translator->getCriticalMessage(QUERY_FAILED_ERROR_MSG).arg(q.lastError().text());
                logger->LogError(err);
                QSqlDatabase::removeDatabase(cx);
                return Result<void>::Failure(err);
            }
        }
        QSqlDatabase::removeDatabase(cx);
        return Result<void>::Success({});
    }

    Result<void> ScanProtocolRepository::deleteProcedure(int procedureId, bool hard) const
    {
        const QString cx = "proc_del_" + QString::number(QRandomGenerator::global()->generate());
        {
            QSqlDatabase db = createConnection(cx);
            if (!db.open())
                return Result<void>::Failure(translator->getErrorMessage(FAILED_TO_OPEN_DB_MSG).arg(db.lastError().text()));

            QSqlQuery q(db);
            if (hard) {
                // CASCADE on procedure_views will remove links automatically
                q.prepare(R"(DELETE FROM procedures WHERE id = ?)");
                q.addBindValue(procedureId);
            }
            else {
                q.prepare(R"(UPDATE procedures SET is_active = 0, updated_at=NOW(6) WHERE id = ?)");
                q.addBindValue(procedureId);
            }

            if (!q.exec()) {
                const auto err = translator->getCriticalMessage(QUERY_FAILED_ERROR_MSG).arg(q.lastError().text());
                logger->LogError(err);
                QSqlDatabase::removeDatabase(cx);
                return Result<void>::Failure(err);
            }
        }
        QSqlDatabase::removeDatabase(cx);
        return Result<void>::Success({});
    }

    Result<QVector<Procedure>> ScanProtocolRepository::getProceduresByRegion(int regionId) const
    {
        QVector<Procedure> rows;
        const QString cx = "proc_by_region_" + QString::number(QRandomGenerator::global()->generate());
        {
            QSqlDatabase db = createConnection(cx);
            if (!db.open()) {
                const auto err = translator->getErrorMessage(FAILED_TO_OPEN_DB_MSG).arg(db.lastError().text());
                logger->LogError(err);
                return Result<QVector<Procedure>>::Failure(err);
            }

            QSqlQuery q(db);
            q.prepare(R"(
            SELECT
                p.*,

                ar.id            AS ar_id,
                ar.name          AS ar_name,
                ar.code_value    AS ar_code_value,
                ar.coding_scheme AS ar_coding_scheme,
                ar.code_meaning  AS ar_code_meaning,
                ar.description   AS ar_description,
                ar.display_order AS ar_display_order,

                bp.id            AS bp_id,
                bp.name          AS bp_name,
                bp.code_value    AS bp_code_value,
                bp.coding_scheme AS bp_coding_scheme,
                bp.description   AS bp_description,
                bp.is_active     AS bp_is_active

            FROM procedures p
            LEFT JOIN anatomic_regions ar ON ar.id = p.anatomic_region_id
            LEFT JOIN body_parts      bp ON bp.id = p.body_part_id
            WHERE p.anatomic_region_id = ?
            ORDER BY p.name, p.id
        )");
            q.addBindValue(regionId);

            if (!q.exec()) {
                const auto err = translator->getCriticalMessage(QUERY_FAILED_ERROR_MSG).arg(q.lastError().text());
                logger->LogError(err);
                QSqlDatabase::removeDatabase(cx);
                return Result<QVector<Procedure>>::Failure(err);
            }
            while (q.next()) rows.push_back(mapProcedureRow(q));
        }
        QSqlDatabase::removeDatabase(cx);
        return Result<QVector<Procedure>>::Success(rows);
    }

    Result<QVector<Procedure>> ScanProtocolRepository::getProceduresByBodyPart(int bodyPartId) const
    {
        QVector<Procedure> rows;
        const QString cx = "proc_by_bp_" + QString::number(QRandomGenerator::global()->generate());
        {
            QSqlDatabase db = createConnection(cx);
            if (!db.open()) {
                const auto err = translator->getErrorMessage(FAILED_TO_OPEN_DB_MSG).arg(db.lastError().text());
                logger->LogError(err);
                return Result<QVector<Procedure>>::Failure(err);
            }

            QSqlQuery q(db);
            q.prepare(R"(
            SELECT
                p.*,

                ar.id            AS ar_id,
                ar.name          AS ar_name,
                ar.code_value    AS ar_code_value,
                ar.coding_scheme AS ar_coding_scheme,
                ar.code_meaning  AS ar_code_meaning,
                ar.description   AS ar_description,
                ar.display_order AS ar_display_order,

                bp.id            AS bp_id,
                bp.name          AS bp_name,
                bp.code_value    AS bp_code_value,
                bp.coding_scheme AS bp_coding_scheme,
                bp.description   AS bp_description,
                bp.is_active     AS bp_is_active

            FROM procedures p
            LEFT JOIN anatomic_regions ar ON ar.id = p.anatomic_region_id
            LEFT JOIN body_parts      bp ON bp.id = p.body_part_id
            WHERE p.body_part_id = ?
            ORDER BY p.name, p.id
        )");
            q.addBindValue(bodyPartId);

            if (!q.exec()) {
                const auto err = translator->getCriticalMessage(QUERY_FAILED_ERROR_MSG).arg(q.lastError().text());
                logger->LogError(err);
                QSqlDatabase::removeDatabase(cx);
                return Result<QVector<Procedure>>::Failure(err);
            }
            while (q.next()) rows.push_back(mapProcedureRow(q));
        }
        QSqlDatabase::removeDatabase(cx);
        return Result<QVector<Procedure>>::Success(rows);
    }

    Result<QVector<ProcedureView>> ScanProtocolRepository::getProcedureViews(int procedureId) const
    {
        QVector<ProcedureView> rows;
        const QString cx = "pv_all_" + QString::number(QRandomGenerator::global()->generate());
        {
            QSqlDatabase db = createConnection(cx);
            if (!db.open()) {
                const auto err = translator->getErrorMessage(FAILED_TO_OPEN_DB_MSG).arg(db.lastError().text());
                logger->LogError(err);
                return Result<QVector<ProcedureView>>::Failure(err);
            }

            QSqlQuery q(db);
            q.prepare(R"(SELECT procedure_id, view_id FROM procedure_views WHERE procedure_id = ? ORDER BY view_id)");
            q.addBindValue(procedureId);

            if (!q.exec()) {
                const auto err = translator->getCriticalMessage(QUERY_FAILED_ERROR_MSG).arg(q.lastError().text());
                logger->LogError(err);
                QSqlDatabase::removeDatabase(cx);
                return Result<QVector<ProcedureView>>::Failure(err);
            }

            while (q.next()) {
                ProcedureView pv;
                pv.procedure_id = q.value("procedure_id").toInt();
                pv.view_id = q.value("view_id").toInt();
                rows.push_back(std::move(pv));
            }
        }
        QSqlDatabase::removeDatabase(cx);
        return Result<QVector<ProcedureView>>::Success(rows);
    }

    Result<QVector<View>> ScanProtocolRepository::getViewsForProcedure(int procedureId) const
    {
        QVector<View> rows;
        const QString cx = "pv_views_" + QString::number(QRandomGenerator::global()->generate());
        {
            QSqlDatabase db = createConnection(cx);
            if (!db.open()) {
                const auto err = translator->getErrorMessage(FAILED_TO_OPEN_DB_MSG).arg(db.lastError().text());
                logger->LogError(err);
                return Result<QVector<View>>::Failure(err);
            }

            QSqlQuery q(db);
            q.prepare(R"(
            SELECT v.*,
                   bp.name          AS bp_name,
                   bp.code_value    AS bp_code_value,
                   bp.coding_scheme AS bp_coding_scheme,
                   bp.description   AS bp_description,
                   bp.is_active     AS bp_is_active
            FROM procedure_views pv
            JOIN views v         ON v.id  = pv.view_id
            LEFT JOIN body_parts bp ON bp.id = v.body_part_id
            WHERE pv.procedure_id = ?
            ORDER BY v.name, v.id
        )");
            q.addBindValue(procedureId);

            if (!q.exec()) {
                const auto err = translator->getCriticalMessage(QUERY_FAILED_ERROR_MSG).arg(q.lastError().text());
                logger->LogError(err);
                QSqlDatabase::removeDatabase(cx);
                return Result<QVector<View>>::Failure(err);
            }

            while (q.next())
                rows.push_back(mapViewRow(q));
        }
        QSqlDatabase::removeDatabase(cx);
        return Result<QVector<View>>::Success(rows);
    }

    Result<void> ScanProtocolRepository::addProcedureView(int procedureId, int viewId) const
    {
        const QString cx = "pv_add_" + QString::number(QRandomGenerator::global()->generate());
        {
            QSqlDatabase db = createConnection(cx);
            if (!db.open())
                return Result<void>::Failure(translator->getErrorMessage(FAILED_TO_OPEN_DB_MSG).arg(db.lastError().text()));

            QSqlQuery q(db);
            // MySQL upsert; no-op if already exists (PK: procedure_id, view_id)
            q.prepare(R"(
            INSERT INTO procedure_views (procedure_id, view_id)
            VALUES (?, ?)
            ON DUPLICATE KEY UPDATE view_id = VALUES(view_id)
        )");
            q.addBindValue(procedureId);
            q.addBindValue(viewId);

            if (!q.exec()) {
                const auto err = translator->getCriticalMessage(QUERY_FAILED_ERROR_MSG).arg(q.lastError().text());
                logger->LogError(err);
                QSqlDatabase::removeDatabase(cx);
                return Result<void>::Failure(err);
            }
        }
        QSqlDatabase::removeDatabase(cx);
        return Result<void>::Success({});
    }

    Result<void> ScanProtocolRepository::removeProcedureView(int procedureId, int viewId) const
    {
        const QString cx = "pv_del_" + QString::number(QRandomGenerator::global()->generate());
        {
            QSqlDatabase db = createConnection(cx);
            if (!db.open())
                return Result<void>::Failure(translator->getErrorMessage(FAILED_TO_OPEN_DB_MSG).arg(db.lastError().text()));

            QSqlQuery q(db);
            q.prepare(R"(DELETE FROM procedure_views WHERE procedure_id = ? AND view_id = ?)");
            q.addBindValue(procedureId);
            q.addBindValue(viewId);

            if (!q.exec()) {
                const auto err = translator->getCriticalMessage(QUERY_FAILED_ERROR_MSG).arg(q.lastError().text());
                logger->LogError(err);
                QSqlDatabase::removeDatabase(cx);
                return Result<void>::Failure(err);
            }
        }
        QSqlDatabase::removeDatabase(cx);
        return Result<void>::Success({});
    }

    Result<void> ScanProtocolRepository::clearProcedureViews(int procedureId) const
    {
        const QString cx = "pv_clear_" + QString::number(QRandomGenerator::global()->generate());
        {
            QSqlDatabase db = createConnection(cx);
            if (!db.open())
                return Result<void>::Failure(translator->getErrorMessage(FAILED_TO_OPEN_DB_MSG).arg(db.lastError().text()));

            QSqlQuery q(db);
            q.prepare(R"(DELETE FROM procedure_views WHERE procedure_id = ?)");
            q.addBindValue(procedureId);

            if (!q.exec()) {
                const auto err = translator->getCriticalMessage(QUERY_FAILED_ERROR_MSG).arg(q.lastError().text());
                logger->LogError(err);
                QSqlDatabase::removeDatabase(cx);
                return Result<void>::Failure(err);
            }
        }
        QSqlDatabase::removeDatabase(cx);
        return Result<void>::Success({});
    }




}
