#ifndef ETREK_APPLICATION_SERVICE_SETTINGMANAGERLAUNCHSTRATEGY_H
#define ETREK_APPLICATION_SERVICE_SETTINGMANAGERLAUNCHSTRATEGY_H

#include <QObject>
#include "ILaunchStrategy.h"

namespace Etrek::Application::Service
{
	class SettingManagerLaunchStrategy : public QObject, public ILaunchStrategy
	{
		Q_OBJECT

	public:
		SettingManagerLaunchStrategy(QObject* parent);
		void launch(ApplicationService* service) override;
		~SettingManagerLaunchStrategy();
	};
}

#endif // ETREK_APPLICATION_SERVICE_SETTINGMANAGERLAUNCHSTRATEGY_H