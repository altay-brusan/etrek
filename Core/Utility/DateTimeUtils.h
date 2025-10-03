#ifndef DATETIMEUTILS_H
#define DATETIMEUTILS_H
#include <QDateTime>
#include <QString>
#include <QTime>

/**
 * @namespace Etrek::Utility
 * @brief Provides utility functions for converting and handling QDateTime objects in UTC and local time formats.
 */
namespace Etrek::Utility {

    /**
     * @brief Converts a QDateTime to a UTC string in "yyyy-MM-dd HH:mm:ss" format.
     * @param dateTime The QDateTime to convert.
     * @return UTC string representation of the date and time.
     */
    inline QString toUtcString(const QDateTime& dateTime) {
        return dateTime.toUTC().toString("yyyy-MM-dd HH:mm:ss");
    }

    /**
     * @brief Converts a QDateTime to a local time string in "yyyy-MM-dd HH:mm:ss" format.
     * @param dateTime The QDateTime to convert.
     * @return Local string representation of the date and time.
     */
    inline QString toLocalString(const QDateTime& dateTime) {
        return dateTime.toLocalTime().toString("yyyy-MM-dd HH:mm:ss");
    }

    /**
     * @brief Converts a UTC string in "yyyy-MM-dd HH:mm:ss" format to a QDateTime in UTC.
     * @param dateTimeStr The UTC string to convert.
     * @return QDateTime object in UTC.
     */
    inline QDateTime fromUtcString(const QString& dateTimeStr) {
        return QDateTime::fromString(dateTimeStr, "yyyy-MM-dd HH:mm:ss").toUTC();
    }

    /**
     * @brief Converts a local time string in "yyyy-MM-dd HH:mm:ss" format to a QDateTime in local time.
     * @param dateTimeStr The local time string to convert.
     * @return QDateTime object in local time.
     */
    inline QDateTime fromLocalString(const QString& dateTimeStr) {
        return QDateTime::fromString(dateTimeStr, "yyyy-MM-dd HH:mm:ss").toLocalTime();
    }

    /**
     * @brief Gets the current UTC time as a QDateTime.
     * @return Current UTC QDateTime.
     */
    inline QDateTime getCurrentUtcTime() {
        return QDateTime::currentDateTimeUtc();
    }

    /**
     * @brief Gets the current local time as a QDateTime.
     * @return Current local QDateTime.
     */
    inline QDateTime getCurrentLocalTime() {
        return QDateTime::currentDateTime();
    }

    /**
     * @brief Checks if a QDateTime is in UTC.
     * @param dateTime The QDateTime to check.
     * @return True if the QDateTime is in UTC, false otherwise.
     */
    inline bool isUtc(const QDateTime& dateTime) {
        return dateTime.timeSpec() == Qt::UTC;
    }

    /**
     * @brief Checks if a QDateTime is in local time.
     * @param dateTime The QDateTime to check.
     * @return True if the QDateTime is in local time, false otherwise.
     */
    inline bool isLocal(const QDateTime& dateTime) {
        return dateTime.timeSpec() == Qt::LocalTime;
    }

}
#endif // DATETIMEUTILITY_H
