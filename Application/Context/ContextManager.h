#ifndef CONTEXTMANAGER_H
#define CONTEXTMANAGER_H

#include <QObject>
#include <QMap>
#include <QString>
#include <memory>
#include <QMutex>

#include "IContextManager.h"

namespace Etrek::Context {
    class ISessionContext;
    class IWorkflowContext;
}

namespace Etrek::Application::Context {

/**
 * @class ContextManager
 * @brief Centralized context management service implementation.
 *
 * The ContextManager maintains application-wide context information:
 * - Session context: User info, sign-in time, workstation, institution
 * - Workflow contexts: Transient state for workflows (e.g., Examination)
 *
 * This class is thread-safe and can be accessed from multiple threads.
 *
 * Usage:
 * 1. ApplicationService creates and holds the ContextManager instance
 * 2. After login, ApplicationService sets the session context
 * 3. When user selects a worklist item, the delegate sets workflow context
 * 4. Builders fetch context and inject into DelegateParameter
 */
class ContextManager : public QObject, public Etrek::Context::IContextManager {
    Q_OBJECT

public:
    explicit ContextManager(QObject* parent = nullptr);
    ~ContextManager() override;

    // --- IContextManager: Session Context ---
    void setSessionContext(std::shared_ptr<Etrek::Context::ISessionContext> context) override;
    std::shared_ptr<Etrek::Context::ISessionContext> sessionContext() const override;
    void clearSessionContext() override;

    // --- IContextManager: Workflow Context ---
    void setWorkflowContext(const QString& key, std::shared_ptr<Etrek::Context::IWorkflowContext> context) override;
    std::shared_ptr<Etrek::Context::IWorkflowContext> workflowContext(const QString& key) const override;
    void clearWorkflowContext(const QString& key) override;
    void clearAllWorkflowContexts() override;

    // --- IContextManager: Utility ---
    bool hasSessionContext() const override;
    bool hasWorkflowContext(const QString& key) const override;

signals:
    /**
     * @brief Emitted when the session context changes.
     */
    void sessionContextChanged();

    /**
     * @brief Emitted when a workflow context is set or updated.
     * @param key The workflow identifier.
     */
    void workflowContextChanged(const QString& key);

    /**
     * @brief Emitted when a workflow context is cleared.
     * @param key The workflow identifier.
     */
    void workflowContextCleared(const QString& key);

private:
    mutable QMutex m_mutex;
    std::shared_ptr<Etrek::Context::ISessionContext> m_sessionContext;
    QMap<QString, std::shared_ptr<Etrek::Context::IWorkflowContext>> m_workflowContexts;
};

} // namespace Etrek::Application::Context

#endif // CONTEXTMANAGER_H
