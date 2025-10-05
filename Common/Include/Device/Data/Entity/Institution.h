#ifndef INSTITUTION_H
#define INSTITUTION_H

/**
 * @file Institution.h
 * @brief Entity representing an institution/organization and optional department info.
 */

#include <QString>
#include <QMetaType>

namespace Etrek::Device::Data::Entity {

    /**
     * @class Institution
     * @brief Organization metadata used for device/site association and display.
     *
     * @note Id == -1 indicates a non-persisted instance.
     */
    class Institution {
    public:
        int Id = -1;                 ///< Primary identifier.

        QString Name;                ///< Institution name (e.g., hospital, clinic, lab).
        QString InstitutionType;     ///< Type/category (e.g., Hospital, Clinic, Research).
        QString InstitutionId;       ///< External or organizational identifier/code.
        QString DepartmentName;      ///< Department or unit name within the institution.
        QString Address;             ///< Postal/street address.
        QString ContactInformation;  ///< Contact details (phone/email/notes).

        bool IsActive = true;        ///< Whether the institution entry is active.

        /// Default constructor.
        Institution() = default;

        /**
         * @brief Equality by primary identifier.
         * @param other Another instance to compare.
         * @return true if Id values are equal; otherwise false.
         */
        bool operator==(const Institution& other) const noexcept {
            return Id == other.Id;
        }
    };

} // namespace Etrek::Device::Data::Entity

/// Qt metatype declaration (needed only if used with QVariant/queued connections).
Q_DECLARE_METATYPE(Etrek::Device::Data::Entity::Institution)

#endif // INSTITUTION_H
