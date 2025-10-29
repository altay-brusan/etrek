#include <QtTest>
#include "Core/Data/Entity/Patient.h"
#include "Core/Data/Entity/Study.h"
#include "Core/Data/Entity/Series.h"
#include "Core/Data/Entity/Image.h"
#include "Worklist/Specification/WorklistEnum.h"

/**
 * @brief Unit tests for status tracking in imaging entities.
 * 
 * This test suite verifies that Patient, Study, Series, and Image entities
 * correctly handle status tracking fields.
 */
class StatusTrackingTest : public QObject
{
    Q_OBJECT

public:
    StatusTrackingTest();
    ~StatusTrackingTest();

private slots:
    void testPatientDefaultStatus();
    void testStudyDefaultStatus();
    void testSeriesDefaultStatus();
    void testImageDefaultStatus();
    
    void testPatientStatusUpdate();
    void testStudyStatusUpdate();
    void testSeriesStatusUpdate();
    void testImageStatusUpdate();
    
    void testStatusReasonAssignment();
    void testStatusEnumValues();
};

StatusTrackingTest::StatusTrackingTest() {}

StatusTrackingTest::~StatusTrackingTest() {}

void StatusTrackingTest::testPatientDefaultStatus()
{
    Etrek::Core::Data::Entity::Patient patient;
    QCOMPARE(patient.Status, ProcedureStepStatus::PENDING);
    QVERIFY(patient.StatusReason.isEmpty());
    QVERIFY(patient.StatusUpdatedAt.isNull());
}

void StatusTrackingTest::testStudyDefaultStatus()
{
    Etrek::Core::Data::Entity::Study study;
    QCOMPARE(study.Status, ProcedureStepStatus::PENDING);
    QVERIFY(study.StatusReason.isEmpty());
    QVERIFY(study.StatusUpdatedAt.isNull());
}

void StatusTrackingTest::testSeriesDefaultStatus()
{
    Etrek::Core::Data::Entity::Series series;
    QCOMPARE(series.Status, ProcedureStepStatus::PENDING);
    QVERIFY(series.StatusReason.isEmpty());
    QVERIFY(series.StatusUpdatedAt.isNull());
}

void StatusTrackingTest::testImageDefaultStatus()
{
    Etrek::Core::Data::Entity::Image image;
    QCOMPARE(image.Status, ProcedureStepStatus::PENDING);
    QVERIFY(image.StatusReason.isEmpty());
    QVERIFY(image.StatusUpdatedAt.isNull());
}

void StatusTrackingTest::testPatientStatusUpdate()
{
    Etrek::Core::Data::Entity::Patient patient;
    patient.Status = ProcedureStepStatus::IN_PROGRESS;
    QCOMPARE(patient.Status, ProcedureStepStatus::IN_PROGRESS);
}

void StatusTrackingTest::testStudyStatusUpdate()
{
    Etrek::Core::Data::Entity::Study study;
    study.Status = ProcedureStepStatus::COMPLETED;
    QCOMPARE(study.Status, ProcedureStepStatus::COMPLETED);
}

void StatusTrackingTest::testSeriesStatusUpdate()
{
    Etrek::Core::Data::Entity::Series series;
    series.Status = ProcedureStepStatus::ABORTED;
    QCOMPARE(series.Status, ProcedureStepStatus::ABORTED);
}

void StatusTrackingTest::testImageStatusUpdate()
{
    Etrek::Core::Data::Entity::Image image;
    image.Status = ProcedureStepStatus::CANCELLED;
    QCOMPARE(image.Status, ProcedureStepStatus::CANCELLED);
}

void StatusTrackingTest::testStatusReasonAssignment()
{
    Etrek::Core::Data::Entity::Patient patient;
    patient.Status = ProcedureStepStatus::ABORTED;
    patient.StatusReason = "Patient requested cancellation";
    patient.StatusUpdatedAt = QDateTime::currentDateTime();
    
    QCOMPARE(patient.Status, ProcedureStepStatus::ABORTED);
    QCOMPARE(patient.StatusReason, QString("Patient requested cancellation"));
    QVERIFY(!patient.StatusUpdatedAt.isNull());
}

void StatusTrackingTest::testStatusEnumValues()
{
    // Test that all status enum values can be assigned
    Etrek::Core::Data::Entity::Study study;
    
    study.Status = ProcedureStepStatus::SCHEDULED;
    QCOMPARE(study.Status, ProcedureStepStatus::SCHEDULED);
    
    study.Status = ProcedureStepStatus::PENDING;
    QCOMPARE(study.Status, ProcedureStepStatus::PENDING);
    
    study.Status = ProcedureStepStatus::IN_PROGRESS;
    QCOMPARE(study.Status, ProcedureStepStatus::IN_PROGRESS);
    
    study.Status = ProcedureStepStatus::COMPLETED;
    QCOMPARE(study.Status, ProcedureStepStatus::COMPLETED);
    
    study.Status = ProcedureStepStatus::CANCELLED;
    QCOMPARE(study.Status, ProcedureStepStatus::CANCELLED);
    
    study.Status = ProcedureStepStatus::ABORTED;
    QCOMPARE(study.Status, ProcedureStepStatus::ABORTED);
}

QTEST_APPLESS_MAIN(StatusTrackingTest)

#include "tst_StatusTrackingTest.moc"
