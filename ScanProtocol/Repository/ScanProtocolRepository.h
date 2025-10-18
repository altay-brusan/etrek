#ifndef SCANPROTOCOLREPOSITORY_H
#define SCANPROTOCOLREPOSITORY_H

#include <memory>
#include <QString>
#include <QVector>
#include <QSqlDatabase>

#include "Result.h"
#include "DatabaseConnectionSetting.h"
#include "TranslationProvider.h"
#include "AppLogger.h"
#include "BodyPart.h"
#include "AnatomicRegion.h"
#include "TechniqueParameter.h"
#include "View.h"
#include "ViewTechnique.h"

#include "Procedure.h"
#include "ProcedureView.h"

namespace Etrek::ScanProtocol::Repository {

    namespace spc = Etrek::Specification;
    namespace sp = Etrek::ScanProtocol;
    namespace ent = Etrek::ScanProtocol::Data::Entity;
    namespace mdl = Etrek::Core::Data::Model;
    namespace glob = Etrek::Core::Globalization;
    namespace lg = Etrek::Core::Log;

    class ScanProtocolRepository
    {
    public:
        explicit ScanProtocolRepository(std::shared_ptr<mdl::DatabaseConnectionSetting> connectionSetting,
            glob::TranslationProvider* tr = nullptr);

        // Regions / BodyParts / TechniqueParameters (unchanged)
        spc::Result<QVector<ent::AnatomicRegion>>     getAllAnatomicRegions() const;
        spc::Result<QVector<ent::BodyPart>>           getAllBodyParts() const;

        spc::Result<QVector<ent::TechniqueParameter>> getAllTechniqueParameters() const;
        spc::Result<void>                        upsertTechniqueParameter(const ent::TechniqueParameter& tp) const;
        spc::Result<void>                        deleteTechniqueParameter(int bodyPartId, sp::BodySize size, sp::TechniqueProfile profile) const;

        // ------------------------------- Views ---------------------------------------
        spc::Result<QVector<ent::View>> getAllViews() const;
        spc::Result<QVector<ent::View>> getViewsByBodyPart(int bodyPartId) const;
        spc::Result<ent::View>          getViewById(int id) const;
        spc::Result<int>           createView(const ent::View& v) const;        // returns new id
        spc::Result<void>          updateView(const ent::View& v) const;        // requires v.Id > 0
        spc::Result<void>          deleteView(int viewId, bool hard = false) const; // soft delete by default

        // ---------------------------- View Techniques --------------------------------
        spc::Result<QVector<ent::ViewTechnique>> getViewTechniques(int viewId) const;

        // Upsert by PK (view_id, seq)
        spc::Result<void> upsertViewTechnique(int viewId,
            int techniqueParameterId,
            quint8 seq,
            sp::TechniqueParameterRole role,
            bool isActive = true) const;

        // Delete/clear
        spc::Result<void> deleteViewTechnique(int viewId, quint8 seq) const; // by PK
        spc::Result<void> deleteViewTechniquesByRole(int viewId, sp::TechniqueParameterRole role) const; // convenience
        spc::Result<void> clearViewTechniques(int viewId) const;             // all rows for a view


        // ------------------------------- Procedures ----------------------------------
        spc::Result<QVector<ent::Procedure>> getAllProcedures() const;
        spc::Result<ent::Procedure>          getProcedureById(int id) const;
        spc::Result<int>                createProcedure(const ent::Procedure& p) const;   // returns new id
        spc::Result<void>               updateProcedure(const ent::Procedure& p) const;   // requires p.Id > 0
        spc::Result<void>               deleteProcedure(int procedureId, bool hard = false) const; // soft by default

        // Convenience filters
        spc::Result<QVector<ent::Procedure>> getProceduresByRegion(int regionId) const;     // where anatomic_region_id = ?
        spc::Result<QVector<ent::Procedure>> getProceduresByBodyPart(int bodyPartId) const; // where body_part_id = ?

        // --------------------------- Procedure ↔ Views --------------------------------
        spc::Result<QVector<ent::ProcedureView>> getProcedureViews(int procedureId) const; // raw join rows
        spc::Result<QVector<ent::View>>          getViewsForProcedure(int procedureId) const; // hydrated views

        spc::Result<void> addProcedureView(int procedureId, int viewId) const;        // upsert (no-op if exists)
        spc::Result<void> removeProcedureView(int procedureId, int viewId) const;     // delete one row
        spc::Result<void> clearProcedureViews(int procedureId) const;                 // delete all rows



        ~ScanProtocolRepository();

    private:
        QSqlDatabase createConnection(const QString& connectionName) const;

    private:
        std::shared_ptr<mdl::DatabaseConnectionSetting> m_connectionSetting;
        glob::TranslationProvider* translator;
        std::shared_ptr<lg::AppLogger>                 logger;
    };

} // namespace Etrek::ScanProtocol::Repository

#endif // !SCANPROTOCOLREPOSITORY_H
