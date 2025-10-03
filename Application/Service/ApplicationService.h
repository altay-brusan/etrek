#ifndef APPLICATION_SERVICE_H
#define APPLICATION_SERVICE_H

#include <QObject>
#include "LaunchMode.h"
#include "MainWindow.h"
#include "AppLogger.h"
#include "CryptoManager.h"
#include "SettingProvider.h"
#include "FileloggerSetting.h"
#include "TranslationProvider.h"
#include "AuthenticationService.h"

#include "DatabaseConnectionSetting.h"
#include "AuthenticationRepository.h"
#include "ModalityWorklistManager.h"
#include "MainWindowDelegate.h"

#include "ILaunchStrategy.h"
#include "SplashScreenManager.h"


using namespace Etrek::Core::Log;
using namespace Etrek::Core::Setting;
using namespace Etrek::Core::Data::Model;
using namespace Etrek::Core::Repository;
using namespace Etrek::Core::Security;
using namespace Etrek::Application::Authentication;
using namespace Etrek::Worklist::Connectivity;

namespace Etrek::Application::Service
{
    class ApplicationService : public QObject
    {
        Q_OBJECT
    public:
        explicit ApplicationService(QObject* parent = nullptr);
        void initialize(LaunchMode mode);
        bool initializeDatabase(std::function<void(const QString&, int)> progressCallback);
        void initializeRisConnections(std::function<void(const QString&, int)> progressCallback);
        void intializeAuthentication(std::function<void(const QString&, int)> progressCallback);
        void intializeDevices(std::function<void(const QString&, int)> progressCallback);
        std::optional<Entity::User> authenticateUser();
        void loadMainWindow(std::function<void(const QString&, int)> progressCallback);
        bool loadSettings(std::function<void(const QString&, int)> progressCallback);
        void setupLogger(std::function<void(const QString&, int)> progressCallback);
		void connectSignalsAndSlots();
        void closeApplication();
        void constructMainWindow();
        void showMainWindow();
        ~ApplicationService();

    private:
        std::unique_ptr<ILaunchStrategy> createLaunchStrategy(LaunchMode mode);


        MainWindow* m_mainWindow = nullptr;
        MainWindowDelegate* m_mainWindowDelegate;
        std::shared_ptr<AppLogger> logger;
        SettingProvider m_settingProvider;
        TranslationProvider* translator;
        std::shared_ptr<AuthenticationRepository> m_authRepository;
        CryptoManager m_securityService;
        AuthenticationService* m_authService = nullptr;
        std::shared_ptr<Model::DatabaseConnectionSetting> m_databaseConnectionSetting;
        std::shared_ptr<Model::FileLoggerSetting> m_fileLoggerSetting;
        QVector< QSharedPointer<Model::RisConnectionSetting>> m_risConnectionSettingList;
        ModalityWorklistManager* m_modalityWorklistManager = nullptr;
    };
}

#endif
