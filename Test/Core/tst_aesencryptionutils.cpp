#include <QtTest>
#include "AesEncryptionUtils.h"

using namespace Etrek::Security::Crypto;


class AesEncryptionUtilsTest : public QObject
{
    Q_OBJECT

public:
    AesEncryptionUtilsTest();
    ~AesEncryptionUtilsTest();

private slots:
    void testEncryptDecrypt();
};

AesEncryptionUtilsTest::AesEncryptionUtilsTest() {}

AesEncryptionUtilsTest::~AesEncryptionUtilsTest() {}

void AesEncryptionUtilsTest::testEncryptDecrypt()
{
    QByteArray plain = "HelloWorld123";
    QByteArray key = QByteArray::fromHex("00112233445566778899aabbccddeeff00112233445566778899aabbccddeeff");
    QByteArray iv = QByteArray::fromHex("0102030405060708090a0b0c0d0e0f10");

    QByteArray encrypted = AesEncryptionUtils::EncryptWithSalt(plain, key, iv);
    QByteArray decrypted = AesEncryptionUtils::DecryptWithSalt(encrypted, key, iv);

    QCOMPARE(decrypted, plain);
}

QTEST_APPLESS_MAIN(AesEncryptionUtilsTest)

#include "tst_AesEncryptionUtils.moc"

