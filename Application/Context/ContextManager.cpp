#include "ContextManager.h"
#include "ISessionContext.h"
#include "IWorkflowContext.h"

namespace Etrek::Application::Context {

ContextManager::ContextManager(QObject* parent)
    : QObject(parent)
    , m_sessionContext(nullptr)
{
}

ContextManager::~ContextManager() = default;

// --- Session Context Management ---

void ContextManager::setSessionContext(std::shared_ptr<Etrek::Context::ISessionContext> context)
{
    QMutexLocker locker(&m_mutex);
    m_sessionContext = std::move(context);
    locker.unlock();
    emit sessionContextChanged();
}

std::shared_ptr<Etrek::Context::ISessionContext> ContextManager::sessionContext() const
{
    QMutexLocker locker(&m_mutex);
    return m_sessionContext;
}

void ContextManager::clearSessionContext()
{
    QMutexLocker locker(&m_mutex);
    m_sessionContext.reset();
    locker.unlock();
    emit sessionContextChanged();
}

// --- Workflow Context Management ---

void ContextManager::setWorkflowContext(const QString& key, std::shared_ptr<Etrek::Context::IWorkflowContext> context)
{
    QMutexLocker locker(&m_mutex);
    m_workflowContexts[key] = std::move(context);
    locker.unlock();
    emit workflowContextChanged(key);
}

std::shared_ptr<Etrek::Context::IWorkflowContext> ContextManager::workflowContext(const QString& key) const
{
    QMutexLocker locker(&m_mutex);
    return m_workflowContexts.value(key, nullptr);
}

void ContextManager::clearWorkflowContext(const QString& key)
{
    QMutexLocker locker(&m_mutex);
    if (m_workflowContexts.remove(key) > 0) {
        locker.unlock();
        emit workflowContextCleared(key);
    }
}

void ContextManager::clearAllWorkflowContexts()
{
    QMutexLocker locker(&m_mutex);
    QStringList keys = m_workflowContexts.keys();
    m_workflowContexts.clear();
    locker.unlock();

    for (const auto& key : keys) {
        emit workflowContextCleared(key);
    }
}

// --- Utility Methods ---

bool ContextManager::hasSessionContext() const
{
    QMutexLocker locker(&m_mutex);
    return m_sessionContext != nullptr && m_sessionContext->isValid();
}

bool ContextManager::hasWorkflowContext(const QString& key) const
{
    QMutexLocker locker(&m_mutex);
    auto it = m_workflowContexts.find(key);
    return it != m_workflowContexts.end() && it.value() != nullptr && it.value()->isValid();
}

} // namespace Etrek::Application::Context
