#include "AppLogger.h"
#include <spdlog/spdlog.h>
#include <QDebug>
#include "MessageKey.h"
#include "LogLevel.h"

namespace Etrek::Core::Log {

	using Etrek::Core::Globalization::TranslationProvider;

    AppLogger::AppLogger(const QString& serviceName, LoggerProvider& provider, glb::TranslationProvider* translator)
		: m_serviceName(serviceName), m_provider(&provider), translator(translator)
    {
    }

    void AppLogger::Log(const QString& message, LogLevel level)
    {
        if (!m_provider) {
			QString errorMsg = translator->getErrorMessage(LOG_APPLOGGER_PROVIDER_NULL_ERROR);
            qWarning() << errorMsg;
			throw std::runtime_error(errorMsg.toStdString());
        }

        auto logger = m_provider->GetFileLogger(m_serviceName.toStdString());
        if (!logger) {
			QString errorMsg = translator->getErrorMessage(LOG_APPLOGGER_INSTANCE_IS_NULL).arg(m_serviceName);
            qWarning() << errorMsg;
			throw std::runtime_error(errorMsg.toStdString());
        }

        std::string msg = message.toStdString();

        try {
            switch (level) {
            case LogLevel::Debug:
                logger->debug(msg);
                break;
            case LogLevel::Info:
                logger->info(msg);
                break;
            case LogLevel::Warning:
                logger->warn(msg);
                break;
            case LogLevel::Error:
                logger->error(msg);
                break;
            case LogLevel::Exception:
                logger->critical(msg);
                break;
            default:
                logger->info(msg);
                break;
            }
        }
        catch (const spdlog::spdlog_ex& ex) { 
            // Catch exceptions from spdlog. 
            // This include file error, disk error, os error.
			QString errorMsg = translator->getErrorMessage(LOG_FAILED_FOR_SERVICE).arg(m_serviceName, QString::fromStdString(ex.what()));
            qWarning() << errorMsg;
			throw std::runtime_error(errorMsg.toStdString());
        }
    }

    void AppLogger::LogException(const QString& message, const std::exception& ex)
    {
        if (!m_provider) {
            QString errorMsg = translator->getErrorMessage(LOG_APPLOGGER_PROVIDER_NULL_ERROR);
            qWarning() << errorMsg;
            throw std::runtime_error(errorMsg.toStdString());
        }

        auto logger = m_provider->GetFileLogger(m_serviceName.toStdString());
        if (!logger) {
            QString errorMsg = translator->getErrorMessage(LOG_APPLOGGER_INSTANCE_IS_NULL).arg(m_serviceName);
            qWarning() << errorMsg;
            throw std::runtime_error(errorMsg.toStdString());
        }

        try {
            logger->critical("{} | Exception: {}", message.toStdString(), ex.what());
        }
        catch (const spdlog::spdlog_ex& spdEx) {
            // Catch exceptions from spdlog. 
            // This include file error, disk error, os error.
			QString message = translator->getErrorMessage(LOG_EXCEPTION_FAILED_FOR_SERVICE)
				.arg(m_serviceName, QString::fromStdString(spdEx.what()));
            qWarning() << message;
			throw std::runtime_error(message.toStdString());
        }
    }

    void AppLogger::LogDebug(const QString& message)
    {
        Log(message, LogLevel::Debug);
    }

    void AppLogger::LogInfo(const QString& message)
    {
        Log(message, LogLevel::Info);
    }

    void AppLogger::LogWarning(const QString& message)
    {
        Log(message, LogLevel::Warning);
    }

    void AppLogger::LogError(const QString& message)
    {
        Log(message, LogLevel::Error);
    }
}
