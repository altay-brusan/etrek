#include <QDir>
#include <QFileInfo>
#include <QStorageInfo>
#include <QDebug>
#include "LoggerProvider.h"
#include "spdlog/spdlog.h"
#include "spdlog/sinks/rotating_file_sink.h"
#include "MessageKey.h"
#include "LogLevel.h"


namespace Etrek::Core::Log {
    LoggerProvider& LoggerProvider::Instance()
    {
        static LoggerProvider instance;
        return instance;
    }

    LoggerProvider::LoggerProvider()
    {}

    Result<QString> LoggerProvider::InitializeFileLogger(const QString& logDir, size_t fileSizeMB, size_t fileCount, TranslationProvider* translator)
    {
        if(fileSizeMB > 1)
        {
            m_fileSizeBytes = fileSizeMB * 1024 * 1024;
		}

        if (fileCount > 1)
        {
            m_fileCount = fileCount;
        }
		
        this->translator = translator;
        
        spdlog::set_pattern("[%Y-%m-%d %H:%M:%S.%e] [%n] [%l] %v");
        
        QDir dir(logDir);
        if (!dir.exists()) {
            if (!dir.mkpath(".")) {
				qWarning() << translator->getErrorMessage(LOG_DIR_CREATION_FAILED_ERROR).arg(logDir);
				return Result<QString>::Failure(translator->getErrorMessage(LOG_DIR_CREATION_FAILED_ERROR).arg(logDir));
            }
        }

        // At this point, dir.exists() should be true if creation succeeded
        if (!dir.exists()) {
            qWarning() << translator->getErrorMessage(LOG_DIR_NOT_EXIST_AFTER_CREATION_ERROR).arg(logDir);
            return Result<QString>::Failure(translator->getErrorMessage(LOG_DIR_NOT_EXIST_AFTER_CREATION_ERROR).arg(logDir));
        }
        
        m_logDirectory = logDir;    
        return Result<QString>::Failure(translator->getInfoMessage(LOG_FILE_LOGGER_INIT_SUCCEED_MSG));
    }

    std::shared_ptr<spdlog::logger> LoggerProvider::GetFileLogger(const std::string& serviceName, LogLevel level)
    {

        QReadLocker readLocker(&lock);
        QString qName = QString::fromStdString(serviceName);
        if (m_loggerMap.contains(qName)) {
            return m_loggerMap[qName];
        }
        readLocker.unlock();

        QString logFilePath = m_logDirectory + "/logs.log";
        if (!HasSufficientDiskSpace(logFilePath)) {
            qWarning() << translator->getErrorMessage(LOG_INSUFFICIENT_SPACE_ERROR);            
            return nullptr;
        }

        try {
            auto rotating_logger = spdlog::rotating_logger_mt(
                serviceName,
                logFilePath.toStdString(),
                m_fileSizeBytes,
                m_fileCount);

            spdlog::level::level_enum spdLogLevel = ToSpdlogLevel(level);
            rotating_logger->set_level(spdLogLevel);
            rotating_logger->flush_on(spdLogLevel);

            QWriteLocker writeLocker(&lock);
            m_loggerMap[qName] = rotating_logger;
            return rotating_logger;
        } catch (const spdlog::spdlog_ex& ex) {
            qWarning() << "Logger creation failed for" << qName << ":" << ex.what();
            return nullptr;
        }
    }

    bool LoggerProvider::HasSufficientDiskSpace(const QString& path)
    {
        QString dirPath = QFileInfo(path).absolutePath();
        QStorageInfo storage(dirPath);
        return storage.bytesAvailable() > m_fileCount * m_fileSizeBytes;
    }


    void LoggerProvider::Shutdown()
    {
        m_loggerMap.clear();
        spdlog::shutdown();
    }
}
