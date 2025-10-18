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

    class PacsNodeRepository
    {
    public:
        explicit PacsNodeRepository(std::shared_ptr<Etrek::Core::Data::Model::DatabaseConnectionSetting> connectionSetting);

        Etrek::Specification::Result<Etrek::Pacs::Data::Entity::PacsNode> addPacsNode(const Etrek::Pacs::Data::Entity::PacsNode& node) const;
        Etrek::Specification::Result<bool>     removePacsNode(const Etrek::Pacs::Data::Entity::PacsNode& node) const;
        Etrek::Specification::Result<Etrek::Pacs::Data::Entity::PacsNode> updatePacsNode(const Etrek::Pacs::Data::Entity::PacsNode& node) const;

        QVector<Etrek::Pacs::Data::Entity::PacsNode> getPacsNodes() const;

        ~PacsNodeRepository() = default;

    private:
        // Helpers
        QSqlDatabase createConnection(const QString& connectionName) const;

        static QString toEntityTypeString(PacsEntityType t);  // "Archive"/"MPPS"
        static PacsEntityType parseEntityTypeString(const QString& s);

        std::shared_ptr<Etrek::Core::Data::Model::DatabaseConnectionSetting> m_connectionSetting;
        Etrek::Core::Globalization::TranslationProvider* translator = nullptr;
        std::shared_ptr<Etrek::Core::Log::AppLogger> logger;
    };

} // namespace Etrek::Pacs::Repository

#endif // PACSNODEREPOSITORY_H
