#ifndef DEVICEPOSITION_H
#define DEVICEPOSITION_H

#include <QStringList>
#include <QString>

namespace Etrek::Device
{
    /**
     * @enum DevicePosition
     * @brief Defines possible positions of a device such as an X-ray tube or a Bucky.
     *
     * This enum helps to standardize device position values across the project
     * and avoid typos.
     *
     * - STAND: Device is fixed in a standing position; patient stands in front.
     *          Common for chest X-ray machines.
     * - TABLE: Device is mounted on or under a table; patient lies on the table.
     * - MOTORIZED: Device is mounted on a motorized actuator for precise positioning.
     */
    enum class DevicePosition
    {
        STAND,      /**< Fixed standing position. */
        TABLE,      /**< Table-mounted position, patient lies down. */
        MOTORIZED   /**< Motorized actuator-controlled position. */
    };

    /**
     * @brief Parses a string to determine the corresponding DevicePosition.
     *
     * This function maps a textual description (case-insensitive) to the enum value.
     *
     * @param args Input string describing the device position.
     * @return DevicePosition corresponding to the string. Defaults to STAND if unrecognized.
     */
    inline DevicePosition ParseDevicePosition(const QString& args)
    {
        if (args.contains("stand", Qt::CaseInsensitive)) return DevicePosition::STAND;
        if (args.contains("table", Qt::CaseInsensitive)) return DevicePosition::TABLE;
        if (args.contains("motorized", Qt::CaseInsensitive)) return DevicePosition::MOTORIZED;
        return DevicePosition::STAND; // Default fallback
    }

    /**
     * @brief Converts a DevicePosition enum value into a user-friendly string.
     *
     * This is useful for UI display, logs, or serialization.
     *
     * @param position The DevicePosition value.
     * @return QString representation of the enum.
     */
    inline QString DevicePositionToString(DevicePosition position)
    {
        switch (position)
        {
        case DevicePosition::STAND:      return "Stand";
        case DevicePosition::TABLE:      return "Table";
        case DevicePosition::MOTORIZED:  return "Motorized";
        default:                         return "Unknown";
        }
    }

} // namespace Etrek::Device

#endif // DEVICEPOSITION_H
