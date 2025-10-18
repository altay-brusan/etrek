#ifndef AUTHENTICATIONSERVICE_H
#define AUTHENTICATIONSERVICE_H

#include <QObject>
#include <QString>
#include <memory>
#include <optional>

// Forward declarations - no heavy includes needed
namespace Etrek::Core::Repository {
    class AuthenticationRepository;
}

namespace Etrek::Core::Security {
    class CryptoManager;
}

namespace Etrek::Core::Data::Entity {
    class User;
}

namespace Etrek::Specification {
    template<typename T> class Result;
}

namespace Etrek::Core::Globalization {
    class TranslationProvider;
}

namespace Etrek::Core::Log {
    class AppLogger;
}

namespace Etrek::Application::Authentication {

    class AuthenticationService : public QObject
    {
        Q_OBJECT
    public:
        explicit AuthenticationService(
            const Etrek::Core::Repository::AuthenticationRepository& repository,
            const Etrek::Core::Security::CryptoManager& securityService,
            QObject* parent = nullptr);

        Etrek::Specification::Result<std::optional<Etrek::Core::Data::Entity::User>>
            authenticateUser() const;

    signals:
        void userCreatedSuccessfully(Etrek::Core::Data::Entity::User& user);
        void failedToCreateUser(Etrek::Core::Data::Entity::User& user, QString& failReason);
        void userUpdatedSuccessfully(Etrek::Core::Data::Entity::User& user);
        void failedToUpdateUser(Etrek::Core::Data::Entity::User& user, QString& failReason);
        void userDeletedSuccessfully(Etrek::Core::Data::Entity::User& user);
        void failedToDeleteUser(Etrek::Core::Data::Entity::User& user, QString& failReason);

    private slots:
        void onAddUserRequested(Etrek::Core::Data::Entity::User& user, const QString& password);
        void onUpdateUserRequested(Etrek::Core::Data::Entity::User& user, const QString& password);
        void onDeleteUserRequested(Etrek::Core::Data::Entity::User& user);

    private:
        void displayUserManagerDialog() const;
        Etrek::Specification::Result<std::optional<Etrek::Core::Data::Entity::User>>
            displayLoginDialog() const;

        Etrek::Core::Security::CryptoManager m_securityService;
        Etrek::Core::Repository::AuthenticationRepository m_repository;
        Etrek::Core::Globalization::TranslationProvider* translator;  // non-owning
        std::shared_ptr<Etrek::Core::Log::AppLogger> logger;
    };

} // namespace Etrek::Application::Authentication

#endif // AUTHENTICATIONSERVICE_H