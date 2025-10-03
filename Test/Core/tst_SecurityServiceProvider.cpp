#include <QtTest>
#include "SecurityServiceProvider.h"
#include "AesEncryptionUtils.h"

using namespace Etrek::Security;

namespace Etrek::Security
{
class SecurityServiceProviderTest : public QObject
{
    Q_OBJECT

public:
    SecurityServiceProviderTest();
    ~SecurityServiceProviderTest();

private slots:
    void test_decryptPassword_returnsOriginalPlaintext();
};

SecurityServiceProviderTest::SecurityServiceProviderTest() {}

SecurityServiceProviderTest::~SecurityServiceProviderTest() {}

void SecurityServiceProviderTest::test_decryptPassword_returnsOriginalPlaintext()
{
    SecurityServiceProvider provider;

    const QString original = "MySecurePassword123";

    // Use actual key and iv
    QByteArray key = provider.GetAesKey();
    QByteArray iv = provider.GetAesIv();

    // Encrypt
    QByteArray encrypted = Crypto::AesEncryptionUtils::EncryptWithSalt(
        original.toUtf8(), key, iv);

    QString encryptedBase64 = encrypted.toBase64();

    // Act
    QString decrypted = provider.DecryptPassword(encryptedBase64);

    // Assert
    QCOMPARE(decrypted, original);
}
}


QTEST_APPLESS_MAIN(SecurityServiceProviderTest)

#include "tst_SecurityServiceProvider.moc"
