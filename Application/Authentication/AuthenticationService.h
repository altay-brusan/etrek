#ifndef AUTHENTICATIONSERVICE_H
#define AUTHENTICATIONSERVICE_H
#include <QObject>
#include <AuthenticationRepository.h>
#include <CryptoManager.h>


namespace Etrek::Application::Authentication {

    using Etrek::Specification::Result;
	namespace ent = Etrek::Core::Data::Entity;
	namespace usr = Etrek::Core::Data;
    namespace rep = Etrek::Core::Repository;
	namespace sec = Etrek::Core::Security;

    class AuthenticationService : public QObject
    {
        Q_OBJECT

    public:
        explicit AuthenticationService(const rep::AuthenticationRepository& repository,
            const Etrek::Core::Security::CryptoManager& securityService,
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


        sec::CryptoManager m_securityService;
        rep::AuthenticationRepository m_repository;
        TranslationProvider* translator;
        std::shared_ptr<AppLogger> logger;
    };

}


#endif // AUTHENTICATIONSERVICE_H
