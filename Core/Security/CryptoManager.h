#ifndef CRYPTOMANAGER_H
#define CRYPTOMANAGER_H

#include <QObject>

/**
 * @namespace Etrek::Security
 * @brief Contains security-related classes and utilities for cryptographic operations.
 */
namespace Etrek::Core::Security
{
    class SecurityServiceProviderTest; // forward declaration

    /**
     * @class CryptoManager
     * @brief Provides static methods for AES-based encryption and decryption.
     *
     * CryptoManager offers utility functions to encrypt and decrypt passwords (or other sensitive data)
     * using AES algorithms. All methods are static and stateless, making this class suitable for use
     * as a general-purpose cryptographic utility. Future extensions may add support for other algorithms
     * or data types.
     */
    class CryptoManager
    {
    public:
        /**
         * @brief Decrypts an AES-encrypted password string.
         * @param encryptedPassword The encrypted password as a QString.
         * @return The decrypted plain password as a QString.
         */
        static QString decryptPassword(const QString& encryptedPassword);

        /**
         * @brief Encrypts a plain password string using AES.
         * @param plainPassword The plain password as a QString.
         * @return The encrypted password as a QString.
         */
        static QString encryptPassword(const QString& plainPassword);

    private:
        /**
         * @brief Retrieves the AES key used for encryption/decryption.
         * @return The AES key as a QByteArray.
         */
        static QByteArray getAesKey();

        /**
         * @brief Retrieves the AES initialization vector (IV).
         * @return The AES IV as a QByteArray.
         */
        static QByteArray getAesIv();

        /**
         * @brief Encrypts data using AES with salt.
         * @param data The data to encrypt.
         * @param key The AES key.
         * @param iv The AES initialization vector.
         * @return The encrypted data as a QByteArray.
         */
        static QByteArray encryptWithSalt(const QByteArray& data, const QByteArray& key, const QByteArray& iv);

        /**
         * @brief Decrypts AES-encrypted data with salt.
         * @param encryptedData The encrypted data.
         * @param key The AES key.
         * @param iv The AES initialization vector.
         * @return The decrypted data as a QByteArray.
         */
        static QByteArray decryptWithSalt(const QByteArray& encryptedData, const QByteArray& key, const QByteArray& iv);

        friend class SecurityServiceProviderTest;
    };
}

#endif // CRYPTOMANAGER_H
