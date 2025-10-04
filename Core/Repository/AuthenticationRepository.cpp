// AuthenticationRepository.cpp
#include "AuthenticationRepository.h"
#include <QSqlQuery>
#include <QSqlError>
#include <QDebug>
#include "AppLoggerFactory.h"
#include "MessageKey.h"

namespace Etrek::Core::Repository {

    using Etrek::Specification::Result;

    AuthenticationRepository::AuthenticationRepository(std::shared_ptr<DatabaseConnectionSetting> connectionSetting, TranslationProvider* tr)
        : m_connectionSetting(std::move(connectionSetting)),
        translator(tr ? tr : &TranslationProvider::Instance()),
        logger(AppLoggerFactory(LoggerProvider::Instance(), (tr ? tr : &TranslationProvider::Instance()))
            .CreateLogger("AuthenticationRepository"))
        {

        //translator = &TranslationProvider::Instance();
        //AppLoggerFactory factory(LoggerProvider::Instance(), translator);
        //logger = factory.CreateLogger("WorklistRepository");
    }

    QSqlDatabase AuthenticationRepository::createConnection(const QString& connectionName) const {
        QSqlDatabase db = QSqlDatabase::addDatabase("QMYSQL", connectionName);
        db.setHostName(m_connectionSetting->getHostName());
        db.setDatabaseName(m_connectionSetting->getDatabaseName());
        db.setUserName(m_connectionSetting->getEtrekUserName());
        db.setPassword(m_connectionSetting->getPassword());
        db.setPort(m_connectionSetting->getPort());
        return db;
    }

    Result<User> AuthenticationRepository::createUser(Entity::User& user)
    {
        int newId = -1;
        QString connectionName = "auth_connection_create_user";

        {
            QSqlDatabase db = createConnection(connectionName);
            if (!db.open()) {
                QString error = translator->getErrorMessage(FAILED_TO_OPEN_DB_MSG).arg(db.lastError().text());
                logger->LogError(error);
                qDebug() << error;				
                return Result<User>::Failure(error);
            }

            QSqlQuery checkQuery(db);
            checkQuery.prepare("SELECT COUNT(*) FROM users WHERE user_name = ?");
            checkQuery.addBindValue(user.Username);
            if (!checkQuery.exec()) {
				QString error = translator->getErrorMessage(AUTH_CHECK_USER_EXISTS_FAILED_MSG).arg(checkQuery.lastError().text());
                logger->LogError(error);
                qDebug() << error;
                return Result<User>::Failure(error);
            }

            if (checkQuery.next() && checkQuery.value(0).toInt() > 0) {
                QString info = translator->getErrorMessage(AUTH_USERNAME_ALREADY_EXISTS_MSG).arg(checkQuery.lastError().text());
                logger->LogInfo(info);
                qInfo() << info;
                return Result<User>::Failure(info);
            }

            QSqlQuery insertQuery(db);
            insertQuery.prepare(R"(
            INSERT INTO users (user_name, name, surname, password_hash)
            VALUES (?, ?, ?, ?)
            )");
            insertQuery.addBindValue(user.Username);
            insertQuery.addBindValue(user.Name);
            insertQuery.addBindValue(user.Surname);
            insertQuery.addBindValue(user.PasswordHash);

            if (!insertQuery.exec()) {
                QString error = translator->getErrorMessage(AUTH_FAILED_TO_CREATE_USER_ERROR_MSG).arg(insertQuery.lastError().text());
                logger->LogError(error);
                qDebug() << error;
                return Result<User>::Failure(error);
            }

            newId = insertQuery.lastInsertId().toInt();

            for (const auto& role : user.Roles) {
                if (!roleExists(db, role.Id)) {
                    QString error = translator->getErrorMessage(AUTH_INVALID_ROLE_ID_ERROR_MSG).arg(QString::number(role.Id));
                    logger->LogError(error);
                    qDebug() << error;
                    return Result<User>::Failure(error);
                }

                QSqlQuery roleQuery(db);
                roleQuery.prepare("INSERT INTO user_roles (user_id, role_id) VALUES (?, ?)");
                roleQuery.addBindValue(newId);
                roleQuery.addBindValue(role.Id);
                if (!roleQuery.exec()) {
                    QString error = translator->getErrorMessage(AUTH_ROLE_ASSIGNMENT_FAILED_ERROR_MSG).arg(roleQuery.lastError().text());
                    logger->LogError(error);
                    qDebug() << error;
                    return Result<User>::Failure(error);
                }
            }
        }

        QSqlDatabase::removeDatabase(connectionName);

        user.Id = newId;

        return Result<User>::Success(user);
    }

    Result<User> AuthenticationRepository::updateUser(Entity::User& user)
    {
        QString connectionName = "auth_connection_update_user";

        {
            QSqlDatabase db = createConnection(connectionName);
            if (!db.open()) {
                QString error = translator->getErrorMessage(FAILED_TO_OPEN_DB_MSG).arg(db.lastError().text());
                logger->LogError(error);
                qDebug() << error;
				return Result<User>::Failure(error);            
            }

            // 1. Ensure the user with given ID exists
            QSqlQuery checkIdQuery(db);
            checkIdQuery.prepare("SELECT COUNT(*) FROM users WHERE id = ?");
            checkIdQuery.addBindValue(user.Id);
            
            if (!checkIdQuery.exec() || !checkIdQuery.next() || checkIdQuery.value(0).toInt() == 0) {

                QString error = translator->getErrorMessage(AUTH_USER_NOT_EXISTS_ERROR);
                logger->LogError(error);
                qDebug() << error;
                return Result<User>::Failure(error);
            
            }

            // 2. Check if the new Username is already taken by another user
            QSqlQuery checkUsernameQuery(db);
            checkUsernameQuery.prepare("SELECT COUNT(*) FROM users WHERE user_name = ? AND id != ?");
            checkUsernameQuery.addBindValue(user.Username);
            checkUsernameQuery.addBindValue(user.Id);
            
            if (!checkUsernameQuery.exec()) {

                QString error = translator->getErrorMessage(AUTH_CHECK_USERNAME_FAILED_ERROR).arg(checkUsernameQuery.lastError().text());
                logger->LogError(error);
                qDebug() << error;
                return Result<User>::Failure(error);
            }

            if (checkUsernameQuery.next() && checkUsernameQuery.value(0).toInt() > 0) {

                QString warning = translator->getWarningMessage(AUTH_USER_ALREADY_EXISTS_WARNING_MSG);
                logger->LogError(warning);
                qDebug() << warning;
                return Result<User>::Failure(warning);
            }

            // 3. Perform the update
            QSqlQuery updateQuery(db);
            updateQuery.prepare(R"(
            UPDATE users
            SET user_name = ?, name = ?, surname = ?, password_hash = ?, is_active = ?, update_date = NOW()
            WHERE id = ?
            )");
            updateQuery.addBindValue(user.Username);
            updateQuery.addBindValue(user.Name);
            updateQuery.addBindValue(user.Surname);
            updateQuery.addBindValue(user.PasswordHash);
            updateQuery.addBindValue(user.IsActive);
            updateQuery.addBindValue(user.Id);

            if (!updateQuery.exec()) {

                QString error = translator->getErrorMessage(AUTH_FAILED_TO_UPDATE_USER_ERROR_MSG).arg(updateQuery.lastError().text());
                logger->LogError(error);
                qDebug() << error;

                return Result<User>::Failure(error);
            }

            // 4. Remove and reassign roles
            QSqlQuery deleteRolesQuery(db);
            deleteRolesQuery.prepare("DELETE FROM user_roles WHERE user_id = ?");
            deleteRolesQuery.addBindValue(user.Id);
            
            if (!deleteRolesQuery.exec()) {
                QString error = translator->getErrorMessage(AUTH_CLEAR_EXISTING_ROLES_FAILED_ERROR_MSG).arg(deleteRolesQuery.lastError().text());
                logger->LogError(error);
                qDebug() << error;
                return Result<User>::Failure(error);
            }

            for (const auto& role : user.Roles) {
                if (!roleExists(db, role.Id)) {
                    QString error = translator->getErrorMessage(AUTH_INVALID_ROLE_ID_ERROR_MSG).arg(QString::number(role.Id));
                    logger->LogError(error);
                    qDebug() << error;
                    return Result<User>::Failure(error);
                }

                QSqlQuery insertRoleQuery(db);
                insertRoleQuery.prepare("INSERT INTO user_roles (user_id, role_id) VALUES (?, ?)");
                insertRoleQuery.addBindValue(user.Id);
                insertRoleQuery.addBindValue(role.Id);

                if (!insertRoleQuery.exec()) {
                    QString error = translator->getErrorMessage(AUTH_ROLE_ASSIGNMENT_FAILED_ERROR_MSG).arg(insertRoleQuery.lastError().text());
                    logger->LogError(error);
                    qDebug() << error;
                    return Result<User>::Failure("Role assignment failed: " + insertRoleQuery.lastError().text());
                }
            }
        }

        QSqlDatabase::removeDatabase(connectionName);
        return Result<User>::Success(user);
    }

    Result<User> AuthenticationRepository::deleteUser(Entity::User& user)
    {
        QString connectionName = "auth_connection_delete_user";

        {
            QSqlDatabase db = createConnection(connectionName);
            if (!db.open()) {
                return Result<User>::Failure("Failed to open DB: " + db.lastError().text());
            }

            // 1. Check if user exists
            QSqlQuery checkQuery(db);
            checkQuery.prepare("SELECT COUNT(*) FROM users WHERE id = ?");
            checkQuery.addBindValue(user.Id);

            if (!checkQuery.exec() || !checkQuery.next() || checkQuery.value(0).toInt() == 0) {
                return Result<User>::Failure("User does not exist.");
            }

            // 2. Mark user as deleted
            QSqlQuery deleteQuery(db);
            deleteQuery.prepare(R"(
            UPDATE users
            SET is_deleted = TRUE, update_date = NOW()
            WHERE id = ?
            )");
            deleteQuery.addBindValue(user.Id);

            if (!deleteQuery.exec()) {
                return Result<User>::Failure("Delete user failed: " + deleteQuery.lastError().text());
            }

            // 3. Optionally update the user model for return
            user.IsDeleted = true;
            user.UpdateDate = QDateTime::currentDateTime();
        }

        QSqlDatabase::removeDatabase(connectionName);
        return Result<User>::Success(user);
    }




    Result<int> AuthenticationRepository::createRole(const QString& roleName) {
        QString connectionName = "auth_connection_create_role";
        int newId = -1;

        {
            QSqlDatabase db = createConnection(connectionName);
            if (!db.open()) {
                QString error = translator->getErrorMessage(FAILED_TO_OPEN_DB_MSG).arg(db.lastError().text());
                logger->LogError(error);
                qDebug() << error;
                return Result<int>::Failure(error);
            }

            QSqlQuery query(db);
            query.prepare("INSERT INTO roles (name) VALUES (?)");
            query.addBindValue(roleName);

            if (!query.exec()) {
                QString error = translator->getErrorMessage(AUTH_FAILED_TO_CREATE_ROLE_ERROR_MSG).arg(query.lastError().text());
                logger->LogError(error);
                qDebug() << error;
                return Result<int>::Failure(error);
            }

            newId = query.lastInsertId().toInt();
        }

        QSqlDatabase::removeDatabase(connectionName);
        return Result<int>::Success(newId);
    }

    Result<QString> AuthenticationRepository::assignRoleToUser(int userId, int roleId) {
        QString connectionName = "auth_connection_assign_role";

        {
            QSqlDatabase db = createConnection(connectionName);
            if (!db.open()) {
                QString error = translator->getErrorMessage(FAILED_TO_OPEN_DB_MSG).arg(db.lastError().text());
                logger->LogError(error);
                qDebug() << error;
                return Result<QString>::Failure(error);
            }

            QSqlQuery query(db);
            query.prepare("INSERT INTO user_roles (user_id, role_id) VALUES (?, ?)");
            query.addBindValue(userId);
            query.addBindValue(roleId);

            if (!query.exec()) {
                QString error = translator->getErrorMessage(AUTH_ROLE_ASSIGNMENT_FAILED_ERROR_MSG).arg(query.lastError().text());
                logger->LogError(error);
                qDebug() << error;
                return Result<QString>::Failure(error);
            }

        }

        QSqlDatabase::removeDatabase(connectionName);

        QString info = translator->getInfoMessage(AUTH_ROLE_ASSIGNMENT_SUCCEED_MSG);

        return Result<QString>::Success(info);
    }

    Result<QString> AuthenticationRepository::removeRoleFromUser(int userId, int roleId) {
        QString connectionName = "auth_connection_remove_role";

        {
            QSqlDatabase db = createConnection(connectionName);
            if (!db.open()) {
                QString error = translator->getErrorMessage(FAILED_TO_OPEN_DB_MSG).arg(db.lastError().text());
                logger->LogError(error);
                qDebug() << error;
                return Result<QString>::Failure(error);                
            }

            QSqlQuery query(db);
            query.prepare("DELETE FROM user_roles WHERE user_id = ? AND role_id = ?");
            query.addBindValue(userId);
            query.addBindValue(roleId);

            if (!query.exec()) {
				QString error = translator->getErrorMessage(AUTH_FAILED_TO_REMOVE_ROLE_ERROR_MSG).arg(query.lastError().text());
                logger->LogError(error);
                qDebug() << error;
                return Result<QString>::Failure(error);
            }
        }

        QSqlDatabase::removeDatabase(connectionName);
        return Result<QString>::Success("Role removed successfully.");
    }

    Result<bool> AuthenticationRepository::checkUserHasRole(int userId, const QString& roleName) {
        QString connectionName = "auth_connection_check_role";

        bool ok = false;
        bool hasRole = false;
        QString err;

        {
            QSqlDatabase db = createConnection(connectionName);
            if (!db.open()) {
                err = translator->getErrorMessage(FAILED_TO_OPEN_DB_MSG).arg(db.lastError().text());
            }
            else {
                QSqlQuery query(db);
                query.prepare(R"(
                SELECT COUNT(*)
                FROM user_roles ur
                JOIN roles r ON ur.role_id = r.id
                WHERE ur.user_id = ? AND r.name = ?
            )");
                query.addBindValue(userId);
                query.addBindValue(roleName);

                if (!query.exec()) {
                    err = translator->getErrorMessage(AUTH_CHECK_ROLE_FAILED_ERROR_MSG).arg(query.lastError().text());
                }
                else if (query.next()) {
                    hasRole = query.value(0).toInt() > 0;
                    ok = true;
                }
            }
        }

        QSqlDatabase::removeDatabase(connectionName);
        if (!ok) return Result<bool>::Failure(err.isEmpty()
            ? translator->getCriticalMessage(UNEXPECTED_ERROR_OCCURED_ERROR_MSG) : err);
        return Result<bool>::Success(hasRole);
    }



    Result<QVector<User>> AuthenticationRepository::getAllActiveUsers() const {
        QVector<User> users;
        QString connectionName = "auth_connection_list_active_users";

        {
            QSqlDatabase db = createConnection(connectionName);
            if (!db.open()) {
                QString error = translator->getErrorMessage(FAILED_TO_OPEN_DB_MSG).arg(db.lastError().text());
                logger->LogError(error);
                qDebug() << error;
                return Result<QVector<User>>::Failure(error);
            }

            QSqlQuery query(db);
            query.prepare(R"(
            SELECT id, user_name, name, surname, password_hash, is_active, is_deleted, create_date, update_date
            FROM users
            WHERE is_active = TRUE AND (is_deleted IS NULL OR is_deleted = FALSE)
            )");

            if (!query.exec()) {
				QString error = translator->getCriticalMessage(QUERY_FAILED_ERROR_MSG).arg(query.lastError().text());
                logger->LogError(error);
                qDebug() << error;
                return Result<QVector<User>>::Failure(error);
            }

            while (query.next()) {
                User user;
                user.Id = query.value("id").toInt();
                user.Username = query.value("user_name").toString();
                user.Name = query.value("name").toString();
                user.Surname = query.value("surname").toString();
                user.PasswordHash = query.value("password_hash").toString();
                user.IsActive = query.value("is_active").toBool();
                user.IsDeleted = query.value("is_deleted").toBool();
                user.CreateDate = query.value("create_date").toDateTime();
                user.UpdateDate = query.value("update_date").toDateTime();

                // Load roles
                auto roleResult = getUserRoles(user.Id, db);  // reuse same connection
                if (roleResult.isSuccess) {
                    user.Roles = roleResult.value;
                }

                users.append(user);
            }
        }

        QSqlDatabase::removeDatabase(connectionName);
        return Result<QVector<User>>::Success(users);
    }


    Result<QVector<Role>> AuthenticationRepository::getUserRoles(int userId, QSqlDatabase& db) const {
        QVector<Role> roles;

        if (!db.isOpen()) {
            QString error = translator->getErrorMessage(FAILED_TO_OPEN_DB_MSG).arg(db.lastError().text());
            logger->LogError(error);
            qDebug() << error;
            return Result<QVector<Role>>::Failure(error);
        }

        QSqlQuery query(db);
        query.prepare(R"(
        SELECT r.id, r.name
        FROM roles r
        INNER JOIN user_roles ur ON r.id = ur.role_id
        WHERE ur.user_id = ?
        )");

        query.addBindValue(userId);

        if (!query.exec()) {
            QString error = translator->getCriticalMessage(QUERY_FAILED_ERROR_MSG).arg(query.lastError().text());
            logger->LogError(error);
            qDebug() << error;
            return Result<QVector<Role>>::Failure(error);           
        }

        while (query.next()) {
            Role role;
            role.Id = query.value("id").toInt();
            role.Name = query.value("name").toString();
            roles.append(role);
        }

        return Result<QVector<Role>>::Success(roles);
    }



    Result<QVector<User>> AuthenticationRepository::getAllInactiveUsers() const {
        QVector<User> users;
        QString connectionName = "auth_connection_list_inactive_users";

        {
            QSqlDatabase db = createConnection(connectionName);
            if (!db.open()) {
                QString error = translator->getErrorMessage(FAILED_TO_OPEN_DB_MSG).arg(db.lastError().text());
                logger->LogError(error);
                qDebug() << error;
                return Result<QVector<User>>::Failure(error);
            }

            QSqlQuery query(db);
            query.prepare(R"(
            SELECT id, user_name, name, surname, password_hash, is_active, is_deleted, create_date, update_date
            FROM users
            WHERE is_active = FALSE OR is_deleted = TRUE
            )");

            if (!query.exec()) {
                QString error = translator->getCriticalMessage(QUERY_FAILED_ERROR_MSG).arg(query.lastError().text());
                logger->LogError(error);
                qDebug() << error;
                return Result<QVector<User>>::Failure(error);
            }

            while (query.next()) {
                User user;
                user.Id = query.value("id").toInt();
                user.Username = query.value("user_name").toString();
                user.Name = query.value("name").toString();
                user.Surname = query.value("surname").toString();
                user.PasswordHash = query.value("password_hash").toString();
                user.IsActive = query.value("is_active").toBool();
                user.IsDeleted = query.value("is_deleted").toBool();
                user.CreateDate = query.value("create_date").toDateTime();
                user.UpdateDate = query.value("update_date").toDateTime();

                // Load roles
                auto roleResult = getUserRoles(user.Id, db);  // reuse same connection
                if (roleResult.isSuccess) {
                    user.Roles = roleResult.value;
                }

                users.append(user);
            }
        }

        QSqlDatabase::removeDatabase(connectionName);
        return Result<QVector<User>>::Success(users);
    }

    Result<QVector<Entity::Role>> AuthenticationRepository::getAllRoles() const {
        QVector<Entity::Role> roles;
        QString connectionName = "auth_connection_get_all_roles";

        QString err;
        bool ok = false;

        {
            QSqlDatabase db = createConnection(connectionName);
            if (!db.open()) {
                err = translator->getErrorMessage(FAILED_TO_OPEN_DB_MSG).arg(db.lastError().text());
            }
            else {
                QSqlQuery query(db);
                if (!query.exec("SELECT id, name FROM roles")) {
                    err = translator->getCriticalMessage(QUERY_FAILED_ERROR_MSG).arg(query.lastError().text());
                }
                else {
                    while (query.next()) {
                        Entity::Role role;
                        role.Id = query.value("id").toInt();
                        role.Name = query.value("name").toString();
                        roles.append(role);
                    }
                    ok = true;
                }
            }
        }

        QSqlDatabase::removeDatabase(connectionName);
        if (!ok) return Result<QVector<Entity::Role>>::Failure(err);
        return Result<QVector<Entity::Role>>::Success(roles);
    }



    Result<User> AuthenticationRepository::getUser(const QString& username) const {
        QString connectionName = "auth_connection_get_user";

        QString err;
        bool ok = false;
        User user;

        {
            QSqlDatabase db = createConnection(connectionName);
            if (!db.open()) {
                err = translator->getErrorMessage(FAILED_TO_OPEN_DB_MSG).arg(db.lastError().text());
            }
            else {
                QSqlQuery query(db);
                query.prepare(R"(
                SELECT id, user_name, password_hash, is_active, is_deleted, create_date, update_date
                FROM users
                WHERE user_name = ?
            )");
                query.addBindValue(username);

                if (!query.exec()) {
                    err = translator->getErrorMessage(AUTH_GET_USER_FAILED_ERROR_MSG).arg(query.lastError().text());
                }
                else if (query.next()) {
                    user.Id = query.value("id").toInt();
                    user.Username = query.value("user_name").toString();
                    user.PasswordHash = query.value("password_hash").toString();
                    user.IsActive = query.value("is_active").toBool();
                    user.IsDeleted = query.value("is_deleted").toBool();
                    user.CreateDate = query.value("create_date").toDateTime();
                    user.UpdateDate = query.value("update_date").toDateTime();

                    // load roles
                    QSqlQuery roleQuery(db);
                    roleQuery.prepare(R"(
                    SELECT r.id, r.name
                    FROM user_roles ur
                    JOIN roles r ON r.id = ur.role_id
                    WHERE ur.user_id = ?
                )");
                    roleQuery.addBindValue(user.Id);
                    if (roleQuery.exec()) {
                        while (roleQuery.next()) {
                            Role role;
                            role.Id = roleQuery.value("id").toInt();
                            role.Name = roleQuery.value("name").toString();
                            user.Roles.append(role);
                        }
                    }
                    ok = true;
                }
                else {
                    err = translator->getErrorMessage(AUTH_USER_NOT_EXISTS_ERROR);
                }
            }
        }

        QSqlDatabase::removeDatabase(connectionName);
        if (!ok) return Result<User>::Failure(err);
        return Result<User>::Success(user);
    }


    bool AuthenticationRepository::roleExists(QSqlDatabase& db, int roleId) {
        QSqlQuery checkQuery(db);
        checkQuery.prepare("SELECT COUNT(*) FROM roles WHERE id = ?");
        checkQuery.addBindValue(roleId);
        return checkQuery.exec() && checkQuery.next() && checkQuery.value(0).toInt() > 0;
    }

} // namespace Etrek::Repository
