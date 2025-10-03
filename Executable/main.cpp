#include <QApplication>
#include <QSharedMemory>
#include <QMessageBox>
#include <csignal>
#include <QDebug>
#include <QCoreApplication>
#include "ApplicationService.h"
#include "LoggerProvider.h"
#include "LaunchMode.h"


#define _CRTDBG_MAP_ALLOC
#include <cstdlib>
#include <crtdbg.h>

// Optional: redirect new to track file/line
#ifdef _DEBUG
#define new new(_NORMAL_BLOCK, __FILE__, __LINE__)
#endif


void signalHandler(int signal)
{
    auto& logger = Etrek::Core::Log::LoggerProvider::Instance();
    //logger.GetFileLogger("CrashHandler")->critical("Application received signal: {}", signal);
    logger.Shutdown();

    // Optional: Dump more diagnostics or write to a file
    std::abort();  // Or std::exit(signal); for graceful exit
}


int main(int argc, char *argv[])
{
    _CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
   // _CrtSetBreakAlloc(1102360); // replace with allocation number from your leak dump
    QApplication a(argc, argv);

    // Single instance enforcement
    QSharedMemory sharedMemory("EtrekAppUniqueKey");
    if (!sharedMemory.create(1)) {
        QMessageBox::warning(nullptr, "Already Running", "Another instance of this application is already running.");
        return 1;
    }
   
    // Handle OS signals for graceful shutdown
    std::signal(SIGINT, signalHandler);   // Ctrl+C
    std::signal(SIGTERM, signalHandler);  // kill <pid>
    std::signal(SIGSEGV, signalHandler);  // segmentation fault (unsafe to handle, but optional)

    QObject::connect(&a, &QCoreApplication::aboutToQuit, []() {
        Etrek::Core::Log::LoggerProvider::Instance().Shutdown();
    });

    QStringList arguments = QCoreApplication::arguments();
    LaunchMode mode = ParseLaunchMode(arguments);

    Etrek::Application::Service::ApplicationService applicationService(&a);
    applicationService.initialize(mode);

    return a.exec();
}
