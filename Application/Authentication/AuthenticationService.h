#ifndef AUTHENTICATIONSERVICE_H
#define AUTHENTICATIONSERVICE_H
#include <QObject>
#include "AuthenticationRepository.h"
#include "TranslationProvider.h"
#include "AppLogger.h"
#include "CryptoManager.h"


using namespace Etrek::Core::Log;
using namespace Etrek::Core::Data;
using namespace Etrek::Core::Security;
using namespace Etrek::Core::Repository;

namespace Etrek::Application::Authentication {

    class AuthenticationService : public QObject
    {
        Q_OBJECT

    public:
        explicit AuthenticationService(const AuthenticationRepository& repository,
            const CryptoManager& securityService,
            QObject* parent = nullptr);


        Result<std::optional<Entity::User>> authenticateUser() const;


    signals:
        void UserCreatedSuccessfully(User& user);
        void FailedToCreateUser(User& user, QString& failReason);
        void UserUpdatedSuccessfully(User& user);
        void FailedToUpdateUser(User& user, QString& failReason);
        void UserDeletedSuccessfully(User& user);
        void FailedToDeleteUser(User& user, QString& failReason);

    private slots:
        void OnAddUserRequested(User& user, const QString& password);
        void OnUpdateUserRequested(User& user, const QString& password);
        void OnDeleteUserRequested(User& user);


    private:
        void displayUserManagerDialog() const;
        Result<std::optional<Entity::User>> DisplayLoginDialog() const;


        CryptoManager m_securityService;
        AuthenticationRepository m_repository;
        TranslationProvider* translator;
        std::shared_ptr<AppLogger> logger;
    };

}


#endif // AUTHENTICATIONSERVICE_H
