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
        void userCreatedSuccessfully(User& user);
        void failedToCreateUser(User& user, QString& failReason);
        void userUpdatedSuccessfully(User& user);
        void failedToUpdateUser(User& user, QString& failReason);
        void userDeletedSuccessfully(User& user);
        void failedToDeleteUser(User& user, QString& failReason);

    private slots:
        void onAddUserRequested(User& user, const QString& password);
        void onUpdateUserRequested(User& user, const QString& password);
        void onDeleteUserRequested(User& user);


    private:
        void displayUserManagerDialog() const;
        Result<std::optional<Entity::User>> displayLoginDialog() const;


        CryptoManager m_securityService;
        AuthenticationRepository m_repository;
        TranslationProvider* translator;
        std::shared_ptr<AppLogger> logger;
    };

}


#endif // AUTHENTICATIONSERVICE_H
