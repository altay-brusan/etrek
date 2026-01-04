#include "RisProcedureMappingService.h"
#include "ScanProtocolRepository.h"
#include <QDebug>

namespace Etrek::Worklist::Service {

    RisProcedureMappingService::RisProcedureMappingService(
        std::shared_ptr<Etrek::Worklist::Repository::RisProcedureMappingRepository> mappingRepo,
        std::shared_ptr<Etrek::ScanProtocol::Repository::ScanProtocolRepository> viewRepo,
        QObject* parent)
        : QObject(parent)
        , m_mappingRepo(std::move(mappingRepo))
        , m_viewRepo(std::move(viewRepo))
    {
    }

    MappingResult RisProcedureMappingService::mapProcedureCode(
        const QString& connectionName,
        const QString& externalCode,
        const QString& codingScheme,
        const QString& codeMeaning)
    {
        MappingResult result;

        if (externalCode.isEmpty()) {
            result.warningMessage = "Empty procedure code provided";
            return result;
        }

        // Look up the mapping
        auto mapping = m_mappingRepo->findByExternalCode(connectionName, externalCode);

        if (mapping.has_value()) {
            result.success = true;
            result.viewId = mapping->InternalViewId;

            // Get view name from view repository
            if (m_viewRepo) {
                auto viewResult = m_viewRepo->getViewById(mapping->InternalViewId);
                if (viewResult.isSuccess) {
                    result.viewName = viewResult.value.Name;
                }
            }

            qDebug() << "[RisProcedureMappingService] Mapped code" << externalCode
                     << "to view ID" << result.viewId << "(" << result.viewName << ")";
        } else {
            // Code not mapped - emit signal and return warning
            result.success = false;
            result.warningMessage = QString("No mapping found for procedure code '%1' from connection '%2'")
                .arg(externalCode, connectionName);

            emit unmappedCodeEncountered(connectionName, externalCode, codeMeaning);

            qDebug() << "[RisProcedureMappingService] Unmapped code:" << externalCode
                     << "from" << connectionName;
        }

        return result;
    }

    QVector<SuggestedMapping> RisProcedureMappingService::suggestMappings(
        const QString& externalCodeMeaning)
    {
        QVector<SuggestedMapping> suggestions;

        if (externalCodeMeaning.isEmpty() || !m_viewRepo) {
            return suggestions;
        }

        // Get all views and perform simple fuzzy matching
        auto viewsResult = m_viewRepo->getAllViews();
        if (!viewsResult.isSuccess) {
            return suggestions;
        }

        QString searchTerm = externalCodeMeaning.toLower();
        QStringList searchWords = searchTerm.split(QRegularExpression("\\W+"), Qt::SkipEmptyParts);

        for (const auto& view : viewsResult.value) {
            QString viewName = view.Name.toLower();
            QString viewDesc = view.Description.toLower();

            // Calculate simple match score
            int matchCount = 0;
            for (const QString& word : searchWords) {
                if (word.length() < 3) continue;  // Skip short words

                if (viewName.contains(word) || viewDesc.contains(word)) {
                    matchCount++;
                }
            }

            if (matchCount > 0) {
                SuggestedMapping suggestion;
                suggestion.viewId = view.Id;
                suggestion.viewName = view.Name;
                suggestion.confidenceScore = static_cast<double>(matchCount) / searchWords.size();
                suggestions.push_back(suggestion);
            }
        }

        // Sort by confidence score descending
        std::sort(suggestions.begin(), suggestions.end(),
            [](const SuggestedMapping& a, const SuggestedMapping& b) {
                return a.confidenceScore > b.confidenceScore;
            });

        // Limit to top 5 suggestions
        if (suggestions.size() > 5) {
            suggestions.resize(5);
        }

        return suggestions;
    }

    Etrek::Specification::Result<ent::RisProcedureMapping> RisProcedureMappingService::createMapping(
        ent::RisProcedureMapping& mapping)
    {
        return m_mappingRepo->insert(mapping);
    }

    Etrek::Specification::Result<bool> RisProcedureMappingService::updateMapping(
        const ent::RisProcedureMapping& mapping)
    {
        return m_mappingRepo->update(mapping);
    }

    Etrek::Specification::Result<bool> RisProcedureMappingService::deleteMapping(int mappingId)
    {
        return m_mappingRepo->remove(mappingId);
    }

    QVector<ent::RisProcedureMapping> RisProcedureMappingService::getMappingsForConnection(
        const QString& connectionName)
    {
        return m_mappingRepo->getByConnectionName(connectionName);
    }

    QVector<ent::RisProcedureMapping> RisProcedureMappingService::getAllMappings()
    {
        return m_mappingRepo->getAllActive();
    }

} // namespace Etrek::Worklist::Service
