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

using namespace Etrek::Core::Log;
using namespace Etrek::Core::Data::Model;
using namespace Etrek::Pacs;
using namespace Etrek::Pacs::Data::Entity;

class PacsNodeRepository
{
public:
    explicit PacsNodeRepository(std::shared_ptr<DatabaseConnectionSetting> connectionSetting);

    Etrek::Specification::Result<PacsNode> addPacsNode(const PacsNode& node) const;
    Etrek::Specification::Result<bool>     removePacsNode(const PacsNode& node) const;
    Etrek::Specification::Result<PacsNode> updatePacsNode(const PacsNode& node) const;

    QVector<PacsNode> getPacsNodes() const;

    ~PacsNodeRepository() = default;

private:
    // Helpers
    QSqlDatabase createConnection(const QString& connectionName) const;

    static QString toEntityTypeString(PacsEntityType t);  // "Archive"/"MPPS"
    static PacsEntityType parseEntityTypeString(const QString& s);

    std::shared_ptr<DatabaseConnectionSetting> m_connectionSetting;
    TranslationProvider* translator = nullptr;
    std::shared_ptr<AppLogger> logger;
};

#endif // PACSNODEREPOSITORY_H
