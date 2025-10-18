#ifndef APPLICATION_SERVICE_H
#define APPLICATION_SERVICE_H

#include <QObject>
#include <QString>
#include <QVector>
#include <QSharedPointer>
#include <memory>
#include <functional>
#include <optional>

// These MUST be included (value members)
#include "SettingProvider.h"
#include "CryptoManager.h"
#include "MainWindow.h"

// Forward declarations for pointer/smart pointer members
namespace Etrek::Specification {
    enum class LaunchMode;
}

namespace Etrek::Core::Data::Entity {
    class User;
}

namespace Etrek::Core::Data::Model {
    class DatabaseConnectionSetting;
    class FileLoggerSetting;
    class RisConnectionSetting;
}

namespace Etrek::Core::Log {
    class AppLogger;
}

namespace Etrek::Core::Globalization {
    class TranslationProvider;
}

namespace Etrek::Application::Authentication {
    class AuthenticationService;
}

namespace Etrek::Application::Delegate {
    class MainWindowDelegate;
}

namespace Etrek::Core::Repository {
    class AuthenticationRepository;
}

namespace Etrek::Worklist::Connectivity {
    class ModalityWorklistManager;
}

class ILaunchStrategy;

namespace Etrek::Application::Service
{
    class ApplicationService : public QObject
    {
        Q_OBJECT

    public:
        explicit ApplicationService(QObject* parent = nullptr);
        ~ApplicationService() override;  // MUST define in .cpp for unique_ptr

        void initialize(Etrek::Specification::LaunchMode mode);
        bool initializeDatabase(std::function<void(const QString&, int)> progressCallback);
        void initializeRisConnections(std::function<void(const QString&, int)> progressCallback);
        void intializeAuthentication(std::function<void(const QString&, int)> progressCallback);
        void intializeDevices(std::function<void(const QString&, int)> progressCallback);
        std::optional<Etrek::Core::Data::Entity::User> authenticateUser();
        void loadMainWindow(std::function<void(const QString&, int)> progressCallback);
        bool loadSettings(std::function<void(const QString&, int)> progressCallback);
        void setupLogger(std::function<void(const QString&, int)> progressCallback);
        void connectSignalsAndSlots();
        void closeApplication();
        void constructMainWindow();
        void showMainWindow();

    private:
        std::unique_ptr<ILaunchStrategy> createLaunchStrategy(Etrek::Specification::LaunchMode mode);

        // Smart pointers and raw pointers - forward declarations OK
        std::unique_ptr<MainWindow> m_mainWindow;
        Etrek::Application::Delegate::MainWindowDelegate* m_mainWindowDelegate = nullptr;
        std::shared_ptr<Etrek::Core::Log::AppLogger> logger;
        Etrek::Core::Globalization::TranslationProvider* translator = nullptr;
        std::shared_ptr<Etrek::Core::Repository::AuthenticationRepository> m_authRepository;
        Etrek::Application::Authentication::AuthenticationService* m_authService = nullptr;
        std::shared_ptr<Etrek::Core::Data::Model::DatabaseConnectionSetting> m_databaseConnectionSetting;
        std::shared_ptr<Etrek::Core::Data::Model::FileLoggerSetting> m_fileLoggerSetting;
        QVector<QSharedPointer<Etrek::Core::Data::Model::RisConnectionSetting>> m_risConnectionSettingList;
        Etrek::Worklist::Connectivity::ModalityWorklistManager* m_modalityWorklistManager = nullptr;

        // Value members - MUST include headers
        Etrek::Core::Setting::SettingProvider m_settingProvider;
        Etrek::Core::Security::CryptoManager m_securityService;
    };

} // namespace Etrek::Application::Service

#endif // APPLICATION_SERVICE_H