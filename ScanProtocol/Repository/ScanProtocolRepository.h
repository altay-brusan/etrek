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

    class ScanProtocolRepository
    {
    public:
        explicit ScanProtocolRepository(std::shared_ptr<Etrek::Core::Data::Model::DatabaseConnectionSetting> connectionSetting,
            Etrek::Core::Globalization::TranslationProvider* tr = nullptr);

        // Regions / BodyParts / TechniqueParameters (unchanged)
        Etrek::Specification::Result<QVector<Etrek::ScanProtocol::Data::Entity::AnatomicRegion>>     getAllAnatomicRegions() const;
        Etrek::Specification::Result<QVector<Etrek::ScanProtocol::Data::Entity::BodyPart>>           getAllBodyParts() const;

        Etrek::Specification::Result<QVector<Etrek::ScanProtocol::Data::Entity::TechniqueParameter>> getAllTechniqueParameters() const;
        Etrek::Specification::Result<void>                        upsertTechniqueParameter(const Etrek::ScanProtocol::Data::Entity::TechniqueParameter& tp) const;
        Etrek::Specification::Result<void>                        deleteTechniqueParameter(int bodyPartId, Etrek::ScanProtocol::BodySize size, Etrek::ScanProtocol::TechniqueProfile profile) const;

        // ------------------------------- Views ---------------------------------------
        Etrek::Specification::Result<QVector<Etrek::ScanProtocol::Data::Entity::View>> getAllViews() const;
        Etrek::Specification::Result<QVector<Etrek::ScanProtocol::Data::Entity::View>> getViewsByBodyPart(int bodyPartId) const;
        Etrek::Specification::Result<Etrek::ScanProtocol::Data::Entity::View>          getViewById(int id) const;
        Etrek::Specification::Result<int>           createView(const Etrek::ScanProtocol::Data::Entity::View& v) const;        // returns new id
        Etrek::Specification::Result<void>          updateView(const Etrek::ScanProtocol::Data::Entity::View& v) const;        // requires v.Id > 0
        Etrek::Specification::Result<void>          deleteView(int viewId, bool hard = false) const; // soft delete by default

        // ---------------------------- View Techniques --------------------------------
        Etrek::Specification::Result<QVector<Etrek::ScanProtocol::Data::Entity::ViewTechnique>> getViewTechniques(int viewId) const;

        // Upsert by PK (view_id, seq)
        Etrek::Specification::Result<void> upsertViewTechnique(int viewId,
            int techniqueParameterId,
            quint8 seq,
            Etrek::ScanProtocol::TechniqueParameterRole role,
            bool isActive = true) const;

        // Delete/clear
        Etrek::Specification::Result<void> deleteViewTechnique(int viewId, quint8 seq) const; // by PK
        Etrek::Specification::Result<void> deleteViewTechniquesByRole(int viewId, Etrek::ScanProtocol::TechniqueParameterRole role) const; // convenience
        Etrek::Specification::Result<void> clearViewTechniques(int viewId) const;             // all rows for a view


        // ------------------------------- Procedures ----------------------------------
        Etrek::Specification::Result<QVector<Etrek::ScanProtocol::Data::Entity::Procedure>> getAllProcedures() const;
        Etrek::Specification::Result<Etrek::ScanProtocol::Data::Entity::Procedure>          getProcedureById(int id) const;
        Etrek::Specification::Result<int>                createProcedure(const Etrek::ScanProtocol::Data::Entity::Procedure& p) const;   // returns new id
        Etrek::Specification::Result<void>               updateProcedure(const Etrek::ScanProtocol::Data::Entity::Procedure& p) const;   // requires p.Id > 0
        Etrek::Specification::Result<void>               deleteProcedure(int procedureId, bool hard = false) const; // soft by default

        // Convenience filters
        Etrek::Specification::Result<QVector<Etrek::ScanProtocol::Data::Entity::Procedure>> getProceduresByRegion(int regionId) const;     // where anatomic_region_id = ?
        Etrek::Specification::Result<QVector<Etrek::ScanProtocol::Data::Entity::Procedure>> getProceduresByBodyPart(int bodyPartId) const; // where body_part_id = ?

        // --------------------------- Procedure ↔ Views --------------------------------
        Etrek::Specification::Result<QVector<Etrek::ScanProtocol::Data::Entity::ProcedureView>> getProcedureViews(int procedureId) const; // raw join rows
        Etrek::Specification::Result<QVector<Etrek::ScanProtocol::Data::Entity::View>>          getViewsForProcedure(int procedureId) const; // hydrated views

        Etrek::Specification::Result<void> addProcedureView(int procedureId, int viewId) const;        // upsert (no-op if exists)
        Etrek::Specification::Result<void> removeProcedureView(int procedureId, int viewId) const;     // delete one row
        Etrek::Specification::Result<void> clearProcedureViews(int procedureId) const;                 // delete all rows



        ~ScanProtocolRepository();

    private:
        QSqlDatabase createConnection(const QString& connectionName) const;

    private:
        std::shared_ptr<Etrek::Core::Data::Model::DatabaseConnectionSetting> m_connectionSetting;
        Etrek::Core::Globalization::TranslationProvider* translator;
        std::shared_ptr<Etrek::Core::Log::AppLogger> logger;
    };

} // namespace Etrek::ScanProtocol::Repository

#endif // !SCANPROTOCOLREPOSITORY_H
