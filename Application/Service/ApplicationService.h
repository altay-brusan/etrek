/**
 * @file ApplicationService.h
 * @brief Central application orchestration service.
 *
 * @details Provides the main entry point for application initialization,
 *          coordinating database setup, authentication, device initialization,
 *          and main window construction.
 *
 * @author Etrek Development Team
 * @date 2026-01-11
 *
 * @copyright Copyright (c) 2024-2026 Etrek Medical Imaging
 *
 * @see ILaunchStrategy
 * @see AuthenticationService
 * @see ContextManager
 */

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

namespace Etrek::Core::Context {
    class ContextManager;
    class ContextAuditService;
}

namespace Etrek::Context {
    class IContextManager;
}

/**
 * @namespace Etrek::Application::Service
 * @brief Contains application-level service classes.
 */
namespace Etrek::Application::Service
{
    class ILaunchStrategy;

    /**
     * @class ApplicationService
     * @brief Central orchestration service for application lifecycle.
     *
     * @details Singleton-like service that coordinates all aspects of
     *          application initialization and lifecycle management.
     *
     *          Initialization order:
     *          1. Logger setup (spdlog)
     *          2. Settings loading (SettingProvider)
     *          3. Database initialization (Qt SQL with MySQL)
     *          4. Authentication service setup
     *          5. RIS connections (ModalityWorklistManager)
     *          6. Device initialization
     *          7. Main window construction
     *
     *          The service uses the Strategy pattern for different launch
     *          modes (main, demo, settings, users) via ILaunchStrategy.
     *
     * @see ILaunchStrategy
     * @see AuthenticationService
     * @see ContextManager
     * @see MainWindowDelegate
     *
     * @ingroup Application
     */
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

        /**
         * @brief Returns the context manager instance.
         * @return Shared pointer to the context manager.
         */
        std::shared_ptr<Etrek::Context::IContextManager> contextManager() const;

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
        std::shared_ptr<Etrek::Core::Context::ContextManager> m_contextManager;
        std::shared_ptr<Etrek::Core::Context::ContextAuditService> m_contextAuditService;

        // Value members - MUST include headers
        Etrek::Core::Setting::SettingProvider m_settingProvider;
        Etrek::Core::Security::CryptoManager m_securityService;
    };

} // namespace Etrek::Application::Service

#endif // APPLICATION_SERVICE_H