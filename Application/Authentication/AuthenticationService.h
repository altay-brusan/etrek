#ifndef AUTHENTICATIONSERVICE_H
#define AUTHENTICATIONSERVICE_H
#include <QObject>
#include <AuthenticationRepository.h>
#include <CryptoManager.h>


namespace Etrek::Application::Authentication {

    namespace spc = Etrek::Specification;
    namespace ent = Etrek::Core::Data::Entity;
    namespace rep = Etrek::Core::Repository;
    namespace sec = Etrek::Core::Security;
    namespace glob = Etrek::Core::Globalization;
    namespace lg = Etrek::Core::Log;

    class AuthenticationService : public QObject
    {
        Q_OBJECT

    public:
        explicit AuthenticationService(const rep::AuthenticationRepository& repository,
            const sec::CryptoManager& securityService,
            QObject* parent = nullptr);


        spc::Result<std::optional<ent::User>> authenticateUser() const;


    signals:
        void userCreatedSuccessfully(ent::User& user);
        void failedToCreateUser(ent::User& user, QString& failReason);
        void userUpdatedSuccessfully(ent::User& user);
        void failedToUpdateUser(ent::User& user, QString& failReason);
        void userDeletedSuccessfully(ent::User& user);
        void failedToDeleteUser(ent::User& user, QString& failReason);

    private slots:
        void onAddUserRequested(ent::User& user, const QString& password);
        void onUpdateUserRequested(ent::User& user, const QString& password);
        void onDeleteUserRequested(ent::User& user);


    private:
        void displayUserManagerDialog() const;
        spc::Result<std::optional<ent::User>> displayLoginDialog() const;


        sec::CryptoManager m_securityService;
        rep::AuthenticationRepository m_repository;
        glob::TranslationProvider* translator;
        std::shared_ptr<lg::AppLogger> logger;
    };

}


#endif // AUTHENTICATIONSERVICE_H
