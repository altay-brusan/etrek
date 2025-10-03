#include <QtTest>
#include <QBuffer>
#include "SettingProvider.h"
#include "SecurityServiceProvider.h"
#include "AesEncryptionUtils.h"

using namespace Etrek::Service;
using namespace Etrek::Security;
using namespace Etrek::Security::Crypto;
class SettingProviderTests : public QObject
{
    Q_OBJECT

private slots:
    void test_LoadSettings_WithValidJson();
    void test_LoadSettings_WithEncryptedPassword();
    void test_LoadSettings_WithFileLoggerSettings();

};

void SettingProviderTests::test_LoadSettings_WithValidJson()
{
    QByteArray jsonData = R"({
            "DatabaseConnection": {
                "HostName": "localhost",
                "DatabaseName": "TestDb",
                "UserName": "admin",
                "Password": "encrypted_pwd",
                "IsPasswordEncrypted": false
            }
        })";

    auto buffer = std::make_unique<QBuffer>(&jsonData);
    buffer->open(QIODevice::ReadOnly);

    SettingProvider provider;
    bool result = provider.LoadSettings(*buffer);  // dereference smart pointer

    QVERIFY(result);
    auto setting = provider.GetDatabaseConnectionSettings();
    QCOMPARE(setting->GetHostName(), "localhost");
    QCOMPARE(setting->GetDatabaseName(), "TestDb");
    QCOMPARE(setting->GetEtrekUserName(), "admin");
    QCOMPARE(setting->GetPassword(), "encrypted_pwd");
    QCOMPARE(setting->GetIsPasswordEncrypted(), false);
}
void SettingProviderTests::test_LoadSettings_WithEncryptedPassword()
{
    // Encrypt plaintext password
    QString plainPassword = "mySecret123";
    QByteArray key = SecurityServiceProvider::GetAesKey();
    QByteArray iv = SecurityServiceProvider::GetAesIv();
    QByteArray encrypted = Crypto::AesEncryptionUtils::EncryptWithSalt(plainPassword.toUtf8(), key, iv);
    QString encryptedBase64 = encrypted.toBase64();

    QString json = QString(R"({
            "DatabaseConnection": {
                "HostName": "securehost",
                "DatabaseName": "SecureDb",
                "UserName": "secureuser",
                "Password": "%1",
                "IsPasswordEncrypted": true
            }
        })").arg(encryptedBase64);

    QByteArray jsonData = json.toUtf8();
    auto buffer = std::make_unique<QBuffer>(&jsonData);
    buffer->open(QIODevice::ReadOnly);

    SettingProvider provider;
    bool result = provider.LoadSettings(*buffer);

    QVERIFY(result);
    auto setting = provider.GetDatabaseConnectionSettings();
    QCOMPARE(setting->GetHostName(), "securehost");
    QCOMPARE(setting->GetDatabaseName(), "SecureDb");
    QCOMPARE(setting->GetEtrekUserName(), "secureuser");
    QCOMPARE(setting->GetPassword(), plainPassword);  // Should be decrypted
    QCOMPARE(setting->GetIsPasswordEncrypted(), true);
}
void SettingProviderTests::test_LoadSettings_WithFileLoggerSettings()
{
    QString cleanupTimeStr = "2026-12-25T14:30:00";
    QByteArray jsonData = QString(R"({
        "FileLogger": {
            "LogDirectory": "/var/logs/myapp",
            "NextCleanupTime": "%1"
        }
    })").arg(cleanupTimeStr).toUtf8();

    auto buffer = std::make_unique<QBuffer>(&jsonData);
    buffer->open(QIODevice::ReadOnly);

    SettingProvider provider;
    bool result = provider.LoadSettings(*buffer);

    QVERIFY(result);

    auto fileLoggerSettings = provider.GetFileLoggerSettings();
    QVERIFY(fileLoggerSettings != nullptr);
    QCOMPARE(fileLoggerSettings->GetLogDirectory(), QString("/var/logs/myapp"));
    QCOMPARE(fileLoggerSettings->GetNextCleanupTime(), QDateTime::fromString(cleanupTimeStr, Qt::ISODate));
}



QTEST_MAIN(SettingProviderTests)
#include "tst_SettingProvider.moc"
