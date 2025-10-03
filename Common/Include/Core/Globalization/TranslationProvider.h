#ifndef TRANSLATIONPROVIDER_H
#define TRANSLATIONPROVIDER_H

#include <QString>
#include <QMap>
#include <QReadWriteLock>
#include <QJsonObject>


namespace Etrek::Core::Globalization
{
    /**
     * @class TranslationProvider
     * @brief Provides localized message translation for the application.
     *
     * Loads message resources from JSON files and supplies translated messages
     * for various categories (critical, error, warning, debug, info, general).
     * The message levels are ordered as follows: Debug < Info < Warning < Error < Critical.
     * Thread-safe access is ensured via read/write locks.
     */
    class TranslationProvider
    {
        public:
            /**
             * @brief Retrieves a translated message for the given key and category.
             * @param key The message key.
             * @param category The message category (default: "general").
             * @return The translated message, or a missing message indicator if not found.
             */
            QString getMessage(const QString& key, const QString& category = "general") const;

            /**
             * @brief Retrieves a critical message for the given key.
             * @param key The message key.
             * @return The translated critical message.
             */
            QString getCriticalMessage(const QString& key) const;

            /**
             * @brief Retrieves an error message for the given key.
             * @param key The message key.
             * @return The translated error message.
             */
            QString getErrorMessage(const QString& key) const;

            /**
             * @brief Retrieves a warning message for the given key.
             * @param key The message key.
             * @return The translated warning message.
             */
            QString getWarningMessage(const QString& key) const;

            /**
             * @brief Retrieves a debug message for the given key.
             * @param key The message key.
             * @return The translated debug message.
             */
            QString getDebugMessage(const QString& key) const;

            /**
             * @brief Retrieves an informational message for the given key.
             * @param key The message key.
             * @return The translated info message.
             */
            QString getInfoMessage(const QString& key) const;

            /**
             * @brief Sets the current language for message translation.
             * @param langCode The language code (e.g., "en", "tr").
             */
            void setLanguage(const QString& langCode);

            /**
             * @brief Constructs the TranslationProvider and loads message resources.
             */
            TranslationProvider();

            /**
             * @brief Destructor for TranslationProvider.
             */
            ~TranslationProvider();

            /**
             * @brief Loads message resources from the application's resource files.
             */
            void loadFromResource();

            /**
             * @brief Gets the singleton instance of TranslationProvider.
             * @return Reference to the singleton instance.
             */
            static TranslationProvider& Instance();

        private:
            QString m_currentLang = "en"; ///< Current language code.
            mutable QReadWriteLock m_lock; ///< Read/write lock for thread-safe access.
            QMap<QString, QMap<QString, QString>> m_messages; ///< Message storage by category and key.
    };
}




#endif // TRANSLATIONPROVIDER_H
