#include <QObject>
#include <QTemporaryDir>
#include <QTest>
#include "LoggerProvider.h"
#include "AppLoggerFactory.h"

using namespace Etrek::Service;

class AppLoggerFactoryTest : public QObject
{
    Q_OBJECT

private slots:
    void init();
    void cleanup();

    void testCreateLogger();

private:
    QTemporaryDir tempDir;
    LoggerProvider* provider = nullptr;
};

void AppLoggerFactoryTest::init()
{
    provider = &LoggerProvider::Instance();
    provider->Initialize(tempDir.path(), QDateTime::currentDateTime().addDays(1));
}

void AppLoggerFactoryTest::cleanup()
{
    provider->Shutdown();
}

void AppLoggerFactoryTest::testCreateLogger()
{
    AppLoggerFactory factory(*provider);
    std::shared_ptr<AppLogger> logger = factory.CreateLogger("FactoryTestService");

    logger->LogInfo("Factory-generated log entry");

    QFile file(tempDir.path() + "/logs.log");
    QVERIFY(file.exists());
    QVERIFY(file.size() > 0);
}

QTEST_APPLESS_MAIN(AppLoggerFactoryTest)
#include "tst_AppLoggerFactory.moc"
