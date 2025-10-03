#include <QObject>
#include <QTest>
#include <QDateTime>
#include "AuthenticationRepository.h"
#include "DatabaseConnectionSetting.h"
#include "Result.h"

using namespace Etrek::Repository;

class AuthenticationRepositoryTest : public QObject
{
    Q_OBJECT

private:
    std::shared_ptr<DatabaseConnectionSetting> connectionSetting;
    std::unique_ptr<AuthenticationRepository> manager;
    int createdUserId = -1;
    int createdRoleId = -1;
    QString createdRoleName;

private slots:
    void init() {
        connectionSetting = std::make_shared<DatabaseConnectionSetting>();
        connectionSetting->SetHostName("localhost");
        connectionSetting->SetDatabaseName("etrekdb");
        connectionSetting->SetEtrektUserName("root");
        connectionSetting->SetPassword("Trt123Tst!)");
        connectionSetting->SetPort(3306);
        connectionSetting->SetIsPasswordEncrypted(false);

        manager = std::make_unique<AuthenticationRepository>(connectionSetting);
    }

    void cleanup() {
        // Optional: clean-up can be implemented later
    }

    void test_CreateUser() {
        QString username = "testuser_" + QString::number(QDateTime::currentSecsSinceEpoch());
        QString passwordHash = "hashedpassword123";

        auto result = manager->CreateUser(username, passwordHash);
        QVERIFY(result.isSuccess);
        QVERIFY(result.value > 0);
        createdUserId = result.value;
    }

    void test_CreateRole() {
        createdRoleName = "tester_role_" + QString::number(QDateTime::currentSecsSinceEpoch());
        auto result = manager->CreateRole(createdRoleName);
        QVERIFY(result.isSuccess);
        QVERIFY(result.value > 0);
        createdRoleId = result.value;
    }

    void test_AssignRoleToUser() {
        QVERIFY(createdUserId > 0);
        QVERIFY(createdRoleId > 0);

        auto result = manager->AssignRoleToUser(createdUserId, createdRoleId);
        QVERIFY(result.isSuccess);
    }

    void test_CheckUserHasRole() {
        QVERIFY(createdUserId > 0);
        QVERIFY(!createdRoleName.isEmpty());

        auto result = manager->CheckUserHasRole(createdUserId, createdRoleName);
        QVERIFY(result.isSuccess);
        QVERIFY(result.value); // Should be true
    }

    void test_RemoveRoleFromUser() {
        QVERIFY(createdUserId > 0);
        QVERIFY(createdRoleId > 0);

        auto result = manager->RemoveRoleFromUser(createdUserId, createdRoleId);
        QVERIFY(result.isSuccess);
    }
};

QTEST_APPLESS_MAIN(AuthenticationRepositoryTest)
#include "tst_AuthenticationRepository.moc"
