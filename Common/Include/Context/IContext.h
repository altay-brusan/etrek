#ifndef ICONTEXT_H
#define ICONTEXT_H

#include <QString>
#include <QDateTime>

namespace Etrek::Context {

/**
 * @interface IContext
 * @brief Base interface for all context types in the system.
 *
 * Contexts represent shared state that needs to be passed between
 * different parts of the application without creating tight coupling.
 * Examples include session context (user info, login time) and
 * workflow context (selected worklist item for examination).
 */
class IContext {
public:
    virtual ~IContext() = default;

    /**
     * @brief Returns the unique type identifier for this context.
     * @return A string identifying the context type (e.g., "Session", "Examination").
     */
    virtual QString contextType() const = 0;

    /**
     * @brief Checks if the context contains valid data.
     * @return true if the context is valid and usable; false otherwise.
     */
    virtual bool isValid() const = 0;

    /**
     * @brief Returns the timestamp when this context was created or last updated.
     * @return The context's timestamp.
     */
    virtual QDateTime timestamp() const = 0;
};

} // namespace Etrek::Context

#endif // ICONTEXT_H
