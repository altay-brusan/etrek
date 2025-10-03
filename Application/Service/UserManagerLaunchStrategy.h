#ifndef ETREK_APPLICATION_SERVICE_USERMANAGERLAUNCHSTRATEGY_H
#define ETREK_APPLICATION_SERVICE_USERMANAGERLAUNCHSTRATEGY_H

#include <QObject>
#include "ILaunchStrategy.h"


namespace Etrek::Application::Service
{
	class UserManagerLaunchStrategy : public QObject, public ILaunchStrategy
	{
		Q_OBJECT

	public:
		UserManagerLaunchStrategy(QObject* parent);
		void launch(ApplicationService* service) override;
		~UserManagerLaunchStrategy();
	};

}


#endif

