#include <QObject>
#include <QTest>
#include <QDebug>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include <QDateTime>
#include <QRandomGenerator>
#include "WorklistRepository.h"
#include "DatabaseConnectionSetting.h"
#include "Result.h"
#include "WorklistEntry.h"
#include "DicomTag.h"
#include <QTimeZone>

using namespace Etrek::Repository;
using namespace Etrek::Model;

class WorklistRepositoryTest : public QObject
{
    Q_OBJECT

public:
    explicit WorklistRepositoryTest(QObject* parent = nullptr) : QObject(parent) {}

private:
    std::shared_ptr<DatabaseConnectionSetting> connectionSetting;
    std::unique_ptr<WorklistRepository> manager;

    // Helper function to generate random date between years
    QDateTime randomDateTimeBetween(int startYear, int endYear) {
        QRandomGenerator *generator = QRandomGenerator::global();
        int year = generator->bounded(startYear, endYear + 1);
        int month = generator->bounded(1, 13);
        int day = generator->bounded(1, QDate(year, month, 1).daysInMonth() + 1);
        int hour = generator->bounded(0, 24);
        int minute = generator->bounded(0, 60);
        int second = generator->bounded(0, 60);

        return QDateTime(QDate(year, month, day), QTime(hour, minute, second));
    }

    // Helper function to get tags by profile ID
    Result<QList<DicomTag>> getTags(int profileId) {
        auto getTagsResult = manager->GetTagsByProfile(profileId);
        if (!getTagsResult.isSuccess || getTagsResult.value.isEmpty()) {
            return Result<QList<DicomTag>>::Failure("Failed to fetch tags");
        }
        return getTagsResult;
    }

    // Helper function to create a worklist entry
    WorklistEntry createWorklistEntry(const DicomTag& patientNameTag, const DicomTag& admissionIdTag) {
        WorklistProfile profile;
        profile.Id = 1;  // Assuming we have a profile with ID 1 in the database
        profile.Name = "DxWorklist";

        WorklistAttribute patientNameAttribute;
        patientNameAttribute.Tag = patientNameTag;
        patientNameAttribute.TagValue = "John Doe";

        WorklistAttribute admissionIdAttribute;
        admissionIdAttribute.Tag = admissionIdTag;
        admissionIdAttribute.TagValue = "12345";

        WorklistEntry worklist;
        worklist.Source = Source::LOCAL;
        worklist.Status = ProcedureStepStatus::SCHEDULED;
        worklist.Profile = profile;
        worklist.CreatedAt = QDateTime::currentDateTime();
        worklist.UpdatedAt = QDateTime::currentDateTime();
        worklist.Attributes.append(patientNameAttribute);
        worklist.Attributes.append(admissionIdAttribute);

        return worklist;
    }


    // Helper function for generating a random name for tags
    QString generateRandomTagName() {
        return "Test Tag " + QString::number(QRandomGenerator::global()->bounded(1000000));
    }

private slots:
    void initTestCase() {
        connectionSetting = std::make_shared<DatabaseConnectionSetting>();
        connectionSetting->SetHostName("localhost");
        connectionSetting->SetDatabaseName("etrekdb");
        connectionSetting->SetEtrektUserName("root");
        connectionSetting->SetPassword("Trt123Tst!)");
        connectionSetting->SetPort(3306);
        connectionSetting->SetIsPasswordEncrypted(false);

        manager = std::make_unique<WorklistRepository>(connectionSetting);
    }

    void cleanupTestCase() {}

    void test_CreateWorklist() {
        auto getTagsResult = getTags(1);
        QVERIFY(getTagsResult.isSuccess);

        DicomTag patientNameTag = getTagsResult.value[0];  // Example, you can refine based on name
        DicomTag admissionIdTag = getTagsResult.value[1];  // Example, you can refine based on name

        // Create worklist entry
        WorklistEntry worklist = createWorklistEntry(patientNameTag, admissionIdTag);

        // Now, call the CreateWorklistEntry function
        auto result = manager->CreateWorklistEntry(worklist);

        // Assertions
        QVERIFY(result.isSuccess);
        QVERIFY(result.value > 0);
    }

    void test_UpdateWorklistStatus() {
        auto getTagsResult = getTags(1);
        QVERIFY(getTagsResult.isSuccess);

        DicomTag patientNameTag = getTagsResult.value[0];
        DicomTag admissionIdTag = getTagsResult.value[1];

        WorklistEntry worklist = createWorklistEntry(patientNameTag, admissionIdTag);

        auto createResult = manager->CreateWorklistEntry(worklist);
        QVERIFY(createResult.isSuccess);

        int worklistId = createResult.value;
        ProcedureStepStatus newStatus = ProcedureStepStatus::COMPLETED;
        auto updateResult = manager->UpdateWorklistStatus(worklistId, newStatus);
        QVERIFY(updateResult.isSuccess);

        auto fetchResult = manager->GetWorklistEntryById(worklistId);
        QVERIFY(fetchResult.isSuccess);
        QCOMPARE(fetchResult.value.Status, newStatus);
    }

    void test_DeleteWorklistEntries_ByDate() {
        auto getTagsResult = getTags(1);
        QVERIFY(getTagsResult.isSuccess);

        DicomTag patientNameTag = getTagsResult.value[0];
        DicomTag admissionIdTag = getTagsResult.value[1];

        // Create a worklist entry with a specific CreatedAt date (1967-12-01)
        // Create a QDateTime with a specific date and time
        QDateTime createdAt(QDate(1967, 1, 12), QTime(12, 10, 1), QTimeZone("Europe/Istanbul"));
        // Set the timezone for the QDateTime object (e.g., UTC)


        WorklistEntry worklist = createWorklistEntry(patientNameTag, admissionIdTag);
        worklist.CreatedAt = createdAt;

        auto createResult = manager->CreateWorklistEntry(worklist);
        QVERIFY(createResult.isSuccess);

        //QDateTime deleteDate = QDateTime::currentDateTime();
        auto deleteResult = manager->DeleteWorklistEntries(createdAt);
        QVERIFY(deleteResult.isSuccess);

        auto fetchResult = manager->GetWorklistEntryById(createResult.value);
        QVERIFY(!fetchResult.isSuccess);  // Entry should be deleted
    }

    void test_DeleteWorklistEntries_ByIds() {
        auto getTagsResult = getTags(1);
        QVERIFY(getTagsResult.isSuccess);

        DicomTag patientNameTag = getTagsResult.value[0];
        DicomTag admissionIdTag = getTagsResult.value[1];

        WorklistEntry worklist = createWorklistEntry(patientNameTag, admissionIdTag);

        auto createResult = manager->CreateWorklistEntry(worklist);
        QVERIFY(createResult.isSuccess);

        QList<int> entryIds = { createResult.value };
        auto deleteResult = manager->DeleteWorklistEntries(entryIds);
        QVERIFY(deleteResult.isSuccess);

        auto fetchResult = manager->GetWorklistEntryById(createResult.value);
        QVERIFY(!fetchResult.isSuccess);  // Entry should be deleted
    }

    void test_AddDicomTag() {
        DicomTag tag;
        tag.Name = generateRandomTagName();
        tag.DisplayName = "Display Name "+ tag.Name;
        tag.GroupHex = 0x0008;
        tag.ElementHex = 0x0050;
        tag.IsActive = true;
        tag.IsRetired = false;

        auto result = manager->AddDicomTag(tag);
        QVERIFY(result.isSuccess);
        QVERIFY(result.value > 0);
    }

    void test_UpdateDicomTagActiveStatus() {
        DicomTag tag;
        tag.Name = generateRandomTagName();
        tag.DisplayName = "Display Name " + tag.Name;
        tag.GroupHex = 0x0008;
        tag.ElementHex = 0x0050;
        tag.IsActive = true;
        tag.IsRetired = false;

        auto addResult = manager->AddDicomTag(tag);
        QVERIFY(addResult.isSuccess);

        int tagId = addResult.value;
        auto updateResult = manager->UpdateDicomTagActiveStatus(tagId, false);
        QVERIFY(updateResult.isSuccess);

        auto tagResult = manager->GetTagsByProfile(1);
        QVERIFY(tagResult.isSuccess);
        QVERIFY(std::none_of(tagResult.value.begin(), tagResult.value.end(),
                             [tagId](const DicomTag& t) { return t.Id == tagId && t.IsActive; }));
    }

    void test_UpdateDicomTagRetiredStatus() {
        DicomTag tag;
        tag.Name = generateRandomTagName();
        tag.DisplayName = "Display name " + tag.Name;
        tag.GroupHex = 0x0008;
        tag.ElementHex = 0x0050;
        tag.IsActive = true;
        tag.IsRetired = false;

        auto addResult = manager->AddDicomTag(tag);
        QVERIFY(addResult.isSuccess);

        int tagId = addResult.value;
        auto updateResult = manager->UpdateDicomTagRetiredStatus(tagId, true);
        QVERIFY(updateResult.isSuccess);

        auto tagResult = manager->GetTagsByProfile(1);
        QVERIFY(tagResult.isSuccess);
        QVERIFY(std::none_of(tagResult.value.begin(), tagResult.value.end(),
                             [tagId](const DicomTag& t) { return t.Id == tagId && t.IsRetired == false; }));
    }
};

QTEST_APPLESS_MAIN(WorklistRepositoryTest)
#include "tst_WorklistRepository.moc"
