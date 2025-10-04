#include "SplashScreenManager.h"

namespace Etrek::Application::Service
{
	void SplashScreenManager::displaySplashScreen()
	{		
		m_splashScreen.show();		
	}

	void SplashScreenManager::closeSplashScreen()
	{
		if (m_splashScreen.isVisible()) {
			m_splashScreen.close();
		}
	}

	void SplashScreenManager::updateSplashScreenMessage(const QString& message, int progressIncrement)
	{
		if (m_splashScreen.isVisible()) {
			m_splashScreen.setMessage(message);
			m_splashScreen.incrementProgress(progressIncrement);
		}
	}

	bool SplashScreenManager::isVisible() const
	{
		return m_splashScreen.isVisible(); 
	}

	void SplashScreenManager::setTitle(const QString& title)
	{
		m_splashScreen.setTitle(title);
	}

	SplashScreenManager::SplashScreenManager(QObject* parent)
		: QObject(parent)
	{
	}

	SplashScreenManager::~SplashScreenManager()
	{
	}


}
