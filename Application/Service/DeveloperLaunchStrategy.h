#ifndef DEVELOPERLAUNCHSTRATEGY_H
#define DEVELOPERLAUNCHSTRATEGY_H


#include <QObject>
#include "ILaunchStrategy.h"

namespace Etrek::Application::Service
{
	class DeveloperLaunchStrategy : public QObject, public ILaunchStrategy
	{
		Q_OBJECT

	public:
		DeveloperLaunchStrategy(QObject* parent);
		void launch(ApplicationService* service) override;
		~DeveloperLaunchStrategy();
	};


}


#endif // DEVELOPERLAUNCHSTRATEGY_H