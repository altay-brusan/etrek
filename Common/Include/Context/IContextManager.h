#ifndef ICONTEXTMANAGER_H
#define ICONTEXTMANAGER_H

#include <memory>
#include <QString>

namespace Etrek::Context {

class IContext;
class ISessionContext;
class IWorkflowContext;

/**
 * @interface IContextManager
 * @brief Interface for the centralized context management service.
 *
 * The ContextManager is responsible for:
 * - Maintaining session context (user, sign-in time, workstation, institution)
 * - Maintaining workflow contexts (e.g., current examination context)
 * - Providing access to contexts for consumers (delegates, services)
 *
 * Contexts are accessed through this interface to avoid tight coupling
 * between producers and consumers. Builders fetch contexts from the
 * ContextManager and inject them into delegates via DelegateParameter.
 *
 * Usage:
 * - Producers call setSessionContext() or setWorkflowContext() to register context
 * - Consumers receive context via DelegateParameter injection from builders
 * - Direct access via getSessionContext() or getWorkflowContext() is also available
 */
class IContextManager {
public:
    virtual ~IContextManager() = default;

    // --- Session Context Management ---

    /**
     * @brief Sets the session context.
     * @param context The session context to store.
     */
    virtual void setSessionContext(std::shared_ptr<ISessionContext> context) = 0;

    /**
     * @brief Returns the current session context.
     * @return Shared pointer to the session context, or nullptr if not set.
     */
    virtual std::shared_ptr<ISessionContext> sessionContext() const = 0;

    /**
     * @brief Clears the session context (e.g., on logout).
     */
    virtual void clearSessionContext() = 0;

    // --- Workflow Context Management ---

    /**
     * @brief Sets a workflow context with a specific key.
     * @param key The workflow identifier (e.g., "Examination").
     * @param context The workflow context to store.
     */
    virtual void setWorkflowContext(const QString& key, std::shared_ptr<IWorkflowContext> context) = 0;

    /**
     * @brief Returns a workflow context by key.
     * @param key The workflow identifier.
     * @return Shared pointer to the workflow context, or nullptr if not found.
     */
    virtual std::shared_ptr<IWorkflowContext> workflowContext(const QString& key) const = 0;

    /**
     * @brief Clears a specific workflow context.
     * @param key The workflow identifier to clear.
     */
    virtual void clearWorkflowContext(const QString& key) = 0;

    /**
     * @brief Clears all workflow contexts.
     */
    virtual void clearAllWorkflowContexts() = 0;

    // --- Utility Methods ---

    /**
     * @brief Checks if a session context is available.
     * @return true if a session context is set; false otherwise.
     */
    virtual bool hasSessionContext() const = 0;

    /**
     * @brief Checks if a specific workflow context is available.
     * @param key The workflow identifier.
     * @return true if the workflow context exists; false otherwise.
     */
    virtual bool hasWorkflowContext(const QString& key) const = 0;
};

} // namespace Etrek::Context

#endif // ICONTEXTMANAGER_H
