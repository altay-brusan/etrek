#ifndef IPAGEACTION_H
#define IPAGEACTION_H

/**
 * @interface IPageAction
 * @brief Interface for page-level actions in the application.
 *
 * In the system, a "page" represents a logical boundary that groups
 * a collection of delegates and their associated views, focusing on
 * a specific aspect of the project. Examples of pages include:
 * - System Settings
 * - Examination
 * - Output
 * - Worklist
 *
 * Pages manage widgets, the connections between them, and object
 * lifetimes. When changes occur within a page—such as creating a new
 * page or closing an existing one—the delegates within that page
 * may need to respond. This interface provides a standardized way
 * for delegates to react to page-level events.
 */
class IPageAction {

public:
    virtual ~IPageAction() = default;

    /**
     * @brief Triggered when the "Apply" action is invoked on the page.
     *
     * Delegates should implement this method to handle intermediate
     * changes that should be applied but not necessarily finalized.
     */
    virtual void apply() = 0;

    /**
     * @brief Triggered when the "Accept" action is invoked on the page.
     *
     * Delegates should implement this method to finalize changes
     * and persist them if required.
     */
    virtual void accept() = 0;

    /**
     * @brief Triggered when the "Reject" action is invoked on the page.
     *
     * Delegates should implement this method to discard changes
     * or reset their state to the last saved version.
     */
    virtual void reject() = 0;
};

/**
 * @brief Declares the interface to Qt's meta-object system.
 *
 * Allows this interface to be recognized and used by Qt's
 * Q_DECLARE_INTERFACE mechanism for QObject-based page delegates.
 */
Q_DECLARE_INTERFACE(IPageAction, "com.etrek.IPageAction/1.0")

#endif // !IPAGEACTION_H
