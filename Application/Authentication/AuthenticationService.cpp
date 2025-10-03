#include "AuthenticationService.h"
#include "Result.h"
#include "MessageKey.h"
#include "LoginDialog.h"
#include "Result.h"
#include "AppLoggerFactory.h"
#include "UserMangerDialog.h"
#include "CryptoManager.h"

namespace Etrek::Application::Authentication
{
    AuthenticationService::AuthenticationService(const AuthenticationRepository& repository,
        const CryptoManager& securityService,
        QObject* parent)
        : QObject(parent), m_securityService(securityService), m_repository(repository)
    {
        translator = &TranslationProvider::Instance();
        AppLoggerFactory factory(LoggerProvider::Instance(), translator);
        logger = factory.CreateLogger("DatabaseInitializer");

    }

    void AuthenticationService::displayUserManagerDialog() const
    {
        auto rolesResult = m_repository.getAllRoles();
        auto activeResult = m_repository.getAllActiveUsers();
        auto inactiveResult = m_repository.getAllInactiveUsers();

        if (!rolesResult.isSuccess || !activeResult.isSuccess || !inactiveResult.isSuccess) {
            // handle failure appropriately (show error, log, etc.)
            return;
        }

        const auto& roles = rolesResult.value;
        const auto& activeUsers = activeResult.value;
        const auto& inactiveUsers = inactiveResult.value;
        UserMangerDialog manager(activeUsers, inactiveUsers, roles, translator);

        connect(&manager, &UserMangerDialog::AddUserRequested, this, &AuthenticationService::OnAddUserRequested);
        connect(&manager, &UserMangerDialog::UpdateUserRequested, this, &AuthenticationService::OnUpdateUserRequested);
        connect(&manager, &UserMangerDialog::DeleteUserRequested, this, &AuthenticationService::OnDeleteUserRequested);
        //selectedUser


        connect(this, &AuthenticationService::UserCreatedSuccessfully, &manager, &UserMangerDialog::onUserCreated);
        connect(this, &AuthenticationService::FailedToCreateUser, &manager, &UserMangerDialog::onUserCreationFailed);
        connect(this, &AuthenticationService::UserUpdatedSuccessfully, &manager, &UserMangerDialog::onUserUpdated);
        connect(this, &AuthenticationService::FailedToUpdateUser, &manager, &UserMangerDialog::onUserUpdateFailed);
        connect(this, &AuthenticationService::UserDeletedSuccessfully, &manager, &UserMangerDialog::onUserDeleted);
        connect(this, &AuthenticationService::FailedToDeleteUser, &manager, &UserMangerDialog::onUserDeletionFailed);

        // This will block the execution until the dialog is accepted or rejected.
        if (manager.exec() == QDialog::Accepted) {
            // Handle post-dialog logic when the dialog is accepted
            return;
        }
    }

    Result<std::optional<Entity::User>> AuthenticationService::DisplayLoginDialog() const
    {
        // Get all active users from the repository
        auto activeResult = m_repository.getAllActiveUsers();
        if (!activeResult.isSuccess) {
            return Result<std::optional<Entity::User>>::Failure("Failed to load user list during login.");
        }

        // Create login dialog with active users
        LoginDialog loginDialog(activeResult.value);

        // Loop until the user enters valid credentials or cancels
        while (true) {
            if (loginDialog.exec() == QDialog::Accepted) {
                // Get credentials (username and password)
                std::optional<std::pair<Entity::User, QString>> credentials = loginDialog.getLoginCredentials();

                if (credentials.has_value()) {
                    // Fetch user from the repository
                    auto result = m_repository.getUser(credentials.value().first.Username); // Assuming Etrek::Model::User has Username
                    if (result.isSuccess) {
                        // Compare passwords (assume you have a method for decrypting)
                        auto decryptedPassword = m_securityService.decryptPassword(result.value.PasswordHash);

                        if (QString::compare(credentials.value().second, decryptedPassword, Qt::CaseInsensitive) == 0) {
                            // Success, return the user
                            return Result<std::optional<Entity::User>>::Success(result.value);
                        }
                        else {
                            // If password is incorrect, clear the password field and try again
                            loginDialog.clearPassword();
                            continue;  // Let the user try again
                        }
                    }
                }
            }
            else {
                return Result<std::optional<Entity::User>>::Failure(AUTH_LOGIN_CANCELED_BY_USR_MSG);
            }
        }
    }

    void AuthenticationService::OnAddUserRequested(User& user, const QString& password) {
        // handle creation: validate, encrypt, save to DB
        QString encryptedPassword = m_securityService.encryptPassword(password);
        user.PasswordHash = encryptedPassword;
        Result<User> createdUser = m_repository.createUser(user);
        if (!createdUser.isSuccess)
        {
            QString error = translator->getErrorMessage(AUTH_FAILED_TO_CREATE_USER_ERROR_MSG).arg(createdUser.message);
            logger->LogError(error);
            emit FailedToCreateUser(user, createdUser.message);
        }
        else
        {
            emit UserCreatedSuccessfully(user);
        }
    }

    void AuthenticationService::OnUpdateUserRequested(User& user, const QString& password) {
        // handle creation: validate, encrypt, save to DB
        QString encryptedPassword = m_securityService.encryptPassword(password);
        user.PasswordHash = encryptedPassword;
        Result<User> updatedUser = m_repository.updateUser(user);
        if (!updatedUser.isSuccess)
        {
            QString error = translator->getErrorMessage(AUTH_FAILED_TO_UPDATE_USER_ERROR_MSG).arg(updatedUser.message);
            logger->LogError(error);
            emit FailedToUpdateUser(user, updatedUser.message);
        }
        else
        {
            emit UserUpdatedSuccessfully(user);
        }
    }

    void AuthenticationService::OnDeleteUserRequested(User& user)
    {
        Result<User> deletedUserResult = m_repository.deleteUser(user);
        if (!deletedUserResult.isSuccess)
        {
            QString error = translator->getErrorMessage(AUTH_FAILED_TO_DELETE_USER_ERROR_MSG).arg(deletedUserResult.message);
            logger->LogError(error);
            emit FailedToDeleteUser(user, deletedUserResult.message);
        }
        else
        {
            emit UserDeletedSuccessfully(user);
        }
    }

    Result<std::optional<Entity::User>> AuthenticationService::authenticateUser() const
    {
        auto usernames = m_repository.getAllActiveUsers();

        if (!usernames.isSuccess || usernames.value.isEmpty()) {

            displayUserManagerDialog();

        }
        Result<std::optional<Entity::User>> loginDialogResult = DisplayLoginDialog();

        return loginDialogResult;
    }

} 



