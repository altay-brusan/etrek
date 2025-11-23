#ifndef SESSIONCONTEXT_H
#define SESSIONCONTEXT_H

#include "ISessionContext.h"
#include "User.h"
#include "Entity/Institution.h"
#include <QString>
#include <QDateTime>
#include <optional>

namespace Etrek::Core::Context {

/**
 * @class SessionContext
 * @brief Concrete implementation of session context.
 *
 * Stores information about the current user session including:
 * - Logged-in user details
 * - Sign-in timestamp
 * - Workstation information
 * - Institution/health center details
 */
class SessionContext : public Etrek::Context::ISessionContext {
public:
    /**
     * @brief Constructs a SessionContext with the given user.
     * @param user The logged-in user.
     * @param workstationName The workstation/machine name.
     * @param institution The institution/health center (optional).
     */
    SessionContext(
        const Etrek::Core::Data::Entity::User& user,
        const QString& workstationName,
        const std::optional<Etrek::Device::Data::Entity::Institution>& institution = std::nullopt);

    ~SessionContext() override = default;

    // --- IContext Implementation ---
    bool isValid() const override;
    QDateTime timestamp() const override;

    // --- ISessionContext Implementation ---
    std::optional<Etrek::Core::Data::Entity::User> currentUser() const override;
    QString username() const override;
    QString userFullName() const override;
    QDateTime signInTime() const override;
    QString workstationName() const override;
    std::optional<Etrek::Device::Data::Entity::Institution> institution() const override;
    QString institutionName() const override;
    bool isLoggedIn() const override;

    // --- Setters for updating context ---

    /**
     * @brief Updates the institution information.
     * @param institution The new institution data.
     */
    void setInstitution(const Etrek::Device::Data::Entity::Institution& institution);

    /**
     * @brief Updates the workstation name.
     * @param name The new workstation name.
     */
    void setWorkstationName(const QString& name);

private:
    Etrek::Core::Data::Entity::User m_user;
    QString m_workstationName;
    std::optional<Etrek::Device::Data::Entity::Institution> m_institution;
    QDateTime m_signInTime;
    QDateTime m_timestamp;
};

} // namespace Etrek::Core::Context

#endif // SESSIONCONTEXT_H
