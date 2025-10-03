#ifndef LOGGERPROVIDER_H
#define LOGGERPROVIDER_H

#include <QMap>
#include <QDateTime>
#include <QString>
#include <memory>
#include <QReadWriteLock>
#include "spdlog/logger.h"
#include "Result.h"
#include "TranslationProvider.h"
#include "LogLevel.h"

using namespace  Etrek::Core::Globalization;
namespace  Etrek::Core::Log {

    /**
     * @class LoggerProvider
     * @brief Provides file-based logging services for the application.
     *
     * LoggerProvider is currently limited to managing file loggers using spdlog's rotating file sink.
     * It is responsible for initializing log file settings, creating and retrieving loggers for services,
     * and ensuring thread-safe access to logger resources.
     *
     * Future extensions may introduce support for other logging backends (e.g., XML, in-memory, network).
     * In such cases, an abstract base class (interface) should be introduced to allow polymorphic use
     * of different logger providers, each with their own initialization and configuration requirements.
     */
    class LoggerProvider : public QObject {
    public:
        /**
         * @brief Returns the singleton instance of LoggerProvider.
         */
        static LoggerProvider& Instance();

        /**
         * @brief Initializes the file logger settings.
         * @param logDir Directory for log files.
         * @param fileSizeMB Maximum size of each log file in Mega Bytes.
         * @param fileCount Maximum number of rotated log files.
         * @param translationProvider Pointer to the translation provider for error messages.
         * @return Result indicating success or failure with a message.
         */
        Result<QString> InitializeFileLogger(const QString& logDir, size_t fileSizeMB, size_t fileCount, TranslationProvider* translationProvider);

        /**
         * @brief Retrieves a file logger for the specified service.
         * @param serviceName Name of the service.
         * @param level Logging level.
         * @return Shared pointer to the spdlog logger instance.
         */
        std::shared_ptr<spdlog::logger> GetFileLogger(const std::string& serviceName, LogLevel level = LogLevel::Debug);

        /**
         * @brief Shuts down all managed loggers and releases resources.
         */
        void Shutdown();

    private:
        explicit LoggerProvider();
        bool HasSufficientDiskSpace(const QString& path);

        QReadWriteLock lock;
        QString m_logDirectory;
        TranslationProvider* translator;
        size_t m_fileSizeBytes = 1 * 1024 * 1024;
        size_t m_fileCount = 5;
        QMap<QString, std::shared_ptr<spdlog::logger>> m_loggerMap;
    };

}

#endif // LOGGERPROVIDER_H
