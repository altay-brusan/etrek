#ifndef VIEWTECHNIQUE_H
#define VIEWTECHNIQUE_H

#include <QString>
#include <QMetaType>
#include <QDateTime>

#include "ScanProtocolUtil.h"

namespace Etrek::ScanProtocol::Data::Entity {

    /**
     * @brief Join table between views and technique_parameters.
     *
     * Suggested MySQL:
     *   PRIMARY KEY (view_id, seq)
     *   UNIQUE (view_id, technique_parameter_id)
     *   FOREIGN KEY (view_id) -> views(id) ON DELETE CASCADE
     *   FOREIGN KEY (technique_parameter_id) -> technique_parameters(id) ON DELETE RESTRICT
     */
    class ViewTechnique {
    public:
        int     view_id = -1;                ///< FK to views.id (part of PK)
        int     technique_parameter_id = -1; ///< FK to technique_parameters.id (unique per view)
        quint8  seq = 1;                     ///< Order within the view (1..n, part of PK)
        Etrek::ScanProtocol::TechniqueParameterRole role =
        Etrek::ScanProtocol::TechniqueParameterRole::Primary;
        bool IsActive = true;

        ViewTechnique() = default;

        bool operator==(const ViewTechnique& other) const noexcept {
            return view_id == other.view_id && seq == other.seq;
        }
    };

} // namespace Etrek::ScanProtocol::Data::Entity

Q_DECLARE_METATYPE(Etrek::ScanProtocol::Data::Entity::ViewTechnique)

#endif // VIEWTECHNIQUE_H
