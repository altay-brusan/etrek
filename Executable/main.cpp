#include <QApplication>
#include <QCoreApplication>
#include <QMessageBox>
#include <QSharedMemory>
#include <csignal>
#include <initializer_list>
#include <vector>

#include "ApplicationService.h"
#include "LaunchMode.h"
#include "LoggerProvider.h"


#define _CRTDBG_MAP_ALLOC
#include <cstdlib>
#ifdef _MSC_VER
#include <crtdbg.h>
#endif

// Optional: redirect new to track file/line
#ifdef _DEBUG
#define new new(_NORMAL_BLOCK, __FILE__, __LINE__)
#endif

namespace
{
    constexpr char kSingleInstanceKey[] = "EtrekAppUniqueKey";

#ifdef _MSC_VER
    void enableLeakChecks()
    {
        _CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
    }
#else
    void enableLeakChecks() {}
#endif

    void handleSignal(int signal)
    {
        auto& logger = Etrek::Core::Log::LoggerProvider::Instance();
        logger.Shutdown();
        std::abort();
    }

    class SignalGuard
    {
    public:
        SignalGuard() = default;

        SignalGuard(const SignalGuard&) = delete;
        SignalGuard& operator=(const SignalGuard&) = delete;
        SignalGuard(SignalGuard&&) = delete;
        SignalGuard& operator=(SignalGuard&&) = delete;

        explicit SignalGuard(std::initializer_list<int> sigs)
        {
            handlers_.reserve(sigs.size());
            for (int sig : sigs) {
                handlers_.push_back({sig, std::signal(sig, handleSignal)});
            }
        }

        ~SignalGuard()
        {
            for (const auto& handler : handlers_) {
                std::signal(handler.signal, handler.previous);
            }
        }

    private:
        struct HandlerMapping
        {
            int signal;
            void (*previous)(int);
        };

        std::vector<HandlerMapping> handlers_;
    };
}


int main(int argc, char *argv[])
{
    enableLeakChecks();
    QApplication a(argc, argv);

    // Single instance enforcement
    QSharedMemory sharedMemory(QString::fromLatin1(kSingleInstanceKey));

    if (!sharedMemory.create(1)) {
        QMessageBox::warning(nullptr, "Already Running", "Another instance of this application is already running.");
        return 1;
    }
   
    // Handle OS signals for graceful shutdown
    const SignalGuard signalGuard({SIGINT, SIGTERM, SIGSEGV});

    QObject::connect(&a, &QCoreApplication::aboutToQuit, []() {
        Etrek::Core::Log::LoggerProvider::Instance().Shutdown();
    });

    QStringList arguments = QCoreApplication::arguments();
    const Etrek::Specification::LaunchMode mode = Etrek::Specification::ParseLaunchMode(arguments);

    Etrek::Application::Service::ApplicationService applicationService(&a);
    try {
        applicationService.initialize(mode);
    } catch (const std::exception& ex) {
        QMessageBox::critical(nullptr, "Fatal error", QStringLiteral("Failed to initialize application: %1").arg(ex.what()));
        return EXIT_FAILURE;
    } catch (...) {
        QMessageBox::critical(nullptr, "Fatal error", QStringLiteral("Failed to initialize application due to an unknown error."));
        return EXIT_FAILURE;
    }

    return a.exec();
}
