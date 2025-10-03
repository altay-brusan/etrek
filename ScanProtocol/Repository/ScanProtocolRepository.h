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


using namespace Etrek::Core::Log;
using namespace Etrek::Core::Data;
using namespace Etrek::Core::Data::Model;
using namespace Etrek::ScanProtocol;
using namespace Etrek::ScanProtocol::Data::Entity;

namespace Etrek::ScanProtocol::Repository {

    class ScanProtocolRepository
    {
    public:
        explicit ScanProtocolRepository(std::shared_ptr<DatabaseConnectionSetting> connectionSetting,
            TranslationProvider* tr = nullptr);

        // Regions / BodyParts / TechniqueParameters (unchanged)
        Result<QVector<AnatomicRegion>>     getAllAnatomicRegions() const;
        Result<QVector<BodyPart>>           getAllBodyParts() const;

        Result<QVector<TechniqueParameter>> getAllTechniqueParameters() const;
        Result<void>                        upsertTechniqueParameter(const TechniqueParameter& tp) const;
        Result<void>                        deleteTechniqueParameter(int bodyPartId, BodySize size, TechniqueProfile profile) const;

        // ------------------------------- Views ---------------------------------------
        Result<QVector<View>> getAllViews() const;
        Result<QVector<View>> getViewsByBodyPart(int bodyPartId) const;
        Result<View>          getViewById(int id) const;
        Result<int>           createView(const View& v) const;        // returns new id
        Result<void>          updateView(const View& v) const;        // requires v.Id > 0
        Result<void>          deleteView(int viewId, bool hard = false) const; // soft delete by default

        // ---------------------------- View Techniques --------------------------------
        Result<QVector<ViewTechnique>> getViewTechniques(int viewId) const;

        // Upsert by PK (view_id, seq)
        Result<void> upsertViewTechnique(int viewId,
            int techniqueParameterId,
            quint8 seq,
            TechniqueParameterRole role,
            bool isActive = true) const;

        // Delete/clear
        Result<void> deleteViewTechnique(int viewId, quint8 seq) const; // by PK
        Result<void> deleteViewTechniquesByRole(int viewId, TechniqueParameterRole role) const; // convenience
        Result<void> clearViewTechniques(int viewId) const;             // all rows for a view


        // ------------------------------- Procedures ----------------------------------
        Result<QVector<Procedure>> getAllProcedures() const;
        Result<Procedure>          getProcedureById(int id) const;
        Result<int>                createProcedure(const Procedure& p) const;   // returns new id
        Result<void>               updateProcedure(const Procedure& p) const;   // requires p.Id > 0
        Result<void>               deleteProcedure(int procedureId, bool hard = false) const; // soft by default

        // Convenience filters
        Result<QVector<Procedure>> getProceduresByRegion(int regionId) const;     // where anatomic_region_id = ?
        Result<QVector<Procedure>> getProceduresByBodyPart(int bodyPartId) const; // where body_part_id = ?

        // --------------------------- Procedure ↔ Views --------------------------------
        Result<QVector<ProcedureView>> getProcedureViews(int procedureId) const; // raw join rows
        Result<QVector<View>>          getViewsForProcedure(int procedureId) const; // hydrated views

        Result<void> addProcedureView(int procedureId, int viewId) const;        // upsert (no-op if exists)
        Result<void> removeProcedureView(int procedureId, int viewId) const;     // delete one row
        Result<void> clearProcedureViews(int procedureId) const;                 // delete all rows



        ~ScanProtocolRepository();

    private:
        QSqlDatabase createConnection(const QString& connectionName) const;

    private:
        std::shared_ptr<DatabaseConnectionSetting> m_connectionSetting;
        TranslationProvider* translator;
        std::shared_ptr<AppLogger>                 logger;
    };

} // namespace Etrek::ScanProtocol::Repository

#endif // !SCANPROTOCOLREPOSITORY_H
