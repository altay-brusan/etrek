#ifndef ETREK_APPLICATION_SERVICE_DEMO_LAUNCH_STRATEGY_H
#define ETREK_APPLICATION_SERVICE_DEMO_LAUNCH_STRATEGY_H


#include <QObject>

#include "ILaunchStrategy.h"


namespace Etrek::Application::Service
{
	class DemoLaunchStrategy : public QObject, public ILaunchStrategy
	{
		Q_OBJECT

	public:
		DemoLaunchStrategy(QObject* parent);
		void launch(ApplicationService* service) override;
		~DemoLaunchStrategy();
	};
}




#endif //ETREK_APPLICATION_SERVICE_DEMO_LAUNCH_STRATEGY_H
