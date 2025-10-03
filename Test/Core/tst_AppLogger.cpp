#include <QObject>
#include <QTemporaryDir>
#include <QFile>
#include <QTextStream>
#include <QTest>
#include "AppLogger.h"
#include "LoggerProvider.h"

using namespace Etrek::Service;

class AppLoggerTest : public QObject
{
    Q_OBJECT

private slots:
    void init();
    void cleanup();

    void testLogLevels();
    void testLogException();

private:
    QTemporaryDir tempDir;
    LoggerProvider* provider = nullptr;
};

void AppLoggerTest::init()
{
    provider = &LoggerProvider::Instance();
    QDateTime future = QDateTime::currentDateTime().addDays(1);
    provider->Initialize(tempDir.path(), future);
}

void AppLoggerTest::cleanup()
{
    provider->Shutdown();
}

void AppLoggerTest::testLogLevels()
{
    AppLogger logger("AppLogService", *provider);
    logger.LogDebug("Debug message");
    logger.LogInfo("Info message");
    logger.LogWarning("Warning message");
    logger.LogError("Error message");

    auto logFilePath = tempDir.path() + "/logs.log";
    QFile logFile(logFilePath);
    QVERIFY(logFile.exists());
    QVERIFY(logFile.size() > 0);
}

void AppLoggerTest::testLogException()
{
    AppLogger logger("AppExceptionService", *provider);
    try {
        throw std::runtime_error("Sample exception");
    } catch (const std::exception& ex) {
        logger.LogException("Exception caught", ex);
    }

    auto logFilePath = tempDir.path() + "/logs.log";
    QFile logFile(logFilePath);
    QVERIFY(logFile.exists());
    QVERIFY(logFile.size() > 0);
}

QTEST_APPLESS_MAIN(AppLoggerTest)
#include "tst_AppLogger.moc"
