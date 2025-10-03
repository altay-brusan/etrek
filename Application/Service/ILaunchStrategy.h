#ifndef ILAUNCHSTRATEGY_H
#define ILAUNCHSTRATEGY_H

#include "SplashScreenManager.h"
namespace Etrek::Application::Service
{
    class ApplicationService;
    class ILaunchStrategy
    {
    public:
        virtual ~ILaunchStrategy() = default;
        virtual void launch(ApplicationService* service) = 0;

    protected:
        SplashScreenManager* m_splashScreenManager = nullptr;

    };
}


#endif // ILAUNCHSTRATEGY_H