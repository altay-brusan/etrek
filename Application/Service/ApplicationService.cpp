#include "ApplicationService.h"

#include <QTimer>
#include <functional>
#include <QApplication>
#include <QCoreApplication>
#include "MessageKey.h"
#include "LoggerProvider.h"
#include "AppLoggerFactory.h"

#include "DatabaseSetupManager.h"
#include "UserManagerLaunchStrategy.h"
#include "SettingManagerLaunchStrategy.h"
#include "DemoLaunchStrategy.h"
#include "DeveloperLaunchStrategy.h"
#include "MainAppLaunchStrategy.h"
#include "ModalityWorklistManager.h"
#include "WorklistRepository.h"
#include "WorklistFieldConfigurationRepository.h"
#include "MainWindowBuilder.h"

using namespace Etrek::Application::Authentication;
using namespace Etrek::Application::Delegate;

namespace Etrek::Application::Service
{
    using Etrek::Specification::LaunchMode;
    using Etrek::Specification::Result;

    using namespace Etrek::Core::Log;
    using namespace Etrek::Core::Setting;
    using namespace Etrek::Core::Data::Model;
    using namespace Etrek::Core::Data::Entity;
    using namespace Etrek::Core::Repository;
    using namespace Etrek::Core::Security;
    using namespace Etrek::Application::Delegate;
    using namespace Etrek::Application::Authentication;
    using namespace Etrek::Worklist::Connectivity;
    using namespace Etrek::Specification;
    using namespace Etrek::Core::Globalization;
	using namespace Etrek::Worklist::Repository;
    ApplicationService::ApplicationService(QObject* parent)
        : QObject{ parent }
    {
        translator = &TranslationProvider::Instance();		
    }


    void ApplicationService::initialize(LaunchMode launchMode)
    {        

        qRegisterMetaType<Role>("Role");
        qRegisterMetaType<User>("User");

        auto strategy = createLaunchStrategy(launchMode);
        strategy->launch(this);

    }

    std::unique_ptr<ILaunchStrategy> ApplicationService::createLaunchStrategy(LaunchMode mode) {
        switch (mode) {
        case LaunchMode::UserManager: return std::make_unique<UserManagerLaunchStrategy>(this);
        case LaunchMode::SettingManager: return std::make_unique<SettingManagerLaunchStrategy>(this);
        case LaunchMode::Demo: return std::make_unique<DemoLaunchStrategy>(this);
        case LaunchMode::Developer: return std::make_unique<DeveloperLaunchStrategy>(this);
        case LaunchMode::MainApp:
        default: return std::make_unique<MainAppLaunchStrategy>(this);
        }
    }

    std::optional<User> ApplicationService::authenticateUser()
    {
        if (!m_authService) {
            if (logger) logger->LogError("Auth service is not initialized.");
            return std::nullopt;
        }

        logger->LogInfo(translator->getInfoMessage(AUTH_START_MSG));
        const auto authenticationResult = m_authService->authenticateUser();
        if (!authenticationResult.isSuccess) {
            return std::nullopt;
        }
        logger->LogInfo(translator->getInfoMessage(AUTH_SUCCEED));
		return authenticationResult.value;
    }

    void ApplicationService::setupLogger(std::function<void(const QString&, int)> progressCallback)
    {
        if (progressCallback) {
            progressCallback("Setup logging system...", 10);
        }
        if (!m_fileLoggerSetting) {
            // Either call loadSettings() earlier or bail out
            throw std::runtime_error("File logger settings not loaded.");
        }
		// Initialize the logger with file settings for all systems
        LoggerProvider::Instance().InitializeFileLogger(m_fileLoggerSetting->getLogDirectory(),
            m_fileLoggerSetting->getFileSize(),
            m_fileLoggerSetting->getFileCount(),
            translator);

		// Initialize the logger for logging the applicationService events
        AppLoggerFactory factory(LoggerProvider::Instance(), translator);
        logger = factory.CreateLogger("applicationService");

        logger->LogInfo(translator->getInfoMessage(LOG_FILE_LOGGER_INIT_SUCCEED_MSG));

    }

    void ApplicationService::connectSignalsAndSlots()
    {
        if (!m_mainWindowDelegate) {
            logger->LogError("Main window is not initialized. Cannot connect signals.");
            return;
        }
        if (!m_modalityWorklistManager) {
            logger->LogWarning("ModalityWorklistManager not ready; skipping connections.");
            return;
        }

        QObject::connect(
            m_mainWindowDelegate, &MainWindowDelegate::aboutToClose,
            m_modalityWorklistManager, &ModalityWorklistManager::onAboutToCloseApplication,
            Qt::UniqueConnection
        );
    }

    
    void ApplicationService::closeApplication()
    {
        if (m_mainWindowDelegate) {
            m_mainWindowDelegate->deleteLater();
            m_mainWindowDelegate = nullptr;
        }

        if (m_mainWindow) {
            m_mainWindow->close();
            m_mainWindow.reset();
        }

        LoggerProvider::Instance().Shutdown();

        // Defer quit until event loop starts
        QTimer::singleShot(0, qApp, &QCoreApplication::quit);

    }

    void ApplicationService::constructMainWindow()
	{

        auto worklistRepo = std::make_shared<WorklistFieldConfigurationRepository>(m_databaseConnectionSetting);
        //MainWindowDelegateBuilder builder(worklistRepo);
        //auto mainWindowDelegate = builder.build(nullptr);

    }
        
    void ApplicationService::loadMainWindow(std::function<void(const QString&, int)> progressCallback)
    {
        logger->LogInfo("loading main window");

        if (progressCallback) {
            progressCallback("Loading screens...", 10);
        }

        if (m_mainWindowDelegate) {
            m_mainWindowDelegate->deleteLater();
            m_mainWindowDelegate = nullptr;
        }

        if (m_mainWindow) {
            m_mainWindow->close();
            m_mainWindow.reset();
        }

        DelegateParameter params;
        params.dbConnection = m_databaseConnectionSetting;

        MainWindowBuilder builder;
        auto result = builder.build(params, nullptr, this);
        m_mainWindow.reset(result.first);
        m_mainWindowDelegate = result.second;

        if (!m_mainWindow || !m_mainWindowDelegate) {
            logger->LogError("Failed to build main window or delegate.");
            return;
        }

    }

    void ApplicationService::showMainWindow()
    {
		if (!m_mainWindowDelegate) {
			logger->LogError("loading main window failed!");
			return;
		}
        if (!m_mainWindow) {
            logger->LogError("Main window instance is not available for display.");
            return;
        }

        // Show the main window
        m_mainWindowDelegate->show();
    }

    void ApplicationService::initializeRisConnections(std::function<void(const QString&, int)> progressCallback)
    {
        logger->LogInfo(translator->getInfoMessage(RIS_START_NETWORK_INIT_MSG));
        if (progressCallback) progressCallback("Initializing RIS connections...", 20);

        if (m_risConnectionSettingList.isEmpty()) {
            logger->LogWarning("No RIS connections configured.");
            return;
        }

        const auto risQ = m_risConnectionSettingList.first();
        if (!risQ || !risQ->getActiveFlag()) {
            logger->LogInfo("RIS connection is not active.");
            return;
        }

        auto worklistRepository = std::make_shared<WorklistRepository>(m_databaseConnectionSetting);

        // Bridge QSharedPointer -> std::shared_ptr while keeping the Qt ref alive.
        std::shared_ptr<RisConnectionSetting> risStd(
            risQ.data(),
            [keepAlive = risQ](RisConnectionSetting*) mutable { keepAlive.clear(); } // hold ref
        );

        m_modalityWorklistManager = new ModalityWorklistManager(worklistRepository, risStd, this);

        const auto profiles = worklistRepository->getProfiles();
        if (profiles.isSuccess && !profiles.value.isEmpty()) {
            const auto defaultProfile = profiles.value.first();
            m_modalityWorklistManager->setActiveProfile(defaultProfile);
            m_modalityWorklistManager->startWorklistQueryFromRis();
        }
        else {
            logger->LogWarning("No worklist profiles available.");
        }

        logger->LogInfo(translator->getInfoMessage(RIS_NETWORK_INIT_SUCCEED));
    }


    void ApplicationService::intializeAuthentication(std::function<void(const QString&, int)> progressCallback)
    {
        logger->LogInfo(translator->getInfoMessage(AUTH_START_INIT_AUTH_MSG));

        if (progressCallback) {
            progressCallback("Initializing authentication system...", 30);
        }
        AuthenticationRepository authRepository(m_databaseConnectionSetting, translator);
        m_authService = new AuthenticationService(authRepository, m_securityService, this);

        logger->LogInfo(translator->getInfoMessage(AUTH_INIT_AUTH_SUCCEED));
    }

    void ApplicationService::intializeDevices(std::function<void(const QString&, int)> progressCallback)
    {
        logger->LogInfo(translator->getInfoMessage(DEV_START_INIT_DEVICE_MSG));
        // TODO: device drivers should be controlled here!
        logger->LogInfo(translator->getInfoMessage(DEV_INIT_DEVICE_SUCCEED));
    }

    bool ApplicationService::initializeDatabase(std::function<void(const QString&, int)> progressCallback)
    {
        logger->LogInfo(translator->getInfoMessage(DB_START_INIT_MSG));

		if (progressCallback) {
			progressCallback("Initializing database...", 15);
		}

        DatabaseSetupManager initializer(m_databaseConnectionSetting);
        Result<QString> result = initializer.initializeDatabase();
        if (!result.isSuccess)
            return false;

        logger->LogInfo(translator->getInfoMessage(DB_INIT_SUCCESS_MSG));

		return true;

    }

    bool ApplicationService::loadSettings(std::function<void(const QString&, int)> progressCallback)
    {
        if (progressCallback) {
            progressCallback("Loading settings...", 10);
        }

        auto settingProvider = std::make_unique<SettingProvider>();
        bool result = settingProvider->loadSettingsFile("./setting/Settings.json");
        if(result == false)
			return false;

        m_databaseConnectionSetting = settingProvider->getDatabaseConnectionSettings();
        m_risConnectionSettingList = settingProvider->getRisSettings();
        m_fileLoggerSetting = settingProvider->getFileLoggerSettings();

		return true;
    }

    ApplicationService::~ApplicationService()
    {
        if (m_mainWindowDelegate) {
            m_mainWindowDelegate->deleteLater();
            m_mainWindowDelegate = nullptr;
        }

        if (m_mainWindow) {
            m_mainWindow->close();
            m_mainWindow.reset();
        }
    }

}

