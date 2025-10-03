#include "MainAppLaunchStrategy.h"  
#include "ApplicationService.h"  
#include <qmessagebox.h>


namespace Etrek::Application::Service  
{  
	MainAppLaunchStrategy::MainAppLaunchStrategy(QObject* parent)
		: QObject(parent)
	{
		m_splashScreenManager = new SplashScreenManager(this);
	}  

void MainAppLaunchStrategy::launch(ApplicationService* service)  
{  
	m_splashScreenManager->displaySplashScreen();

	bool result = service->loadSettings([this](const QString& message, int progress) {  
		m_splashScreenManager->updateSplashScreenMessage(message, progress);  
	});
	
	if (!result) {
		m_splashScreenManager->closeSplashScreen();

		QMessageBox msgWarning;
		msgWarning.setText(
			"FATAL ERROR!\n"			
			"Unable to load settings file. Please check the settings directory for required files."
		);
		msgWarning.setIcon(QMessageBox::Critical);
		msgWarning.setWindowTitle("Fatal Error");

		msgWarning.setMinimumSize(400, 300);
				
		QPixmap iconPixmap(":/Asset/e-icon-32.png"); // Example resource path
		if (!iconPixmap.isNull()) {
			msgWarning.setWindowIcon(QIcon(iconPixmap));
		}

		msgWarning.exec();
		service->closeApplication();
		return;
	}

	service->setupLogger([this](const QString& message, int progress) {
		m_splashScreenManager->updateSplashScreenMessage(message, progress);
		});


	result = service->initializeDatabase([this](const QString& message, int progress) {  
		m_splashScreenManager->updateSplashScreenMessage(message, progress);  
	});
	if (!result) {
		m_splashScreenManager->closeSplashScreen();
		QMessageBox msgWarning;
		msgWarning.setText(
			"FATAL ERROR!\n"
			"Unable to initialize the database. Please check the database connection settings."
		);
		msgWarning.setIcon(QMessageBox::Critical);
		msgWarning.setWindowTitle("Fatal Error");
		msgWarning.setMinimumSize(400, 300);
		
		QPixmap iconPixmap(":/Asset/e-icon-32.png");
		if (!iconPixmap.isNull()) {
			msgWarning.setWindowIcon(QIcon(iconPixmap));
		}
		msgWarning.exec();
		service->closeApplication();
		return;
	}
	
	service->intializeAuthentication([this](const QString& message, int progress) {
		m_splashScreenManager->updateSplashScreenMessage(message, progress);
		});

	service->initializeRisConnections([this](const QString& message, int progress) {
		m_splashScreenManager->updateSplashScreenMessage(message, progress);
		});

	service->loadMainWindow([this](const QString& message, int progress) {
		m_splashScreenManager->updateSplashScreenMessage(message, progress);
		});


	m_splashScreenManager->closeSplashScreen();

	// make sure when application is about to close the ris received signal to close the connection
	service->connectSignalsAndSlots();

	std::optional<Entity::User> user = service->authenticateUser();
	if(user.has_value())
		service->showMainWindow();
	else
		service->closeApplication();  // Close the application if authentication fails
}  

MainAppLaunchStrategy::~MainAppLaunchStrategy()  
{  
}  
}
