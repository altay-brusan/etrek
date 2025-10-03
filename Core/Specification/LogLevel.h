#ifndef LOGLEVELMAPPER_H
#define LOGLEVELMAPPER_H

#include <QString>
#include <stdexcept>
#include "spdlog/spdlog.h"

/**
 * @enum LogLevel
 * @brief Custom log levels for application logging.
 *
 * This enum defines the available log levels for the application,
 * which can be mapped to spdlog levels or string representations.
 */
enum class LogLevel {
    Debug,      /**< Debug-level messages */
    Info,       /**< Informational messages */
    Warning,    /**< Warning conditions */
    Error,      /**< Error conditions */
    Exception   /**< Critical/exceptional conditions */
};

/**
 * @brief Maps a custom LogLevel to the corresponding spdlog::level::level_enum.
 * @param level The custom LogLevel value.
 * @return The equivalent spdlog::level::level_enum value.
 */
inline spdlog::level::level_enum ToSpdlogLevel(LogLevel level) {
    switch (level) {
    case LogLevel::Debug:     return spdlog::level::debug;
    case LogLevel::Info:      return spdlog::level::info;
    case LogLevel::Warning:   return spdlog::level::warn;
    case LogLevel::Error:     return spdlog::level::err;
    case LogLevel::Exception: return spdlog::level::critical;
    default:                  return spdlog::level::info;
    }
}

/**
 * @brief Maps a spdlog::level::level_enum to the corresponding custom LogLevel.
 * @param level The spdlog::level::level_enum value.
 * @return The equivalent custom LogLevel value.
 */
inline LogLevel FromSpdlogLevel(spdlog::level::level_enum level) {
    switch (level) {
    case spdlog::level::debug:    return LogLevel::Debug;
    case spdlog::level::info:     return LogLevel::Info;
    case spdlog::level::warn:     return LogLevel::Warning;
    case spdlog::level::err:      return LogLevel::Error;
    case spdlog::level::critical: return LogLevel::Exception;
    default:                      return LogLevel::Info;
    }
}

/**
 * @brief Converts a LogLevel value to its string representation.
 * @param level The LogLevel value.
 * @return The string representation of the log level.
 */
inline QString LogLevelToString(LogLevel level) {
    switch (level) {
    case LogLevel::Debug:     return "Debug";
    case LogLevel::Info:      return "Info";
    case LogLevel::Warning:   return "Warning";
    case LogLevel::Error:     return "Error";
    case LogLevel::Exception: return "Exception";
    default:                  return "Info";
    }
}

/**
 * @brief Converts a string to the corresponding LogLevel value.
 * @param str The string representation of the log level.
 * @return The corresponding LogLevel value.
 * @throws std::invalid_argument if the string does not match any LogLevel.
 */
inline LogLevel QStringToLogLevel(const QString& str) {
    if (str.compare("Debug", Qt::CaseInsensitive) == 0)      return LogLevel::Debug;
    if (str.compare("Info", Qt::CaseInsensitive) == 0)       return LogLevel::Info;
    if (str.compare("Warning", Qt::CaseInsensitive) == 0)    return LogLevel::Warning;
    if (str.compare("Error", Qt::CaseInsensitive) == 0)      return LogLevel::Error;
    if (str.compare("Exception", Qt::CaseInsensitive) == 0)  return LogLevel::Exception;
    throw std::invalid_argument("Unknown LogLevel string: " + str.toStdString());
}

#endif // LOGLEVELMAPPER_H
