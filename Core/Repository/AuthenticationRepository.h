#ifndef AUTHENTICATIONREPOSITORY_H
#define AUTHENTICATIONREPOSITORY_H

#include <QString>
#include <QStringList>
#include <QVariantMap>
#include <QSqlDatabase>
#include <memory>

#include "DatabaseConnectionSetting.h"
#include "TranslationProvider.h"
#include "AppLogger.h"
#include "Result.h"
#include "User.h"

namespace Etrek::Core::Repository {

    /**
     * @class AuthenticationRepository
     * @brief Provides methods for user authentication and role management.
     *
     * This class interacts with the database to perform operations related to user authentication,
     * role assignment, and user management. It includes methods for retrieving users, creating,
     * updating, and deleting users, as well as managing roles.
     */
    class AuthenticationRepository {
    public:
        /**
         * @brief Constructs the AuthenticationRepository with a database connection setting and translation provider.
         * @param connectionSetting Shared pointer to the database connection settings.
         * @param translator Pointer to the translation provider for localized messages.
         */
        explicit AuthenticationRepository(std::shared_ptr<Etrek::Core::Data::Model::DatabaseConnectionSetting> connectionSetting,
                                         Etrek::Core::Globalization::TranslationProvider* translator);

        /**
         * @brief Retrieves a user by username.
         * @param username The username to search for.
         * @return Result containing the User if found, or an error message.
         */
        Etrek::Specification::Result<Etrek::Core::Data::Entity::User> getUser(const QString& username) const;

        /**
         * @brief Retrieves all roles in the system.
         * @return Result containing a QVector of all roles, or an error message.
         */
        Etrek::Specification::Result<QVector<Etrek::Core::Data::Entity::Role>> getAllRoles() const;

        /**
         * @brief Retrieves all active users.
         * @return Result containing a QVector of all active users, or an error message.
         */
        Etrek::Specification::Result<QVector<Etrek::Core::Data::Entity::User>> getAllActiveUsers() const;

        /**
         * @brief Retrieves all inactive or deleted users.
         * @return Result containing a QVector of all inactive users, or an error message.
         */
        Etrek::Specification::Result<QVector<Etrek::Core::Data::Entity::User>> getAllInactiveUsers() const;

        /**
         * @brief Creates a new user in the database.
         * @param user Reference to the user entity to create.
         * @return Result containing the created User, or an error message.
         */
        Etrek::Specification::Result<Etrek::Core::Data::Entity::User> createUser(Etrek::Core::Data::Entity::User& user);

        /**
         * @brief Updates an existing user in the database.
         * @param user Reference to the user entity to update.
         * @return Result containing the updated User, or an error message.
         */
        Etrek::Specification::Result<Etrek::Core::Data::Entity::User> updateUser(Etrek::Core::Data::Entity::User& user);

        /**
         * @brief Deletes (marks as deleted) a user in the database.
         * @param user Reference to the user entity to delete.
         * @return Result containing the deleted User, or an error message.
         */
        Etrek::Specification::Result<Etrek::Core::Data::Entity::User> deleteUser(Etrek::Core::Data::Entity::User& user);

        /**
         * @brief Creates a new role in the database.
         * @param roleName The name of the role to create.
         * @return Result containing the new role's ID, or an error message.
         */
        Etrek::Specification::Result<int> createRole(const QString& roleName);

        /**
         * @brief Assigns a role to a user.
         * @param userId The ID of the user.
         * @param roleId The ID of the role.
         * @return Result containing a success message, or an error message.
         */
        Etrek::Specification::Result<QString> assignRoleToUser(int userId, int roleId);

        /**
         * @brief Removes a role from a user.
         * @param userId The ID of the user.
         * @param roleId The ID of the role.
         * @return Result containing a success message, or an error message.
         */
        Etrek::Specification::Result<QString> removeRoleFromUser(int userId, int roleId);

        /**
         * @brief Checks if a user has a specific role.
         * @param userId The ID of the user.
         * @param roleName The name of the role.
         * @return Result containing true if the user has the role, false otherwise, or an error message.
         */
        Etrek::Specification::Result<bool> checkUserHasRole(int userId, const QString& roleName);

    private:
        /**
         * @brief Creates a new database connection with the specified name.
         * @param connectionName The name for the database connection.
         * @return The QSqlDatabase object for the connection.
         */
        QSqlDatabase createConnection(const QString& connectionName) const;

        /**
         * @brief Retrieves all roles assigned to a user.
         * @param userId The ID of the user.
         * @param db Reference to an open QSqlDatabase connection.
         * @return Result containing a QVector of roles, or an error message.
         */
        Etrek::Specification::Result<QVector<Etrek::Core::Data::Entity::Role>> getUserRoles(int userId, QSqlDatabase& db) const;

        /**
         * @brief Checks if a role exists in the database.
         * @param db Reference to an open QSqlDatabase connection.
         * @param roleId The ID of the role to check.
         * @return True if the role exists, false otherwise.
         */
        bool roleExists(QSqlDatabase& db, int roleId);

        /**
         * @brief Database connection settings.
         */
        std::shared_ptr<Etrek::Core::Data::Model::DatabaseConnectionSetting> m_connectionSetting;

        /**
         * @brief Pointer to the translation provider for localized messages (non-owning).
         */
        Etrek::Core::Globalization::TranslationProvider* translator;

        /**
         * @brief Shared pointer to the application logger.
         */
        std::shared_ptr<Etrek::Core::Log::AppLogger> logger;
    };

} // namespace Etrek::Repository

#endif // AUTHENTICATIONREPOSITORY_H
