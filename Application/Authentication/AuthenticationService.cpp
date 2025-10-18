#include "AuthenticationService.h"
#include "AuthenticationRepository.h"
#include "CryptoManager.h"
#include "User.h"
#include "Result.h"
#include "MessageKey.h"
#include "LoginDialog.h"
#include "UserMangerDialog.h"
#include "TranslationProvider.h"
#include "LoggerProvider.h"
#include "AppLoggerFactory.h"

namespace Etrek::Application::Authentication
{
    // Using declarations - safe in .cpp files
    using Etrek::Specification::Result;
    using Etrek::Core::Data::Entity::User;
    using Etrek::Core::Repository::AuthenticationRepository;
    using Etrek::Core::Security::CryptoManager;
    using Etrek::Core::Globalization::TranslationProvider;
    namespace lg = Etrek::Core::Log;

    AuthenticationService::AuthenticationService(
        const AuthenticationRepository& repository,
        const CryptoManager& securityService,
        QObject* parent)
        : QObject(parent)
        , m_securityService(securityService)
        , m_repository(repository)
    {
        translator = &TranslationProvider::Instance();
        lg::AppLoggerFactory factory(lg::LoggerProvider::Instance(), translator);
        logger = factory.CreateLogger("AuthenticationService");
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

        connect(&manager, &UserMangerDialog::AddUserRequested,
            this, &AuthenticationService::onAddUserRequested);
        connect(&manager, &UserMangerDialog::UpdateUserRequested,
            this, &AuthenticationService::onUpdateUserRequested);
        connect(&manager, &UserMangerDialog::DeleteUserRequested,
            this, &AuthenticationService::onDeleteUserRequested);

        connect(this, &AuthenticationService::userCreatedSuccessfully,
            &manager, &UserMangerDialog::onUserCreated);
        connect(this, &AuthenticationService::failedToCreateUser,
            &manager, &UserMangerDialog::onUserCreationFailed);
        connect(this, &AuthenticationService::userUpdatedSuccessfully,
            &manager, &UserMangerDialog::onUserUpdated);
        connect(this, &AuthenticationService::failedToUpdateUser,
            &manager, &UserMangerDialog::onUserUpdateFailed);
        connect(this, &AuthenticationService::userDeletedSuccessfully,
            &manager, &UserMangerDialog::onUserDeleted);
        connect(this, &AuthenticationService::failedToDeleteUser,
            &manager, &UserMangerDialog::onUserDeletionFailed);

        // This will block the execution until the dialog is accepted or rejected.
        if (manager.exec() == QDialog::Accepted) {
            // Handle post-dialog logic when the dialog is accepted
            return;
        }
    }

    Result<std::optional<User>> AuthenticationService::displayLoginDialog() const
    {
        // Get all active users from the repository
        auto activeResult = m_repository.getAllActiveUsers();
        if (!activeResult.isSuccess) {
            return Result<std::optional<User>>::Failure(
                "Failed to load user list during login.");
        }

        // Create login dialog with active users
        LoginDialog loginDialog(activeResult.value);

        // Loop until the user enters valid credentials or cancels
        while (true) {
            if (loginDialog.exec() == QDialog::Accepted) {
                // Get credentials (username and password)
                std::optional<std::pair<User, QString>> credentials =
                    loginDialog.getLoginCredentials();

                if (credentials.has_value()) {
                    // Fetch user from the repository
                    auto result = m_repository.getUser(credentials.value().first.Username);

                    if (result.isSuccess) {
                        // Compare passwords
                        auto decryptedPassword =
                            m_securityService.decryptPassword(result.value.PasswordHash);

                        if (QString::compare(credentials.value().second,
                            decryptedPassword,
                            Qt::CaseInsensitive) == 0) {
                            // Success, return the user
                            return Result<std::optional<User>>::Success(result.value);
                        }
                        else {
                            // If password is incorrect, clear and try again
                            loginDialog.clearPassword();
                            continue;
                        }
                    }
                }
            }
            else {
                return Result<std::optional<User>>::Failure(
                    AUTH_LOGIN_CANCELED_BY_USR_MSG);
            }
        }
    }

    void AuthenticationService::onAddUserRequested(User& user, const QString& password)
    {
        // Handle creation: validate, encrypt, save to DB
        QString encryptedPassword = m_securityService.encryptPassword(password);
        user.PasswordHash = encryptedPassword;

        Result<User> createdUser = m_repository.createUser(user);
        if (!createdUser.isSuccess) {
            QString error = translator->getErrorMessage(AUTH_FAILED_TO_CREATE_USER_ERROR_MSG)
                .arg(createdUser.message);
            logger->LogError(error);
            emit failedToCreateUser(user, createdUser.message);
        }
        else {
            emit userCreatedSuccessfully(user);
        }
    }

    void AuthenticationService::onUpdateUserRequested(User& user, const QString& password)
    {
        // Handle update: validate, encrypt, save to DB
        QString encryptedPassword = m_securityService.encryptPassword(password);
        user.PasswordHash = encryptedPassword;

        Result<User> updatedUser = m_repository.updateUser(user);
        if (!updatedUser.isSuccess) {
            QString error = translator->getErrorMessage(AUTH_FAILED_TO_UPDATE_USER_ERROR_MSG)
                .arg(updatedUser.message);
            logger->LogError(error);
            emit failedToUpdateUser(user, updatedUser.message);
        }
        else {
            emit userUpdatedSuccessfully(user);
        }
    }

    void AuthenticationService::onDeleteUserRequested(User& user)
    {
        Result<User> deletedUserResult = m_repository.deleteUser(user);
        if (!deletedUserResult.isSuccess) {
            QString error = translator->getErrorMessage(AUTH_FAILED_TO_DELETE_USER_ERROR_MSG)
                .arg(deletedUserResult.message);
            logger->LogError(error);
            emit failedToDeleteUser(user, deletedUserResult.message);
        }
        else {
            emit userDeletedSuccessfully(user);
        }
    }

    Result<std::optional<User>> AuthenticationService::authenticateUser() const
    {
        auto usernames = m_repository.getAllActiveUsers();

        if (!usernames.isSuccess || usernames.value.isEmpty()) {
            displayUserManagerDialog();
        }

        Result<std::optional<User>> loginDialogResult = displayLoginDialog();
        return loginDialogResult;
    }

} // namespace Etrek::Application::Authentication