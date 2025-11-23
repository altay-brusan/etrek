#ifndef IWORKFLOWCONTEXT_H
#define IWORKFLOWCONTEXT_H

#include "IContext.h"
#include <QString>

namespace Etrek::Context {

/**
 * @interface IWorkflowContext
 * @brief Base interface for workflow-specific context information.
 *
 * Workflow contexts represent transient state that needs to be passed
 * between pages/delegates during a specific workflow (e.g., from
 * Worklist selection to Examination page).
 *
 * Unlike session context which persists for the entire session,
 * workflow contexts are typically short-lived and may be replaced
 * as the user moves through different workflows.
 */
class IWorkflowContext : public IContext {
public:
    ~IWorkflowContext() override = default;

    /**
     * @brief Returns the workflow identifier.
     *
     * Each workflow type should have a unique identifier that can be used
     * to retrieve the correct context from the ContextManager.
     *
     * @return A unique string identifying the workflow (e.g., "Examination", "Report").
     */
    virtual QString workflowId() const = 0;

    /**
     * @brief Checks if the workflow has been completed.
     * @return true if the workflow is complete; false if still in progress.
     */
    virtual bool isComplete() const = 0;

    /**
     * @brief Marks the workflow as complete.
     */
    virtual void markComplete() = 0;
};

} // namespace Etrek::Context

#endif // IWORKFLOWCONTEXT_H
