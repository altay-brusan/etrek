#include "TranslationProvider.h"
#include <QReadLocker>
#include <QWriteLocker>
#include <QFile>
#include <QJsonDocument>
#include <QJsonObject>
#include <QDebug>

namespace Etrek::Core::Globalization
{
    /**
     * @brief Constructs the TranslationProvider and loads message resources.
     */
    TranslationProvider::TranslationProvider()
    {
        loadFromResource();
    }

    /**
     * @brief Destructor for TranslationProvider.
     */
    TranslationProvider::~TranslationProvider() {}

    /**
     * @brief Gets the singleton instance of TranslationProvider.
     * @return Reference to the singleton instance.
     */
    TranslationProvider& TranslationProvider::Instance()
    {
        static TranslationProvider instance;
        return instance;
    }

    /**
     * @brief Loads message resources from the application's resource files.
     *
     * Reads the JSON file for the current language and populates the message map.
     * Thread-safe via write lock.
     */
    void TranslationProvider::loadFromResource()
    {
        QWriteLocker locker(&m_lock);
        
        QString resourcePath = QString("./lang/%1/messages.json").arg(m_currentLang.isEmpty() ? "en" : m_currentLang);
        QFile file(resourcePath);
        if (!file.open(QIODevice::ReadOnly)) {
            qWarning() << "Failed to load message resource:" << file.errorString();
            return;
        }

        QByteArray data = file.readAll();
        QJsonParseError parseError;
        QJsonDocument doc = QJsonDocument::fromJson(data, &parseError);

        if (parseError.error != QJsonParseError::NoError) {
            qWarning() << "Failed to parse message resource:" << parseError.errorString();
            return;
        }

        if (!doc.isObject()) {
            qWarning() << "Invalid format in message resource.";
            return;
        }

        const QJsonObject root = doc.object();
        for (const QString& category : root.keys()) {
            QJsonObject messages = root.value(category).toObject();
            for (const QString& key : messages.keys()) {
                m_messages[category][key] = messages.value(key).toString();
            }
        }
    }

    /**
     * @brief Retrieves a translated message for the given key and category.
     * @param category The message category.
     * @param key The message key.
     * @return The translated message, or a missing message indicator if not found.
     */
    QString TranslationProvider::getMessage(const QString& category, const QString& key) const
    {
        QReadLocker locker(&m_lock);
        if (m_messages.contains(category) && m_messages[category].contains(key)) {
            return m_messages[category].value(key);
        }
        return QString("[Missing %1:%2]").arg(category, key);
    }

    /**
     * @brief Retrieves a critical message for the given key.
     * @param key The message key.
     * @return The translated critical message.
     */
    QString TranslationProvider::getCriticalMessage(const QString& key) const
    {
        return getMessage("critical", key);
    }

    /**
     * @brief Retrieves an error message for the given key.
     * @param key The message key.
     * @return The translated error message.
     */
    QString TranslationProvider::getErrorMessage(const QString& key) const
    {
        return getMessage("errors", key);
    }

    /**
     * @brief Retrieves a warning message for the given key.
     * @param key The message key.
     * @return The translated warning message.
     */
    QString TranslationProvider::getWarningMessage(const QString& key) const
    {
        return getMessage("warnings", key);
    }

    /**
     * @brief Retrieves a debug message for the given key.
     * @param key The message key.
     * @return The translated debug message.
     */
    QString TranslationProvider::getDebugMessage(const QString& key) const
    {
        return getMessage("debugs", key);
    }

    /**
     * @brief Retrieves an informational message for the given key.
     * @param key The message key.
     * @return The translated info message.
     */
    QString TranslationProvider::getInfoMessage(const QString& key) const {
        return getMessage("info", key);
    }

    void TranslationProvider::setLanguage(const QString& langCode) {
        // Your implementation here
        throw std::logic_error("Not implemented");
    }

}
