#include <QApplication>
#include <QCoreApplication>
#include <QMessageBox>
#include <QLockFile>
#include <QStandardPaths>
#include <QDir>
#include <csignal>
#include <atomic>
#include <vector>

#include "ApplicationService.h"
#include "LaunchMode.h"
#include "LoggerProvider.h"

#ifdef _MSC_VER
#define _CRTDBG_MAP_ALLOC
#include <crtdbg.h>
static void enableLeakChecks() {
    _CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
    // Optional: _CrtSetReportMode(_CRT_ERROR, _CRTDBG_MODE_DEBUG);
}
#else
static void enableLeakChecks() {}
#endif

namespace {
    std::atomic<int> g_lastSignal{ 0 };

    void handleSignal(int sig) {
        g_lastSignal.store(sig, std::memory_order_relaxed);
        // _Exit is the safe “get out now” button.
        std::_Exit(EXIT_FAILURE);
    }


    /*
    * 
    * If a graceful shutdown (run cleanup/aboutToQuit) needed,
    * don’t _Exit in the handler. Instead, set a flag and let
    * the main thread call QCoreApplication::quit():
    * global
    *    static volatile sig_atomic_t g_gotSignal = 0;
    *
    *    void handleSignal(int sig) {
    *        g_gotSignal = sig;      // async-signal-safe
    *    }
    *
    *    QTimer timer;
    *    QObject::connect(&timer, &QTimer::timeout, [&]{
    *        if (g_gotSignal) {
    *            // do minimal, safe app-level handling here
    *            QCoreApplication::quit();
    *        }
    *    });
    *    timer.start(100);

    */
    
    class SignalGuard {
    public:
        explicit SignalGuard(std::initializer_list<int> sigs) {
            m_handlers.reserve(sigs.size());
            for (int s : sigs) {
                m_handlers.push_back({ s, std::signal(s, handleSignal) });
            }
        }
        ~SignalGuard() {
            for (const auto& h : m_handlers) std::signal(h.signal, h.previous);
        }
        SignalGuard(const SignalGuard&) = delete;
        SignalGuard& operator=(const SignalGuard&) = delete;
        SignalGuard(SignalGuard&&) = delete;
        SignalGuard& operator=(SignalGuard&&) = delete;
    private:
        struct HandlerMapping { int signal; void (*previous)(int); };
        std::vector<HandlerMapping> m_handlers;
    };
}

int main(int argc, char* argv[])
{
    enableLeakChecks();

    QApplication app(argc, argv);
    QCoreApplication::setApplicationName(QStringLiteral("EtrekApp"));

    // Prefer QLockFile for single-instance
    const QString lockPath =
        QStandardPaths::writableLocation(QStandardPaths::TempLocation) + QLatin1Char('/') +
        QCoreApplication::applicationName() + QLatin1String(".lock");
    QLockFile lock(lockPath);
    lock.setStaleLockTime(0);
    if (!lock.tryLock(0)) {
        QMessageBox::warning(nullptr, QObject::tr("Already Running"),
            QObject::tr("Another instance of this application is already running."));
        return EXIT_FAILURE;
    }

    // Handle OS signals (keep minimal)
    const SignalGuard signalGuard({ SIGINT, SIGTERM /*, SIGSEGV (avoid) */ });

    QObject::connect(&app, &QCoreApplication::aboutToQuit, []() {
        Etrek::Core::Log::LoggerProvider::Instance().Shutdown();
        });

    const QStringList arguments = QCoreApplication::arguments();
    const auto mode = Etrek::Specification::ParseLaunchMode(arguments);

    Etrek::Application::Service::ApplicationService applicationService(&app);
    try {
        applicationService.initialize(mode);
    }
    catch (const std::exception& ex) {
        QMessageBox::critical(nullptr, QObject::tr("Fatal error"),
            QObject::tr("Failed to initialize application: %1").arg(QString::fromUtf8(ex.what())));
        return EXIT_FAILURE;
    }
    catch (...) {
        QMessageBox::critical(nullptr, QObject::tr("Fatal error"),
            QObject::tr("Failed to initialize application due to an unknown error."));
        return EXIT_FAILURE;
    }

    return app.exec();
}
