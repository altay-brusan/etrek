#include "SessionContext.h"
#include <QSysInfo>

namespace Etrek::Application::Context {

SessionContext::SessionContext(
    const Etrek::Core::Data::Entity::User& user,
    const QString& workstationName,
    const std::optional<Etrek::Device::Data::Entity::Institution>& institution)
    : m_user(user)
    , m_workstationName(workstationName.isEmpty() ? QSysInfo::machineHostName() : workstationName)
    , m_institution(institution)
    , m_signInTime(QDateTime::currentDateTime())
    , m_timestamp(QDateTime::currentDateTime())
{
}

// --- IContext Implementation ---

bool SessionContext::isValid() const
{
    return m_user.Id >= 0 && !m_user.Username.isEmpty();
}

QDateTime SessionContext::timestamp() const
{
    return m_timestamp;
}

// --- ISessionContext Implementation ---

std::optional<Etrek::Core::Data::Entity::User> SessionContext::currentUser() const
{
    if (m_user.Id >= 0) {
        return m_user;
    }
    return std::nullopt;
}

QString SessionContext::username() const
{
    return m_user.Username;
}

QString SessionContext::userFullName() const
{
    QString fullName = m_user.Name;
    if (!m_user.Surname.isEmpty()) {
        if (!fullName.isEmpty()) {
            fullName += " ";
        }
        fullName += m_user.Surname;
    }
    return fullName;
}

QDateTime SessionContext::signInTime() const
{
    return m_signInTime;
}

QString SessionContext::workstationName() const
{
    return m_workstationName;
}

std::optional<Etrek::Device::Data::Entity::Institution> SessionContext::institution() const
{
    return m_institution;
}

QString SessionContext::institutionName() const
{
    if (m_institution.has_value()) {
        return m_institution->Name;
    }
    return QString();
}

bool SessionContext::isLoggedIn() const
{
    return isValid();
}

// --- Setters ---

void SessionContext::setInstitution(const Etrek::Device::Data::Entity::Institution& institution)
{
    m_institution = institution;
    m_timestamp = QDateTime::currentDateTime();
}

void SessionContext::setWorkstationName(const QString& name)
{
    m_workstationName = name;
    m_timestamp = QDateTime::currentDateTime();
}

} // namespace Etrek::Application::Context
