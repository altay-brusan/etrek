#ifndef MAINAPPLAUNCHSTRATEGY_H
#define MAINAPPLAUNCHSTRATEGY_H


#include <QObject>
#include "ILaunchStrategy.h"

namespace Etrek::Application::Service
{
	class MainAppLaunchStrategy : public QObject, public ILaunchStrategy
	{
		Q_OBJECT

	public:
		MainAppLaunchStrategy(QObject* parent);
		void launch(ApplicationService* service) override;
		~MainAppLaunchStrategy();

	};
}




#endif // MAINAPPLAUNCHSTRATEGY_H
