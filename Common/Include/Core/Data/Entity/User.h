/**
 * @file User.h
 * @brief User entity representing system users.
 *
 * @details Defines the User entity class used for authentication
 *          and authorization throughout the application.
 *
 * @author Etrek Development Team
 * @date 2026-01-11
 *
 * @copyright Copyright (c) 2024-2026 Etrek Medical Imaging
 *
 * @see Role
 * @see AuthenticationService
 * @see AuthenticationRepository
 */

#ifndef USERENTITYMODEL_H
#define USERENTITYMODEL_H

#include <QString>
#include <QDateTime>
#include <QVector>
#include <QMetaType>
#include "Role.h"

/**
 * @namespace Etrek::Core::Data::Entity
 * @brief Contains core entity classes for the application.
 */
namespace Etrek::Core::Data::Entity {

    /**
     * @class User
     * @brief Represents a system user for authentication and authorization.
     *
     * @details Entity class representing a user account in the system.
     *          Users are authenticated via username and password, and
     *          authorized based on their assigned roles.
     *
     *          Key features:
     *          - Password stored as hash (via CryptoManager)
     *          - Soft-delete support (IsDeleted flag)
     *          - Active/inactive status
     *          - Multiple role assignments
     *          - Audit timestamps (CreateDate, UpdateDate)
     *
     * @see Role
     * @see AuthenticationService
     * @see AuthenticationRepository
     *
     * @ingroup Core
     */
    class User{
    public:
        int Id = -1;              ///< Unique user identifier (-1 for new users).
        QString Username;         ///< Login username (unique).
        QString Name;             ///< User's first name.
        QString Surname;          ///< User's last name.
        QString PasswordHash;     ///< Hashed password (never plaintext).
        bool IsActive = true;     ///< Whether user can log in.
        bool IsDeleted = false;   ///< Soft-delete flag.
        QDateTime CreateDate;     ///< When user was created.
        QDateTime UpdateDate;     ///< Last modification timestamp.
        QVector<Role> Roles;      ///< Assigned roles for authorization.

        /**
         * @brief Default constructor.
         */
        User() = default;

        /**
         * @brief Equality operator based on Id.
         */
        bool operator==(const User& other) const noexcept { return Id == other.Id; }

        /**
         * @brief Inequality operator.
         */
        bool operator!=(const User& other) const noexcept { return !(*this == other); }

    };

}

Q_DECLARE_METATYPE(Etrek::Core::Data::Entity::User)
#endif // USERENTITYMODEL_H
