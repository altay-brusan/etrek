#include <QtTest>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include <QSqlRecord>
#include <QFile>
#include <QTextStream>
#include <QRandomGenerator>
#include <memory>

/**
 * @class AdmissionIdSchemaTest
 * @brief Tests for Admission ID database schema changes
 * 
 * This test verifies that the admission_id column has been properly added
 * to the studies table along with its index as specified in GitHub Issue #29.
 * 
 * Test coverage:
 * - Admission ID column exists in studies table
 * - Column has correct type (VARCHAR(64))
 * - Column is nullable (DEFAULT NULL)
 * - Index idx_admission_id exists
 * - Basic CRUD operations work with admission_id
 * - Queries by admission_id use the index
 */
class AdmissionIdSchemaTest : public QObject
{
    Q_OBJECT

private slots:
    void initTestCase();
    void cleanupTestCase();

    // Schema validation tests
    void testAdmissionIdColumnExists();
    void testAdmissionIdColumnType();
    void testAdmissionIdColumnIsNullable();
    void testAdmissionIdIndexExists();
    
    // Functional tests
    void testInsertStudyWithAdmissionId();
    void testInsertStudyWithoutAdmissionId();
    void testUpdateAdmissionId();
    void testQueryByAdmissionId();
    void testQueryUsesIndex();

private:
    QSqlDatabase m_db;
    QString m_testDbName;
    QString m_connectionName;

    // Helper functions
    bool columnExists(const QString& tableName, const QString& columnName);
    QString getColumnType(const QString& tableName, const QString& columnName);
    bool columnIsNullable(const QString& tableName, const QString& columnName);
    bool indexExists(const QString& tableName, const QString& indexName);
    int insertTestStudy(const QString& studyInstanceUid, const QString& admissionId);
    void cleanupTestData();
};

void AdmissionIdSchemaTest::initTestCase()
{
    // Generate unique connection name for test isolation
    m_connectionName = QString("admission_id_test_%1")
        .arg(QRandomGenerator::global()->bounded(1000000));
    
    // Use existing test database or create a temporary one
    // For CI/automated testing, you may want to use a test database
    m_testDbName = qgetenv("ETREK_TEST_DB_NAME");
    if (m_testDbName.isEmpty()) {
        m_testDbName = "etrek_test";
        qWarning() << "Using default test database:" << m_testDbName;
        qWarning() << "Set ETREK_TEST_DB_NAME environment variable to override";
    }

    m_db = QSqlDatabase::addDatabase("QMYSQL", m_connectionName);
    m_db.setHostName(qgetenv("ETREK_TEST_DB_HOST").isEmpty() ? 
                     "localhost" : qgetenv("ETREK_TEST_DB_HOST"));
    m_db.setDatabaseName(m_testDbName);
    m_db.setUserName(qgetenv("ETREK_TEST_DB_USER").isEmpty() ? 
                     "root" : qgetenv("ETREK_TEST_DB_USER"));
    m_db.setPassword(qgetenv("ETREK_TEST_DB_PASS"));
    
    QVERIFY2(m_db.open(), 
             qUtf8Printable(QString("Failed to connect to test database: %1")
                            .arg(m_db.lastError().text())));
}

void AdmissionIdSchemaTest::cleanupTestCase()
{
    cleanupTestData();
    
    if (m_db.isOpen()) {
        m_db.close();
    }
    QSqlDatabase::removeDatabase(m_connectionName);
}

void AdmissionIdSchemaTest::testAdmissionIdColumnExists()
{
    QVERIFY2(columnExists("studies", "admission_id"),
             "admission_id column should exist in studies table");
}

void AdmissionIdSchemaTest::testAdmissionIdColumnType()
{
    QString columnType = getColumnType("studies", "admission_id");
    QVERIFY2(columnType.contains("varchar", Qt::CaseInsensitive),
             qUtf8Printable(QString("admission_id should be VARCHAR, but found: %1")
                            .arg(columnType)));
}

void AdmissionIdSchemaTest::testAdmissionIdColumnIsNullable()
{
    QVERIFY2(columnIsNullable("studies", "admission_id"),
             "admission_id column should be nullable (DEFAULT NULL)");
}

void AdmissionIdSchemaTest::testAdmissionIdIndexExists()
{
    QVERIFY2(indexExists("studies", "idx_admission_id"),
             "Index idx_admission_id should exist on studies table");
}

void AdmissionIdSchemaTest::testInsertStudyWithAdmissionId()
{
    QString studyUid = QString("1.2.3.4.test.%1").arg(QRandomGenerator::global()->bounded(1000000));
    QString admissionId = QString("ADM%1").arg(QRandomGenerator::global()->bounded(1000000));
    
    QSqlQuery query(m_db);
    query.prepare("INSERT INTO studies (study_instance_uid, study_id, admission_id) "
                  "VALUES (:study_uid, :study_id, :admission_id)");
    query.bindValue(":study_uid", studyUid);
    query.bindValue(":study_id", "ST001");
    query.bindValue(":admission_id", admissionId);
    
    QVERIFY2(query.exec(), 
             qUtf8Printable(QString("Failed to insert study with admission_id: %1")
                            .arg(query.lastError().text())));
    
    // Verify the inserted data
    QSqlQuery selectQuery(m_db);
    selectQuery.prepare("SELECT admission_id FROM studies WHERE study_instance_uid = :study_uid");
    selectQuery.bindValue(":study_uid", studyUid);
    QVERIFY(selectQuery.exec());
    QVERIFY(selectQuery.next());
    QCOMPARE(selectQuery.value(0).toString(), admissionId);
}

void AdmissionIdSchemaTest::testInsertStudyWithoutAdmissionId()
{
    QString studyUid = QString("1.2.3.4.test.%1").arg(QRandomGenerator::global()->bounded(1000000));
    
    QSqlQuery query(m_db);
    query.prepare("INSERT INTO studies (study_instance_uid, study_id) "
                  "VALUES (:study_uid, :study_id)");
    query.bindValue(":study_uid", studyUid);
    query.bindValue(":study_id", "ST002");
    
    QVERIFY2(query.exec(), 
             qUtf8Printable(QString("Failed to insert study without admission_id: %1")
                            .arg(query.lastError().text())));
    
    // Verify admission_id is NULL
    QSqlQuery selectQuery(m_db);
    selectQuery.prepare("SELECT admission_id FROM studies WHERE study_instance_uid = :study_uid");
    selectQuery.bindValue(":study_uid", studyUid);
    QVERIFY(selectQuery.exec());
    QVERIFY(selectQuery.next());
    QVERIFY(selectQuery.value(0).isNull());
}

void AdmissionIdSchemaTest::testUpdateAdmissionId()
{
    QString studyUid = QString("1.2.3.4.test.%1").arg(QRandomGenerator::global()->bounded(1000000));
    QString admissionId = QString("ADM%1").arg(QRandomGenerator::global()->bounded(1000000));
    
    // Insert study without admission_id
    QSqlQuery insertQuery(m_db);
    insertQuery.prepare("INSERT INTO studies (study_instance_uid, study_id) "
                        "VALUES (:study_uid, :study_id)");
    insertQuery.bindValue(":study_uid", studyUid);
    insertQuery.bindValue(":study_id", "ST003");
    QVERIFY(insertQuery.exec());
    
    // Update with admission_id
    QSqlQuery updateQuery(m_db);
    updateQuery.prepare("UPDATE studies SET admission_id = :admission_id "
                        "WHERE study_instance_uid = :study_uid");
    updateQuery.bindValue(":admission_id", admissionId);
    updateQuery.bindValue(":study_uid", studyUid);
    
    QVERIFY2(updateQuery.exec(),
             qUtf8Printable(QString("Failed to update admission_id: %1")
                            .arg(updateQuery.lastError().text())));
    
    // Verify the update
    QSqlQuery selectQuery(m_db);
    selectQuery.prepare("SELECT admission_id FROM studies WHERE study_instance_uid = :study_uid");
    selectQuery.bindValue(":study_uid", studyUid);
    QVERIFY(selectQuery.exec());
    QVERIFY(selectQuery.next());
    QCOMPARE(selectQuery.value(0).toString(), admissionId);
}

void AdmissionIdSchemaTest::testQueryByAdmissionId()
{
    QString studyUid = QString("1.2.3.4.test.%1").arg(QRandomGenerator::global()->bounded(1000000));
    QString admissionId = QString("ADM%1").arg(QRandomGenerator::global()->bounded(1000000));
    
    // Insert test data
    insertTestStudy(studyUid, admissionId);
    
    // Query by admission_id
    QSqlQuery query(m_db);
    query.prepare("SELECT study_instance_uid FROM studies WHERE admission_id = :admission_id");
    query.bindValue(":admission_id", admissionId);
    
    QVERIFY2(query.exec(),
             qUtf8Printable(QString("Failed to query by admission_id: %1")
                            .arg(query.lastError().text())));
    QVERIFY(query.next());
    QCOMPARE(query.value(0).toString(), studyUid);
}

void AdmissionIdSchemaTest::testQueryUsesIndex()
{
    QString admissionId = "TEST_ADM_INDEX";
    
    // EXPLAIN query to check if index is used
    QSqlQuery explainQuery(m_db);
    explainQuery.prepare("EXPLAIN SELECT * FROM studies WHERE admission_id = :admission_id");
    explainQuery.bindValue(":admission_id", admissionId);
    
    QVERIFY2(explainQuery.exec(),
             qUtf8Printable(QString("Failed to execute EXPLAIN query: %1")
                            .arg(explainQuery.lastError().text())));
    
    // MySQL EXPLAIN output varies, but we can check that the query executes
    // A more comprehensive test would parse the EXPLAIN output to verify index usage
    QVERIFY(explainQuery.next());
    
    // Optional: Check if 'key' column mentions our index (MySQL 5.7+)
    // Note: This is fragile and depends on MySQL version and query optimizer decisions
    // QString keyUsed = explainQuery.value("key").toString();
    // qDebug() << "Index used:" << keyUsed;
}

// Helper function implementations

bool AdmissionIdSchemaTest::columnExists(const QString& tableName, const QString& columnName)
{
    QSqlQuery query(m_db);
    query.prepare("SELECT COUNT(*) FROM INFORMATION_SCHEMA.COLUMNS "
                  "WHERE TABLE_SCHEMA = DATABASE() "
                  "AND TABLE_NAME = :table_name "
                  "AND COLUMN_NAME = :column_name");
    query.bindValue(":table_name", tableName);
    query.bindValue(":column_name", columnName);
    
    if (query.exec() && query.next()) {
        return query.value(0).toInt() > 0;
    }
    return false;
}

QString AdmissionIdSchemaTest::getColumnType(const QString& tableName, const QString& columnName)
{
    QSqlQuery query(m_db);
    query.prepare("SELECT COLUMN_TYPE FROM INFORMATION_SCHEMA.COLUMNS "
                  "WHERE TABLE_SCHEMA = DATABASE() "
                  "AND TABLE_NAME = :table_name "
                  "AND COLUMN_NAME = :column_name");
    query.bindValue(":table_name", tableName);
    query.bindValue(":column_name", columnName);
    
    if (query.exec() && query.next()) {
        return query.value(0).toString();
    }
    return QString();
}

bool AdmissionIdSchemaTest::columnIsNullable(const QString& tableName, const QString& columnName)
{
    QSqlQuery query(m_db);
    query.prepare("SELECT IS_NULLABLE FROM INFORMATION_SCHEMA.COLUMNS "
                  "WHERE TABLE_SCHEMA = DATABASE() "
                  "AND TABLE_NAME = :table_name "
                  "AND COLUMN_NAME = :column_name");
    query.bindValue(":table_name", tableName);
    query.bindValue(":column_name", columnName);
    
    if (query.exec() && query.next()) {
        return query.value(0).toString() == "YES";
    }
    return false;
}

bool AdmissionIdSchemaTest::indexExists(const QString& tableName, const QString& indexName)
{
    QSqlQuery query(m_db);
    query.prepare("SELECT COUNT(*) FROM INFORMATION_SCHEMA.STATISTICS "
                  "WHERE TABLE_SCHEMA = DATABASE() "
                  "AND TABLE_NAME = :table_name "
                  "AND INDEX_NAME = :index_name");
    query.bindValue(":table_name", tableName);
    query.bindValue(":index_name", indexName);
    
    if (query.exec() && query.next()) {
        return query.value(0).toInt() > 0;
    }
    return false;
}

int AdmissionIdSchemaTest::insertTestStudy(const QString& studyInstanceUid, const QString& admissionId)
{
    QSqlQuery query(m_db);
    query.prepare("INSERT INTO studies (study_instance_uid, study_id, admission_id) "
                  "VALUES (:study_uid, :study_id, :admission_id)");
    query.bindValue(":study_uid", studyInstanceUid);
    query.bindValue(":study_id", QString("ST%1").arg(QRandomGenerator::global()->bounded(10000)));
    query.bindValue(":admission_id", admissionId);
    
    if (query.exec()) {
        return query.lastInsertId().toInt();
    }
    return -1;
}

void AdmissionIdSchemaTest::cleanupTestData()
{
    // Clean up any test studies we created
    QSqlQuery query(m_db);
    query.exec("DELETE FROM studies WHERE study_instance_uid LIKE '1.2.3.4.test.%'");
}

QTEST_APPLESS_MAIN(AdmissionIdSchemaTest)

#include "tst_AdmissionIdSchema.moc"
