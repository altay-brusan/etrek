#ifndef APPLOGGER_H
#define APPLOGGER_H

#include <QString>
#include <exception>
#include "LogLevel.h"
#include "LoggerProvider.h"
#include "TranslationProvider.h"

namespace  Etrek::Core::Log{

    namespace glb = Etrek::Core::Globalization;
    /**
     * @class AppLogger
     * @brief Facade for application-level logging.
     *
     * AppLogger provides a simplified, domain-specific interface for logging messages
     * within the application. It wraps the underlying spdlog logger and exposes methods
     * for logging at various levels (debug, info, warning, error, exception).
     *
     * The facade pattern is used to shield application code from direct dependency on
     * spdlog and to provide a consistent logging API. AppLogger also supports message
     * formatting and integration with other services (e.g., translation).
     */
    class AppLogger {
    public:
        /**
         * @brief Constructs an AppLogger for the given service.
         * @param serviceName Name of the service.
         * @param provider Reference to the LoggerProvider.
         * @param translator Reference to the translator.
         */
        AppLogger(const QString& serviceName, LoggerProvider& provider, glb::TranslationProvider* translator);

        /**
         * @brief Logs a message at the specified log level.
         * @param message The message to log.
         * @param level The log level.
         */
        void Log(const QString& message, LogLevel level);

        /**
         * @brief Logs an exception message.
         * @param message The message to log.
         * @param ex The exception object.
         */
        void LogException(const QString& message, const std::exception& ex);

        /**
         * @brief Logs a debug message.
         * @param message The message to log.
         */
        void LogDebug(const QString& message);

        /**
         * @brief Logs an informational message.
         * @param message The message to log.
         */
        void LogInfo(const QString& message);

        /**
         * @brief Logs a warning message.
         * @param message The message to log.
         */
        void LogWarning(const QString& message);

        /**
         * @brief Logs an error message.
         * @param message The message to log.
         */
        void LogError(const QString& message);

    private:
        QString m_serviceName;
		glb::TranslationProvider* translator = nullptr; // Pointer to the translation provider for localized messages
        LoggerProvider* m_provider = nullptr;
    };

}


#endif // APPLOGGER_H
