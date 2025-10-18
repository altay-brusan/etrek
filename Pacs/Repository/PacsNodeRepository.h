#ifndef PACSNODEREPOSITORY_H
#define PACSNODEREPOSITORY_H

#include <QSqlDatabase>
#include <QVector>
#include <QSqlQuery>
#include <QRandomGenerator>
#include <QSqlError>
#include <memory>

#include "DatabaseConnectionSetting.h"
#include "TranslationProvider.h"
#include "Result.h"
#include "AppLogger.h"
#include "PacsNode.h"  // contains PacsNode + PacsEntityType
#include "PacsEntityType.h"

namespace Etrek::Pacs::Repository {

    namespace spc = Etrek::Specification;
    namespace ent = Etrek::Pacs::Data::Entity;
    namespace mdl = Etrek::Core::Data::Model;
    namespace glob = Etrek::Core::Globalization;
    namespace lg = Etrek::Core::Log;
    using namespace Etrek::Pacs::Repository;
    class PacsNodeRepository
    {
    public:
        explicit PacsNodeRepository(std::shared_ptr<mdl::DatabaseConnectionSetting> connectionSetting);

        spc::Result<ent::PacsNode> addPacsNode(const ent::PacsNode& node) const;
        spc::Result<bool>     removePacsNode(const ent::PacsNode& node) const;
        spc::Result<ent::PacsNode> updatePacsNode(const ent::PacsNode& node) const;

        QVector<ent::PacsNode> getPacsNodes() const;

        ~PacsNodeRepository() = default;

    private:
        // Helpers
        QSqlDatabase createConnection(const QString& connectionName) const;

        static QString toEntityTypeString(PacsEntityType t);  // "Archive"/"MPPS"
        static PacsEntityType parseEntityTypeString(const QString& s);

        std::shared_ptr<mdl::DatabaseConnectionSetting> m_connectionSetting;
        glob::TranslationProvider* translator = nullptr;
        std::shared_ptr<lg::AppLogger> logger;
    };

} // namespace Etrek::Pacs::Repository

#endif // PACSNODEREPOSITORY_H
