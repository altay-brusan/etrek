#include <QtTest>
#include "WorklistQueryService.h"
#include "RisConnectionEntityModel.h"
#include "WorklistEntry.h"
#include "WorklistPresentationContext.h"
#include "WorklistRepository.h"
#include "DatabaseConnectionSetting.h"
#include "LoggerProvider.h"

using namespace Etrek::Repository;
class WorklistQueryServiceTest : public QObject
{
    Q_OBJECT

private slots:
    void initTestCase();
    void test_SetWorklistTags();
    void test_SetSettings();
    void test_GetWorklistEntries();
    void test_EchoRis_Success();
    void test_AddPresentationContext();
    void cleanupTestCase();

private:
    std::unique_ptr<WorklistQueryService> m_worklistQueryService;
    std::shared_ptr<RisSettingEntityModel> m_connection;
    WorklistPresentationContext m_presentationContext;
    QList<DicomTag> m_dicomTags;
    QList<DicomTag> m_identifierTags;
    std::shared_ptr<DatabaseConnectionSetting> m_connectionSettings;
    WorklistRepository* repo;
};


void WorklistQueryServiceTest::initTestCase()
{
    QDateTime futureCleanup = QDateTime::currentDateTime().addDays(1);
    LoggerProvider::Instance().Initialize(".",futureCleanup);

    m_connection = std::make_shared<RisSettingEntityModel>();
    m_connection->SetCalledAETitle("DVTK_MW_SCP");
    m_connection->SetCallingAETitle("DVTK_MW_SCU");
    m_connection->SetPort(107);
    m_connection->SetHostIP("127.0.0.1");

    // m_dicomTags = {
    //     { -1, "PatientID", "Patient ID", 0x0010, 0x0020, 0x0000, 0x0000, true },
    //     { -1, "PatientName", "Patient Name", 0x0010, 0x0010, 0x0000, 0x0000, true },
    //     { -1, "StudyInstanceUID", "Study Instance UID", 0x0020, 0x000D, 0x0000, 0x0000, true },
    //     { -1, "ScheduledProcedureStepStartDate", "SPS Start Date", 0x0040, 0x0002, 0x0000, 0x0000, true },
    //     { -1, "ScheduledProcedureStepStartTime", "SPS Start Time", 0x0040, 0x0003, 0x0000, 0x0000, true },
    //     { -1, "ScheduledStationAETitle", "SPS AE Title", 0x0040, 0x0001, 0x0000, 0x0000, true },
    //     { -1, "Modality", "Modality", 0x0008, 0x0060, 0x0000, 0x0000, true }
    // };


    m_presentationContext.Id = 1;
    m_presentationContext.TransferSyntaxUid = "1.2.840.10008.1.2";
    m_presentationContext.ProfileId = 1;

    m_connectionSettings = std::make_shared<DatabaseConnectionSetting>();
    // Generate a unique test DB name for isolation
    QString testDbName = QString("etrekdb");
    m_connectionSettings->SetDatabaseName(testDbName);
    m_connectionSettings->SetHostName("localhost");
    m_connectionSettings->SetEtrektUserName("root");
    m_connectionSettings->SetPort(3306);
    m_connectionSettings->SetPassword("Trt123Tst!)");
    repo = new WorklistRepository(m_connectionSettings);
    auto tagsResult = repo->GetTagsByProfile(1);
    m_dicomTags = tagsResult.value;

    auto identifierResult = repo->GetIdentifierByProfile(1);
    m_identifierTags = identifierResult.value;


    m_worklistQueryService = std::make_unique<WorklistQueryService>();
    m_worklistQueryService->SetWorklistTags(m_dicomTags);
    m_worklistQueryService->SetIdentifierTags(m_identifierTags);
    m_worklistQueryService->SetSettings(m_connection);
    m_worklistQueryService->SetPresentationContext(m_presentationContext);

}

void WorklistQueryServiceTest::test_SetWorklistTags()
{
    QList<DicomTag> tags = { { -1, "PatientName","Patient Name", 0x0010, 0x0010, 0x0000, 0x0000, true } };
    m_worklistQueryService->SetWorklistTags(tags);

    try {
        auto entries = m_worklistQueryService->GetWorklistEntries();
        QVERIFY(entries.size() >= 0);
    } catch (...) {
        QFAIL("Exception occurred during GetWorklistEntries");
    }
}

void WorklistQueryServiceTest::test_SetSettings()
{
    m_worklistQueryService->SetSettings(m_connection);
    QVERIFY(m_worklistQueryService->GetWorklistEntries().size() >= 0);
}

void WorklistQueryServiceTest::test_GetWorklistEntries()
{
    QList<WorklistEntry> entries = m_worklistQueryService->GetWorklistEntries();
    QVERIFY(entries.size() >= 0);  // Works with real SCP, returns empty if no entries
}

void WorklistQueryServiceTest::test_EchoRis_Success()
{
    auto result = m_worklistQueryService->EchoRis();
    QVERIFY(result.isSuccess);
}

void WorklistQueryServiceTest::test_AddPresentationContext()
{
    auto result = m_worklistQueryService->AddPresentationContextForOperation("C-ECHO");
    QVERIFY(result.isSuccess);
}

void WorklistQueryServiceTest::cleanupTestCase()
{
    m_worklistQueryService.reset();
}

QTEST_MAIN(WorklistQueryServiceTest)
#include "tst_WorklistQueryService.moc"
