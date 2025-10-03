#include <QtTest>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include <QFile>
#include <QTextStream>
#include <QRandomGenerator>
#include <memory>
#include "DatabaseInitializer.h"
#include "DatabaseConnectionSetting.h"

using namespace Etrek::Service;

class DatabaseInitializerTest : public QObject
{
    Q_OBJECT

private slots:
    void initTestCase();
    void cleanupTestCase();

    void testInitializeDatabase_createsDbAndTables();

private:
    std::shared_ptr<DatabaseConnectionSetting> m_connectionSettings;
};

void DatabaseInitializerTest::initTestCase()
{

    // TODO: The database initializer depends on the logger being initialized beforehand.
    // This dependency should be explicitly managed in the design.

    auto provider = &LoggerProvider::Instance();
    provider->Initialize(".", QDateTime::currentDateTime().addDays(1));


    m_connectionSettings = std::make_shared<DatabaseConnectionSetting>();
    // Generate a unique test DB name for isolation
    QString testDbName = QString("test_etrek_%1").arg(QRandomGenerator::global()->bounded(100000));
    m_connectionSettings->SetDatabaseName(testDbName);
    m_connectionSettings->SetHostName("localhost");
    m_connectionSettings->SetEtrektUserName("root");
    m_connectionSettings->SetPort(3306);
    m_connectionSettings->SetPassword("Trt123Tst!)");
}

void DatabaseInitializerTest::cleanupTestCase()
{
    QSqlDatabase cleanupDb = QSqlDatabase::addDatabase("QMYSQL", "cleanup_connection");
    cleanupDb.setHostName(m_connectionSettings->GetHostName());
    cleanupDb.setUserName(m_connectionSettings->GetEtrekUserName());
    cleanupDb.setPassword(m_connectionSettings->GetPassword());
    cleanupDb.setDatabaseName("mysql");

    if (cleanupDb.open()) {
        QSqlQuery dropQuery(cleanupDb);
        QString testDbName =m_connectionSettings->GetDatabaseName();
        dropQuery.exec(QString("DROP DATABASE `%1`").arg(testDbName));
        cleanupDb.close();
    }
}

void DatabaseInitializerTest::testInitializeDatabase_createsDbAndTables()
{
    auto customFile = std::make_unique<QFile>("temp_test_script.sql");
    QVERIFY(QFile::exists("temp_test_script.sql"));

    DatabaseInitializer initializer(m_connectionSettings);
    Result<QString> result = initializer.InitializeDatabase(std::move(customFile));

    QVERIFY2(result.isSuccess, qUtf8Printable("Database initialization failed: " + result.message));

    // Confirm connection to newly created DB works
    QSqlDatabase db = QSqlDatabase::addDatabase("QMYSQL", "verification_connection");
    db.setHostName(m_connectionSettings->GetHostName());
    db.setUserName(m_connectionSettings->GetEtrekUserName());
    db.setPassword(m_connectionSettings->GetPassword());
    db.setDatabaseName(m_connectionSettings->GetDatabaseName());
    QVERIFY(db.open());

    QSqlQuery checkQuery("SHOW TABLES", db);
    QVERIFY(checkQuery.next()); // Expect at least one table exists (created by setup.sql)
}


QTEST_APPLESS_MAIN(DatabaseInitializerTest)

#include "tst_DatabaseInitializer.moc"
