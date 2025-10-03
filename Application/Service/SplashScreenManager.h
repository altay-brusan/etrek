#ifndef SPLASHSCREENMANAGER_H
#define SPLASHSCREENMANAGER_H


#include <QObject>  
#include "EtrekSplashScreen.h"  

namespace Etrek::Application::Service
{
	class SplashScreenManager : public QObject
	{
		Q_OBJECT

	public:

		SplashScreenManager(QObject* parent = nullptr);

		void displaySplashScreen();
		void closeSplashScreen();
		void updateSplashScreenMessage(const QString& message, int progressIncrement);
		bool isVisible() const;
		void setTitle(const QString& title);

		~SplashScreenManager();

	private:
		EtrekSplashScreen m_splashScreen;
	};
}

#endif // SPLASHSCREENMANAGER_H
