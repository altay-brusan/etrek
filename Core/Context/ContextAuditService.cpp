#include "ContextAuditService.h"
#include "ContextManager.h"
#include "ContextAuditRepository.h"
#include "ISessionContext.h"
#include "IWorkflowContext.h"
#include "IExaminationContext.h"

#include <QJsonObject>
#include <QDebug>

namespace Etrek::Core::Context {

using Etrek::Context::ContextType;
using Etrek::Context::AuditEventType;
using Etrek::Context::ContextAuditEntry;
using Etrek::Context::ISessionContext;
using Etrek::Context::IWorkflowContext;
using Etrek::Context::IExaminationContext;
using Etrek::Core::Repository::ContextAuditRepository;

ContextAuditService::ContextAuditService(
    std::shared_ptr<ContextAuditRepository> repository,
    QObject* parent)
    : QObject(parent)
    , m_repository(std::move(repository))
    , m_contextManager(nullptr)
    , m_hasSessionContext(false)
{
}

ContextAuditService::~ContextAuditService()
{
    disconnectFromContextManager();
}

bool ContextAuditService::connectToContextManager(Etrek::Context::IContextManager* manager)
{
    QMutexLocker locker(&m_mutex);

    // Disconnect from previous manager if any
    if (m_contextManager) {
        locker.unlock();
        disconnectFromContextManager();
        locker.relock();
    }

    // Cast to ContextManager for signal connections (we need the QObject signals)
    auto* contextMgr = dynamic_cast<ContextManager*>(manager);
    if (!contextMgr) {
        qWarning() << "ContextAuditService: Cannot connect - manager is not a ContextManager";
        return false;
    }

    m_contextManager = contextMgr;

    // Connect to ContextManager signals
    connect(m_contextManager, &ContextManager::sessionContextChanged,
            this, &ContextAuditService::onSessionContextChanged,
            Qt::QueuedConnection);

    connect(m_contextManager, &ContextManager::workflowContextChanged,
            this, &ContextAuditService::onWorkflowContextChanged,
            Qt::QueuedConnection);

    connect(m_contextManager, &ContextManager::workflowContextCleared,
            this, &ContextAuditService::onWorkflowContextCleared,
            Qt::QueuedConnection);

    // Check if there's already a session context (service connected after login)
    m_hasSessionContext = manager->hasSessionContext();

    return true;
}

void ContextAuditService::disconnectFromContextManager()
{
    QMutexLocker locker(&m_mutex);

    if (m_contextManager) {
        disconnect(m_contextManager, nullptr, this, nullptr);
        m_contextManager = nullptr;
    }

    m_hasSessionContext = false;
}

bool ContextAuditService::isConnected() const
{
    QMutexLocker locker(&m_mutex);
    return m_contextManager != nullptr;
}

void ContextAuditService::onSessionContextChanged()
{
    if (!m_contextManager || !m_repository) {
        return;
    }

    auto sessionCtx = m_contextManager->sessionContext();

    ContextAuditEntry entry;
    entry.contextType = ContextType::SESSION;
    entry.eventTimestamp = QDateTime::currentDateTime();

    if (sessionCtx && sessionCtx->isValid()) {
        // Session created or updated
        entry.eventType = m_hasSessionContext ? AuditEventType::UPDATED : AuditEventType::CREATED;
        entry.userName = sessionCtx->username();
        entry.workstationName = sessionCtx->workstationName();
        entry.details = extractSessionDetails(sessionCtx);

        if (auto user = sessionCtx->currentUser()) {
            entry.userId = user->Id;
        }

        m_hasSessionContext = true;
    } else {
        // Session cleared (logout)
        entry.eventType = AuditEventType::CLEARED;
        m_hasSessionContext = false;
    }

    auto result = m_repository->insert(entry);
    if (!result.isSuccess) {
        qWarning() << "ContextAuditService: Failed to persist session audit entry:" << result.message;
    }
}

void ContextAuditService::onWorkflowContextChanged(const QString& key)
{
    if (!m_contextManager || !m_repository) {
        return;
    }

    auto workflowCtx = m_contextManager->workflowContext(key);
    if (!workflowCtx) {
        return;
    }

    ContextAuditEntry entry;
    entry.contextType = ContextType::WORKFLOW;
    entry.contextKey = key;
    entry.eventType = AuditEventType::CREATED;
    entry.eventTimestamp = QDateTime::currentDateTime();
    entry.details = extractWorkflowDetails(workflowCtx);

    // Get user info from session context if available
    if (auto sessionCtx = m_contextManager->sessionContext()) {
        entry.userName = sessionCtx->username();
        entry.workstationName = sessionCtx->workstationName();
        if (auto user = sessionCtx->currentUser()) {
            entry.userId = user->Id;
        }
    }

    auto result = m_repository->insert(entry);
    if (!result.isSuccess) {
        qWarning() << "ContextAuditService: Failed to persist workflow audit entry:" << result.message;
    }
}

void ContextAuditService::onWorkflowContextCleared(const QString& key)
{
    if (!m_repository) {
        return;
    }

    ContextAuditEntry entry;
    entry.contextType = ContextType::WORKFLOW;
    entry.contextKey = key;
    entry.eventType = AuditEventType::CLEARED;
    entry.eventTimestamp = QDateTime::currentDateTime();

    // Get user info from session context if available
    if (m_contextManager) {
        if (auto sessionCtx = m_contextManager->sessionContext()) {
            entry.userName = sessionCtx->username();
            entry.workstationName = sessionCtx->workstationName();
            if (auto user = sessionCtx->currentUser()) {
                entry.userId = user->Id;
            }
        }
    }

    auto result = m_repository->insert(entry);
    if (!result.isSuccess) {
        qWarning() << "ContextAuditService: Failed to persist workflow cleared audit entry:" << result.message;
    }
}

QJsonObject ContextAuditService::extractSessionDetails(const std::shared_ptr<ISessionContext>& context) const
{
    QJsonObject details;

    if (!context) {
        return details;
    }

    details["username"] = context->username();
    details["user_full_name"] = context->userFullName();
    details["workstation"] = context->workstationName();
    details["institution"] = context->institutionName();
    details["signin_time"] = context->signInTime().toString(Qt::ISODateWithMs);

    if (auto user = context->currentUser()) {
        details["user_id"] = user->Id;
    }

    return details;
}

QJsonObject ContextAuditService::extractWorkflowDetails(const std::shared_ptr<IWorkflowContext>& context) const
{
    QJsonObject details;

    if (!context) {
        return details;
    }

    details["workflow_id"] = context->workflowId();
    details["is_complete"] = context->isComplete();

    // Check if this is an examination context for additional details
    if (auto examCtx = std::dynamic_pointer_cast<IExaminationContext>(context)) {
        details["worklist_entry_id"] = examCtx->worklistEntryId();
        details["patient_name"] = examCtx->patientName();
        details["patient_id"] = examCtx->patientId();
        details["accession_number"] = examCtx->accessionNumber();
        details["body_part"] = examCtx->bodyPartExamined();
        details["study_description"] = examCtx->studyDescription();
        details["referring_physician"] = examCtx->referringPhysician();
    }

    return details;
}

// --- Query Methods (delegate to repository) ---

QVector<ContextAuditEntry> ContextAuditService::getAuditHistoryForUser(int userId, int limit)
{
    if (!m_repository) {
        return {};
    }

    auto result = m_repository->getByUserId(userId, limit);
    if (!result.isSuccess) {
        qWarning() << "ContextAuditService: Failed to get audit history for user:" << result.message;
        return {};
    }

    return result.value;
}

QVector<ContextAuditEntry> ContextAuditService::getRecentAuditHistory(ContextType contextType, int limit)
{
    if (!m_repository) {
        return {};
    }

    auto result = m_repository->getByContextType(contextType, limit);
    if (!result.isSuccess) {
        qWarning() << "ContextAuditService: Failed to get recent audit history:" << result.message;
        return {};
    }

    return result.value;
}

QVector<ContextAuditEntry> ContextAuditService::getAuditHistoryByDateRange(
    const QDateTime& from,
    const QDateTime& to,
    std::optional<ContextType> contextType)
{
    if (!m_repository) {
        return {};
    }

    auto result = m_repository->getByDateRange(from, to, contextType);
    if (!result.isSuccess) {
        qWarning() << "ContextAuditService: Failed to get audit history by date range:" << result.message;
        return {};
    }

    return result.value;
}

QVector<ContextAuditEntry> ContextAuditService::getLoginHistory(int userId, int days)
{
    if (!m_repository) {
        return {};
    }

    auto result = m_repository->getLoginHistory(userId, days);
    if (!result.isSuccess) {
        qWarning() << "ContextAuditService: Failed to get login history:" << result.message;
        return {};
    }

    return result.value;
}

} // namespace Etrek::Core::Context
