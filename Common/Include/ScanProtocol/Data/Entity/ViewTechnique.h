#ifndef VIEWTECHNIQUE_H
#define VIEWTECHNIQUE_H

/**
 * @file ViewTechnique.h
 * @brief Declares the ViewTechnique join entity between View and TechniqueParameter.
 *
 * Represents the ordered association of technique parameters attached to a view,
 * including the role of each technique within that view.
 */

#include <QString>
#include <QMetaType>
#include <QDateTime>

#include "ScanProtocolUtil.h"

namespace Etrek::ScanProtocol::Data::Entity {

	namespace sp = Etrek::ScanProtocol;

    /**
     * @class ViewTechnique
     * @brief Join/association entity linking a View to a TechniqueParameter.
     *
     * Models a many-to-many relationship with an explicit order (seq) and a role
     * for each technique parameter within a given view. Equality compares the
     * composite key (view_id, seq).
     */
    class ViewTechnique {
    public:
        /** @brief Foreign key to `views.id` (part of composite key). */
        int view_id = -1;

        /** @brief Foreign key to `technique_parameters.id` (unique per view). */
        int technique_parameter_id = -1;

        /**
         * @brief Sequence/order within the view (1..n, part of composite key).
         *
         * Determines the display or evaluation order of technique parameters for
         * the associated view.
         */
        quint8 seq = 1;

        /**
         * @brief The role this technique parameter plays within the view.
         * @see Etrek::ScanProtocol::TechniqueParameterRole
         */
        sp::TechniqueParameterRole role =
            sp::TechniqueParameterRole::Primary;

        /**
         * @brief Activation flag controlling whether this linkage is considered active.
         */
        bool IsActive = true;

        /** @brief Default constructor. */
        ViewTechnique() = default;

        /**
         * @brief Equality operator by composite key (view_id, seq).
         * @param other Another ViewTechnique to compare with.
         * @return true if both @ref view_id and @ref seq match; otherwise false.
         */
        bool operator==(const ViewTechnique& other) const noexcept {
            return view_id == other.view_id && seq == other.seq;
        }
    };

} // namespace Etrek::ScanProtocol::Data::Entity

/**
 * @brief Enables ViewTechnique for use with Qt's meta-object system (e.g., QVariant).
 */
Q_DECLARE_METATYPE(Etrek::ScanProtocol::Data::Entity::ViewTechnique)

#endif // VIEWTECHNIQUE_H
