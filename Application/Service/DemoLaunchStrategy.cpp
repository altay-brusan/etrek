#include <QDebug>
#include "DemoLaunchStrategy.h"
#include "ApplicationService.h"

namespace Etrek::Application::Service
{
	DemoLaunchStrategy::DemoLaunchStrategy(QObject* parent)
		: QObject(parent)
	{
	}

	void DemoLaunchStrategy::launch(ApplicationService* service)
	{
		if (!service) {
			qWarning() << "ApplicationService is null in DemoLaunchStrategy::launch";
			return;
		}

		// Display the main screen
		//service->loadMainWindow(); // the signeture changed.

		qInfo() << "Demo mode launched successfully.";
	}

	DemoLaunchStrategy::~DemoLaunchStrategy()
	{
	}
}
