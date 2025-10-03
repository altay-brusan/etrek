#include <QtTest>
#include <QSignalSpy>
#include <memory>

#include "RisManager.h"
#include "RisConnectionEntityModel.h"
#include "WorklistRepository.h"
#include "WorklistProfile.h"
#include "LoggerProvider.h"

using namespace Etrek::Repository;

class RisManagerIntegrationTest : public QObject
{
    Q_OBJECT

private slots:
    void initTestCase();
    void test_PeriodicFetch_InsertsEntries();
    void cleanupTestCase();

private:
    std::unique_ptr<RisManager> m_manager;
    std::shared_ptr<WorklistRepository> m_realRepo;
    std::shared_ptr<RisSettingEntityModel> m_connection;
    std::shared_ptr<DatabaseConnectionSetting> m_dbSettings;

    WorklistProfile m_testProfile;
};

void RisManagerIntegrationTest::initTestCase()
{
    QDateTime futureCleanup = QDateTime::currentDateTime().addDays(1);
    LoggerProvider::Instance().Initialize(".", futureCleanup);

    // Setup DB connection
    m_dbSettings = std::make_shared<DatabaseConnectionSetting>();
    m_dbSettings->SetDatabaseName("etrekdb");
    m_dbSettings->SetHostName("localhost");
    m_dbSettings->SetEtrektUserName("root");
    m_dbSettings->SetPort(3306);
    m_dbSettings->SetPassword("Trt123Tst!)");

    m_realRepo = std::make_shared<WorklistRepository>(m_dbSettings);

    // Setup RIS connection
    m_connection = std::make_shared<RisSettingEntityModel>();
    m_connection->SetCalledAETitle("DVTK_MW_SCP");
    m_connection->SetCallingAETitle("DVTK_MW_SCU");
    m_connection->SetPort(107);
    m_connection->SetHostIP("127.0.0.1");

    auto profiles = m_realRepo->GetProfiles();
    QVERIFY(profiles.isSuccess);
    QVERIFY(!profiles.value.isEmpty());
    m_testProfile = profiles.value.first();

    // Construct RisManager
    m_manager = std::make_unique<RisManager>(m_realRepo, m_connection);
    m_manager->SetActiveProfile(m_testProfile);  // Prepares thread but does not query yet
}

void RisManagerIntegrationTest::test_PeriodicFetch_InsertsEntries()
{
    m_manager->StartWorklistQueryFromRis();  // Triggers query via QTimer::singleShot

    // Give time for RIS fetch to complete
    QTest::qWait(6000);  // Adjust as needed depending on how fast DICOM query returns

    // Validate that worklist entries were inserted
    auto result = m_realRepo->GetWorklistEntries(nullptr, nullptr);
    QVERIFY(result.isSuccess);

    qDebug() << "Number of entries fetched:" << result.value.size();
    QVERIFY(result.value.size() > 0);
}

void RisManagerIntegrationTest::cleanupTestCase()
{
    m_manager->StopWorklistQueryFromRis();
    m_manager.reset();
    m_realRepo.reset();
}

QTEST_MAIN(RisManagerIntegrationTest)
#include "tst_RisManagerIntegrationTest.moc"
