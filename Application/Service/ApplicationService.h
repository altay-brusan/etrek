#ifndef APPLICATION_SERVICE_H
#define APPLICATION_SERVICE_H

#include <QObject>
#include <memory>
#include <functional>
#include <optional>
#include <QSharedPointer>


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


namespace Etrek::Application::Service
{

    namespace lg = Etrek::Core::Log;
    namespace set = Etrek::Core::Setting;
    namespace mdl = Etrek::Core::Data::Model;
    namespace ent = Etrek::Core::Data::Entity;
    namespace rpo = Etrek::Core::Repository;
    namespace sec = Etrek::Core::Security;
    namespace dlg = Etrek::Application::Delegate;
    namespace aut = Etrek::Application::Authentication;
    namespace cnc = Etrek::Worklist::Connectivity;
    namespace spc = Etrek::Specification;

    class ApplicationService : public QObject
    {
        Q_OBJECT
    public:
        explicit ApplicationService(QObject* parent = nullptr);
        void initialize(spc::LaunchMode mode);
        bool initializeDatabase(std::function<void(const QString&, int)> progressCallback);
        void initializeRisConnections(std::function<void(const QString&, int)> progressCallback);
        void intializeAuthentication(std::function<void(const QString&, int)> progressCallback);
        void intializeDevices(std::function<void(const QString&, int)> progressCallback);
        std::optional<ent::User> authenticateUser();
        void loadMainWindow(std::function<void(const QString&, int)> progressCallback);
        bool loadSettings(std::function<void(const QString&, int)> progressCallback);
        void setupLogger(std::function<void(const QString&, int)> progressCallback);
		void connectSignalsAndSlots();
        void closeApplication();
        void constructMainWindow();
        void showMainWindow();
        ~ApplicationService();

    private:
        std::unique_ptr<ILaunchStrategy> createLaunchStrategy(spc::LaunchMode mode);


        std::unique_ptr<MainWindow> m_mainWindow;
        dlg::MainWindowDelegate* m_mainWindowDelegate = nullptr;
        std::shared_ptr<AppLogger> logger;
        set::SettingProvider m_settingProvider;
        TranslationProvider* translator;
        std::shared_ptr<rpo::AuthenticationRepository> m_authRepository;
        sec::CryptoManager m_securityService;
        aut::AuthenticationService* m_authService = nullptr;
        std::shared_ptr<Model::DatabaseConnectionSetting> m_databaseConnectionSetting;
        std::shared_ptr<Model::FileLoggerSetting> m_fileLoggerSetting;
        QVector< QSharedPointer<Model::RisConnectionSetting>> m_risConnectionSettingList;
        cnc::ModalityWorklistManager* m_modalityWorklistManager = nullptr;
    };
}

#endif
