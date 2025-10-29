#include <QtTest>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include <QRandomGenerator>
#include <memory>

/**
 * @brief Test suite for mwl_attributes foreign key constraint behavior
 * 
 * This test validates the fix for the FK constraint on dicom_tag_id:
 * - AC1: Deleting mwl_attributes does not delete dicom_tags
 * - AC2: Deleting dicom_tags referenced by mwl_attributes is blocked
 * - AC3: Deleting mwl_entries cascades to mwl_attributes
 */
class MwlAttributesForeignKeyTest : public QObject
{
    Q_OBJECT

private slots:
    void initTestCase();
    void cleanupTestCase();
    void init();
    void cleanup();

    // AC1: Child deletion never removes parent
    void testDeletingMwlAttributesDoesNotDeleteDicomTags();
    
    // AC2: Foreign key restriction prevents tag deletion
    void testDeletingReferencedDicomTagIsBlocked();
    
    // AC3: Cascade from mwl_entries to mwl_attributes
    void testDeletingMwlEntryCascadesToAttributes();

private:
    QString m_connectionName;
    QString m_testDbName;
    QSqlDatabase m_db;

    // Helper methods
    int createTestDicomTag(const QString& name);
    int createTestMwlProfile();
    int createTestMwlEntry(int profileId);
    void createTestMwlAttribute(int entryId, int tagId, const QString& value);
    int countDicomTags(int tagId);
    int countMwlAttributes(int entryId);
};

void MwlAttributesForeignKeyTest::initTestCase()
{
    // Generate unique connection and DB names for test isolation
    m_connectionName = QString("test_conn_%1").arg(QRandomGenerator::global()->bounded(1000000));
    m_testDbName = QString("test_etrek_fk_%1").arg(QRandomGenerator::global()->bounded(1000000));

    // Connect to MySQL server (without database)
    QSqlDatabase setupDb = QSqlDatabase::addDatabase("QMYSQL", m_connectionName + "_setup");
    setupDb.setHostName("localhost");
    setupDb.setUserName("root");
    setupDb.setPassword("Trt123Tst!)");
    
    QVERIFY2(setupDb.open(), qPrintable("Failed to connect to MySQL: " + setupDb.lastError().text()));

    // Create test database
    QSqlQuery createDbQuery(setupDb);
    QVERIFY2(createDbQuery.exec(QString("CREATE DATABASE `%1`").arg(m_testDbName)),
             qPrintable("Failed to create test database: " + createDbQuery.lastError().text()));
    
    setupDb.close();
    QSqlDatabase::removeDatabase(m_connectionName + "_setup");

    // Connect to test database
    m_db = QSqlDatabase::addDatabase("QMYSQL", m_connectionName);
    m_db.setHostName("localhost");
    m_db.setUserName("root");
    m_db.setPassword("Trt123Tst!)");
    m_db.setDatabaseName(m_testDbName);
    
    QVERIFY2(m_db.open(), qPrintable("Failed to open test database: " + m_db.lastError().text()));

    // Create minimal schema for testing (only tables we need)
    QSqlQuery schemaQuery(m_db);
    
    // Create mwl_profiles table
    QVERIFY(schemaQuery.exec(
        "CREATE TABLE mwl_profiles ("
        "  id INT AUTO_INCREMENT PRIMARY KEY,"
        "  name VARCHAR(255) NOT NULL UNIQUE"
        ")"));

    // Create dicom_tags table
    QVERIFY(schemaQuery.exec(
        "CREATE TABLE dicom_tags ("
        "  id INT AUTO_INCREMENT PRIMARY KEY,"
        "  name VARCHAR(255) NOT NULL UNIQUE,"
        "  display_name VARCHAR(255) NOT NULL,"
        "  group_hex INT UNSIGNED NOT NULL,"
        "  element_hex INT UNSIGNED NOT NULL,"
        "  pgroup_hex INT UNSIGNED NOT NULL,"
        "  pelement_hex INT UNSIGNED NOT NULL,"
        "  is_active BOOLEAN DEFAULT TRUE,"
        "  is_retired BOOLEAN DEFAULT FALSE"
        ")"));

    // Create mwl_entries table
    QVERIFY(schemaQuery.exec(
        "CREATE TABLE mwl_entries ("
        "  id INT AUTO_INCREMENT PRIMARY KEY,"
        "  source ENUM('LOCAL', 'RIS') NOT NULL,"
        "  profile_id INT NULL,"
        "  status ENUM('SCHEDULED', 'PENDING', 'COMPLETED', 'CANCELLED', 'IN_PROGRESS', 'ABORTED') DEFAULT 'PENDING',"
        "  study_instance_uid VARCHAR(64),"
        "  created_at DATETIME DEFAULT NULL,"
        "  updated_at DATETIME DEFAULT NULL,"
        "  FOREIGN KEY (profile_id) REFERENCES mwl_profiles(id) ON DELETE SET NULL"
        ")"));

    // Create mwl_attributes table with the FIXED FK constraints
    QVERIFY(schemaQuery.exec(
        "CREATE TABLE mwl_attributes ("
        "  id INT AUTO_INCREMENT PRIMARY KEY,"
        "  mwl_entry_id INT NOT NULL,"
        "  dicom_tag_id INT NOT NULL,"
        "  tag_value VARCHAR(512) DEFAULT NULL,"
        "  FOREIGN KEY (mwl_entry_id) REFERENCES mwl_entries(id) ON DELETE CASCADE,"
        "  FOREIGN KEY (dicom_tag_id) REFERENCES dicom_tags(id) ON DELETE RESTRICT"
        ")"));
}

void MwlAttributesForeignKeyTest::cleanupTestCase()
{
    if (m_db.isOpen()) {
        m_db.close();
    }
    QSqlDatabase::removeDatabase(m_connectionName);

    // Drop test database
    QSqlDatabase cleanupDb = QSqlDatabase::addDatabase("QMYSQL", m_connectionName + "_cleanup");
    cleanupDb.setHostName("localhost");
    cleanupDb.setUserName("root");
    cleanupDb.setPassword("Trt123Tst!)");
    
    if (cleanupDb.open()) {
        QSqlQuery dropQuery(cleanupDb);
        dropQuery.exec(QString("DROP DATABASE IF EXISTS `%1`").arg(m_testDbName));
        cleanupDb.close();
    }
    QSqlDatabase::removeDatabase(m_connectionName + "_cleanup");
}

void MwlAttributesForeignKeyTest::init()
{
    // Each test starts with a clean slate
}

void MwlAttributesForeignKeyTest::cleanup()
{
    // Clean up test data after each test
    QSqlQuery query(m_db);
    query.exec("DELETE FROM mwl_attributes");
    query.exec("DELETE FROM mwl_entries");
    query.exec("DELETE FROM dicom_tags");
    query.exec("DELETE FROM mwl_profiles");
}

// AC1: Verify that deleting child (mwl_attributes) does not delete parent (dicom_tags)
void MwlAttributesForeignKeyTest::testDeletingMwlAttributesDoesNotDeleteDicomTags()
{
    // Setup: Create a dicom tag, mwl entry, and attribute
    int tagId = createTestDicomTag("TestTag_AC1");
    int profileId = createTestMwlProfile();
    int entryId = createTestMwlEntry(profileId);
    createTestMwlAttribute(entryId, tagId, "test_value");

    // Verify setup
    QCOMPARE(countDicomTags(tagId), 1);
    QCOMPARE(countMwlAttributes(entryId), 1);

    // Action: Delete the mwl_attribute
    QSqlQuery deleteQuery(m_db);
    deleteQuery.prepare("DELETE FROM mwl_attributes WHERE mwl_entry_id = :entryId");
    deleteQuery.bindValue(":entryId", entryId);
    QVERIFY2(deleteQuery.exec(), qPrintable("Failed to delete attribute: " + deleteQuery.lastError().text()));

    // Assert: dicom_tag should still exist (child deletion does NOT cascade to parent)
    QCOMPARE(countDicomTags(tagId), 1);
}

// AC2: Verify that deleting a referenced dicom_tag is blocked by FK RESTRICT
void MwlAttributesForeignKeyTest::testDeletingReferencedDicomTagIsBlocked()
{
    // Setup: Create a dicom tag and reference it in mwl_attributes
    int tagId = createTestDicomTag("TestTag_AC2");
    int profileId = createTestMwlProfile();
    int entryId = createTestMwlEntry(profileId);
    createTestMwlAttribute(entryId, tagId, "test_value");

    // Verify setup
    QCOMPARE(countDicomTags(tagId), 1);
    QCOMPARE(countMwlAttributes(entryId), 1);

    // Action: Attempt to delete the dicom_tag
    QSqlQuery deleteQuery(m_db);
    deleteQuery.prepare("DELETE FROM dicom_tags WHERE id = :tagId");
    deleteQuery.bindValue(":tagId", tagId);
    
    // Assert: Delete should FAIL due to FK RESTRICT constraint
    bool deleteSucceeded = deleteQuery.exec();
    QVERIFY2(!deleteSucceeded, "Delete should have failed due to FK RESTRICT constraint");
    
    // Verify the error is a foreign key constraint violation
    QString errorText = deleteQuery.lastError().text();
    QVERIFY2(errorText.contains("foreign key constraint", Qt::CaseInsensitive) ||
             errorText.contains("Cannot delete", Qt::CaseInsensitive),
             qPrintable("Expected FK constraint error, got: " + errorText));

    // Verify both tag and attribute still exist
    QCOMPARE(countDicomTags(tagId), 1);
    QCOMPARE(countMwlAttributes(entryId), 1);
}

// AC3: Verify that deleting mwl_entry cascades to mwl_attributes
void MwlAttributesForeignKeyTest::testDeletingMwlEntryCascadesToAttributes()
{
    // Setup: Create entry with multiple attributes
    int tagId1 = createTestDicomTag("TestTag_AC3_1");
    int tagId2 = createTestDicomTag("TestTag_AC3_2");
    int profileId = createTestMwlProfile();
    int entryId = createTestMwlEntry(profileId);
    createTestMwlAttribute(entryId, tagId1, "value1");
    createTestMwlAttribute(entryId, tagId2, "value2");

    // Verify setup
    QCOMPARE(countMwlAttributes(entryId), 2);

    // Action: Delete the mwl_entry
    QSqlQuery deleteQuery(m_db);
    deleteQuery.prepare("DELETE FROM mwl_entries WHERE id = :entryId");
    deleteQuery.bindValue(":entryId", entryId);
    QVERIFY2(deleteQuery.exec(), qPrintable("Failed to delete entry: " + deleteQuery.lastError().text()));

    // Assert: All attributes should be cascaded (deleted)
    QCOMPARE(countMwlAttributes(entryId), 0);
    
    // Assert: dicom_tags should still exist (not affected by attribute deletion)
    QCOMPARE(countDicomTags(tagId1), 1);
    QCOMPARE(countDicomTags(tagId2), 1);
}

// Helper: Create a test dicom tag
int MwlAttributesForeignKeyTest::createTestDicomTag(const QString& name)
{
    QSqlQuery query(m_db);
    query.prepare(
        "INSERT INTO dicom_tags (name, display_name, group_hex, element_hex, pgroup_hex, pelement_hex) "
        "VALUES (:name, :displayName, :group, :element, :pgroup, :pelement)");
    query.bindValue(":name", name);
    query.bindValue(":displayName", name);
    query.bindValue(":group", 0x0010);
    query.bindValue(":element", 0x0020);
    query.bindValue(":pgroup", 0x0000);
    query.bindValue(":pelement", 0x0000);
    
    QVERIFY2(query.exec(), qPrintable("Failed to create dicom tag: " + query.lastError().text()));
    return query.lastInsertId().toInt();
}

// Helper: Create a test mwl profile
int MwlAttributesForeignKeyTest::createTestMwlProfile()
{
    QSqlQuery query(m_db);
    QString profileName = QString("TestProfile_%1").arg(QRandomGenerator::global()->bounded(1000000));
    query.prepare("INSERT INTO mwl_profiles (name) VALUES (:name)");
    query.bindValue(":name", profileName);
    
    QVERIFY2(query.exec(), qPrintable("Failed to create profile: " + query.lastError().text()));
    return query.lastInsertId().toInt();
}

// Helper: Create a test mwl entry
int MwlAttributesForeignKeyTest::createTestMwlEntry(int profileId)
{
    QSqlQuery query(m_db);
    query.prepare(
        "INSERT INTO mwl_entries (source, profile_id, status) "
        "VALUES ('LOCAL', :profileId, 'PENDING')");
    query.bindValue(":profileId", profileId);
    
    QVERIFY2(query.exec(), qPrintable("Failed to create entry: " + query.lastError().text()));
    return query.lastInsertId().toInt();
}

// Helper: Create a test mwl attribute
void MwlAttributesForeignKeyTest::createTestMwlAttribute(int entryId, int tagId, const QString& value)
{
    QSqlQuery query(m_db);
    query.prepare(
        "INSERT INTO mwl_attributes (mwl_entry_id, dicom_tag_id, tag_value) "
        "VALUES (:entryId, :tagId, :value)");
    query.bindValue(":entryId", entryId);
    query.bindValue(":tagId", tagId);
    query.bindValue(":value", value);
    
    QVERIFY2(query.exec(), qPrintable("Failed to create attribute: " + query.lastError().text()));
}

// Helper: Count dicom tags by ID
int MwlAttributesForeignKeyTest::countDicomTags(int tagId)
{
    QSqlQuery query(m_db);
    query.prepare("SELECT COUNT(*) FROM dicom_tags WHERE id = :tagId");
    query.bindValue(":tagId", tagId);
    
    if (query.exec() && query.next()) {
        return query.value(0).toInt();
    }
    return -1; // Error
}

// Helper: Count mwl attributes by entry ID
int MwlAttributesForeignKeyTest::countMwlAttributes(int entryId)
{
    QSqlQuery query(m_db);
    query.prepare("SELECT COUNT(*) FROM mwl_attributes WHERE mwl_entry_id = :entryId");
    query.bindValue(":entryId", entryId);
    
    if (query.exec() && query.next()) {
        return query.value(0).toInt();
    }
    return -1; // Error
}

QTEST_APPLESS_MAIN(MwlAttributesForeignKeyTest)

#include "tst_MwlAttributesForeignKey.moc"
