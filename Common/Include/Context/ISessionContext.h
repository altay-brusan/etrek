#ifndef ISESSIONCONTEXT_H
#define ISESSIONCONTEXT_H

#include "IContext.h"
#include <QString>
#include <QDateTime>
#include <optional>

namespace Etrek::Core::Data::Entity {
    class User;
}

namespace Etrek::Device::Data::Entity {
    class Institution;
}

namespace Etrek::Context {

/**
 * @interface ISessionContext
 * @brief Interface for session/user context information.
 *
 * Provides access to information about the current user session including:
 * - Logged-in user details
 * - Sign-in timestamp
 * - Workstation information
 * - Institution/health center details
 *
 * This context is typically created after successful authentication and
 * remains valid for the duration of the user's session.
 */
class ISessionContext : public IContext {
public:
    ~ISessionContext() override = default;

    /**
     * @brief Returns the context type identifier.
     * @return "Session"
     */
    QString contextType() const override { return QStringLiteral("Session"); }

    /**
     * @brief Returns the currently logged-in user.
     * @return Optional containing the user if logged in; empty otherwise.
     */
    virtual std::optional<Etrek::Core::Data::Entity::User> currentUser() const = 0;

    /**
     * @brief Returns the username of the logged-in user.
     * @return The username string, or empty if not logged in.
     */
    virtual QString username() const = 0;

    /**
     * @brief Returns the full name of the logged-in user.
     * @return The user's full name (Name + Surname).
     */
    virtual QString userFullName() const = 0;

    /**
     * @brief Returns the sign-in timestamp.
     * @return The date and time when the user signed in.
     */
    virtual QDateTime signInTime() const = 0;

    /**
     * @brief Returns the workstation/machine identifier.
     * @return The workstation name or identifier.
     */
    virtual QString workstationName() const = 0;

    /**
     * @brief Returns the current institution/health center.
     * @return Optional containing the institution if set; empty otherwise.
     */
    virtual std::optional<Etrek::Device::Data::Entity::Institution> institution() const = 0;

    /**
     * @brief Returns the institution name.
     * @return The name of the current institution/health center.
     */
    virtual QString institutionName() const = 0;

    /**
     * @brief Checks if a user is currently logged in.
     * @return true if a user is logged in; false otherwise.
     */
    virtual bool isLoggedIn() const = 0;
};

} // namespace Etrek::Context

#endif // ISESSIONCONTEXT_H
