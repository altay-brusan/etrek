#include "RisProcedureMappingConfigurationDelegate.h"
#include "RisProcedureMappingConfigurationWidget.h"
#include "RisProcedureMappingRepository.h"
#include <QDebug>

namespace Etrek::Worklist::Delegate
{
    using Etrek::Worklist::Repository::RisProcedureMappingRepository;

    RisProcedureMappingConfigurationDelegate::RisProcedureMappingConfigurationDelegate(
        RisProcedureMappingConfigurationWidget* widget,
        std::shared_ptr<RisProcedureMappingRepository> repository,
        QObject* parent)
        : QObject(parent)
        , m_widget(widget)
        , m_repository(repository)
    {
    }

    RisProcedureMappingConfigurationDelegate::~RisProcedureMappingConfigurationDelegate()
    {
    }

    QString RisProcedureMappingConfigurationDelegate::name() const
    {
        return QStringLiteral("RisProcedureMappingConfigurationDelegate");
    }

    void RisProcedureMappingConfigurationDelegate::attachDelegates(const QVector<QObject*>& delegates)
    {
        Q_UNUSED(delegates);
    }

    void RisProcedureMappingConfigurationDelegate::apply()
    {
        saveAllMappings();
    }

    void RisProcedureMappingConfigurationDelegate::accept()
    {
        saveAllMappings();
    }

    void RisProcedureMappingConfigurationDelegate::reject()
    {
        // Discard changes - nothing to do
    }

    bool RisProcedureMappingConfigurationDelegate::saveAllMappings()
    {
        if (!m_widget || !m_repository) {
            qWarning() << "[RisProcedureMappingConfigurationDelegate] Cannot save - widget or repository is null";
            return false;
        }

        bool allSuccess = true;

        // Handle deletions first
        for (int deletedId : m_widget->getDeletedMappingIds()) {
            auto result = m_repository->remove(deletedId);
            if (!result.isSuccess) {
                qWarning() << "[RisProcedureMappingConfigurationDelegate] Failed to delete mapping"
                           << deletedId << ":" << result.message;
                allSuccess = false;
            }
        }

        // Handle inserts and updates
        auto modifiedMappings = m_widget->getModifiedMappings();
        for (auto& mapping : modifiedMappings) {
            if (mapping.Id < 0) {
                // New mapping - insert
                auto result = m_repository->insert(mapping);
                if (!result.isSuccess) {
                    qWarning() << "[RisProcedureMappingConfigurationDelegate] Failed to insert mapping:"
                               << result.message;
                    allSuccess = false;
                }
            } else {
                // Existing mapping - update
                auto result = m_repository->update(mapping);
                if (!result.isSuccess) {
                    qWarning() << "[RisProcedureMappingConfigurationDelegate] Failed to update mapping"
                               << mapping.Id << ":" << result.message;
                    allSuccess = false;
                }
            }
        }

        return allSuccess;
    }
}
