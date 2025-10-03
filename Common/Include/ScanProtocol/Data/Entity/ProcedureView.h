#ifndef PROCEDURE_VIEW_H
#define PROCEDURE_VIEW_H

/**
 * @file ProcedureView.h
 * @brief Join entity mapping `procedures` to `views`.
 *
 * @details
 * Mirrors `procedure_views` with PK (procedure_id, view_id).
 */

#include <QMetaType>

namespace Etrek::ScanProtocol::Data::Entity {

    class ProcedureView {
    public:
        int procedure_id = -1; ///< FK to `procedures.id` (part of PK)
        int view_id = -1;      ///< FK to `views.id`       (part of PK)

        ProcedureView() = default;

        /// @brief Identity per composite PK.
        bool operator==(const ProcedureView& o) const noexcept {
            return procedure_id == o.procedure_id && view_id == o.view_id;
        }
        bool operator!=(const ProcedureView& o) const noexcept { return !(*this == o); }
    };

} // namespace Etrek::ScanProtocol::Data::Entity

Q_DECLARE_METATYPE(Etrek::ScanProtocol::Data::Entity::ProcedureView)

#endif // PROCEDURE_VIEW_H
