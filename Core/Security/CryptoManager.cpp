#include <openssl/evp.h>
#include <openssl/rand.h>
#include <openssl/sha.h>
#include <openssl/aes.h>
#include <QByteArray>
#include "CryptoManager.h"


namespace Etrek::Core::Security {

constexpr int SALT_SIZE = 16;

// Obfuscated internal key material
static const char *part1 = "60b725f10c9c85c70d97880dfe8191b";
static const char *part2 = "3eabd7c07e3f5c5db10e10b58e8a6548c";
static const char *ivHex = "5f2c0eb88d5f3349321c9b7da5346ec5";

static QByteArray HexToBytes(const QString &hex) {
    return QByteArray::fromHex(hex.toUtf8());
}

QByteArray CryptoManager::getAesKey() {
    QString combined = QString("%1%2").arg(part1, part2);
    return HexToBytes(combined);
}

QByteArray CryptoManager::getAesIv() {
    return HexToBytes(ivHex);
}

static QByteArray DeriveKey(const QByteArray& key, const QByteArray& salt) {
    QByteArray combined = key + salt;
    QByteArray hash(SHA256_DIGEST_LENGTH, 0);
    SHA256(reinterpret_cast<const unsigned char*>(combined.constData()), combined.size(), reinterpret_cast<unsigned char*>(hash.data()));
    return hash;
}

QByteArray CryptoManager::encryptWithSalt(const QByteArray& data, const QByteArray& key, const QByteArray& iv) {
    QByteArray salt(SALT_SIZE, 0);
    RAND_bytes(reinterpret_cast<unsigned char*>(salt.data()), SALT_SIZE);

    QByteArray derivedKey = DeriveKey(key, salt);

    EVP_CIPHER_CTX *ctx = EVP_CIPHER_CTX_new();
    QByteArray ciphertext(data.size() + AES_BLOCK_SIZE, 0);
    int len = 0, ciphertext_len = 0;

    EVP_EncryptInit_ex(ctx, EVP_aes_256_cbc(), NULL, reinterpret_cast<const unsigned char*>(derivedKey.data()), reinterpret_cast<const unsigned char*>(iv.data()));
    EVP_EncryptUpdate(ctx, reinterpret_cast<unsigned char*>(ciphertext.data()), &len, reinterpret_cast<const unsigned char*>(data.data()), data.size());
    ciphertext_len = len;
    EVP_EncryptFinal_ex(ctx, reinterpret_cast<unsigned char*>(ciphertext.data()) + len, &len);
    ciphertext_len += len;
    EVP_CIPHER_CTX_free(ctx);

    ciphertext.resize(ciphertext_len);
    return salt + ciphertext;
}

QByteArray CryptoManager::decryptWithSalt(const QByteArray& encryptedData, const QByteArray& key, const QByteArray& iv) {
    if (encryptedData.size() < SALT_SIZE)
        return QByteArray();

    QByteArray salt = encryptedData.left(SALT_SIZE);
    QByteArray cipherText = encryptedData.mid(SALT_SIZE);
    QByteArray derivedKey = DeriveKey(key, salt);

    EVP_CIPHER_CTX *ctx = EVP_CIPHER_CTX_new();
    QByteArray plaintext(cipherText.size(), 0);
    int len = 0, plaintext_len = 0;

    EVP_DecryptInit_ex(ctx, EVP_aes_256_cbc(), NULL, reinterpret_cast<const unsigned char*>(derivedKey.data()), reinterpret_cast<const unsigned char*>(iv.data()));
    EVP_DecryptUpdate(ctx, reinterpret_cast<unsigned char*>(plaintext.data()), &len, reinterpret_cast<const unsigned char*>(cipherText.data()), cipherText.size());
    plaintext_len = len;
    EVP_DecryptFinal_ex(ctx, reinterpret_cast<unsigned char*>(plaintext.data()) + len, &len);
    plaintext_len += len;
    EVP_CIPHER_CTX_free(ctx);

    plaintext.resize(plaintext_len);
    return plaintext;
}

QString CryptoManager::decryptPassword(const QString &encryptedPassword) {
    QByteArray encryptedData = QByteArray::fromBase64(encryptedPassword.toUtf8());
    QByteArray decrypted = decryptWithSalt(encryptedData, getAesKey(), getAesIv());
    return QString::fromUtf8(decrypted);
}

// Optional: encryption support
QString CryptoManager::encryptPassword(const QString& plainPassword) {
    QByteArray encrypted = encryptWithSalt(plainPassword.toUtf8(), getAesKey(), getAesIv());
    return encrypted.toBase64();
}

} // namespace Etrek::Security
