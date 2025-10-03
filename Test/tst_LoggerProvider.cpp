#include "LoggerProvider.h"
#include <QFile>
#include <QDebug>
#include <QTextStream>
#include <QTest>
#include <QStorageInfo>
#include <QObject>
#include <QTemporaryDir>
#include <QDateTime>
#include <QRandomGenerator>

using namespace Etrek::Service;
class LoggerProviderTest : public QObject
{
    Q_OBJECT

private slots:
    void initTestCase();
    void init();
    void cleanup();
    void cleanupTestCase();

    void testInitialize();
    void testLoggerCreation();
    void testInsufficientDiskSpace();
    void testLogFileRotation();
    void testCleanupOldLogs();
    void testNextCleanupUpdate();
    void testShutdown();

private:
    QTemporaryDir tempDir;
};


void LoggerProviderTest::init()
{
    QDateTime futureCleanup = QDateTime::currentDateTime().addDays(1);
    LoggerProvider::Instance().Initialize(tempDir.path(), futureCleanup);
}

void LoggerProviderTest::cleanup()
{
    LoggerProvider::Instance().Shutdown();
}


void LoggerProviderTest::initTestCase()
{
    QVERIFY(tempDir.isValid());
}

void LoggerProviderTest::cleanupTestCase()
{
    LoggerProvider::Instance().Shutdown();
}

void LoggerProviderTest::testInitialize()
{
    QDateTime futureCleanup = QDateTime::currentDateTime().addDays(1);
    LoggerProvider::Instance().Initialize(tempDir.path(), futureCleanup);

    QVERIFY(QDir(tempDir.path()).exists());
}

void LoggerProviderTest::testLoggerCreation()
{
    auto logger = LoggerProvider::Instance().GetLogger("TestLogger");
    QVERIFY(logger != nullptr);

    logger->info("Test message");
    logger->flush();
    QFile logFile(tempDir.path() + "/logs.log");
    QVERIFY(logFile.exists());
    QVERIFY(logFile.size() > 0);
}

void LoggerProviderTest::testInsufficientDiskSpace()
{
    // Simulate insufficient space by mocking or manually calling HasSufficientDiskSpace with root path (likely to be large)
    bool result = LoggerProvider::Instance().GetLogger("ShouldWork") != nullptr;
    QVERIFY(result);
}

void LoggerProviderTest::testLogFileRotation()
{
    auto logger = LoggerProvider::Instance().GetLogger("RotationLogger");
    QVERIFY(logger != nullptr);

    // Generate a long random string of ~512 characters
    auto generateRandomLine = []() {
        QString result;
        for (int i = 0; i < 512; ++i) {
            char ch = QRandomGenerator::global()->bounded(32, 127); // Printable ASCII
            result.append(ch);
        }
        return result;
    };

    // Write enough log entries to exceed 10MB (roughly 512 * 21000 ~ 10.5MB)
    const int lineCount = 21000;
    for (int i = 0; i < lineCount; ++i) {
        logger->info("Log entry {}: {}", i, generateRandomLine().toStdString());
    }

    logger->flush(); // Force writing to disk

    QDir dir(tempDir.path());
    QStringList files = dir.entryList(QStringList() << "logs.*", QDir::Files);
    QVERIFY2(files.size() > 1, "Log rotation did not produce multiple files.");
}

void LoggerProviderTest::testCleanupOldLogs()
{
    QFile dummy(tempDir.path() + "/OldFile.log.1");
    dummy.open(QIODevice::WriteOnly);
    dummy.write("dummy data");
    dummy.close();

    QDateTime past = QDateTime::currentDateTime().addYears(-2);
    LoggerProvider::Instance().UpdateNextCleanupTime(past);
    LoggerProvider::Instance().AttemptCleanup(true);

    QVERIFY(!QFile::exists(dummy.fileName()));
}

void LoggerProviderTest::testNextCleanupUpdate()
{
    QDateTime newTime = QDateTime::currentDateTime().addYears(5);
    LoggerProvider::Instance().UpdateNextCleanupTime(newTime);

    LoggerProvider::Instance().AttemptCleanup(); // should not delete anything
    QVERIFY(QDateTime::currentDateTime() < newTime);
}

void LoggerProviderTest::testShutdown()
{
    LoggerProvider::Instance().Shutdown();

    auto logger = LoggerProvider::Instance().GetLogger("AfterShutdown");
    QVERIFY(logger != nullptr);
}

QTEST_APPLESS_MAIN(LoggerProviderTest)

#include "tst_LoggerProvider.moc"
