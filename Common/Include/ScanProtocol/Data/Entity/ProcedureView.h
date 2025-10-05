#ifndef PROCEDURE_VIEW_H
#define PROCEDURE_VIEW_H

/**
 * @file ProcedureView.h
 * @brief Declares the ProcedureView join entity between Procedure and View.
 *
 * @details
 * Represents a many-to-many association using a composite primary key made of
 * `procedure_id` and `view_id`. Designed as a lightweight value type suitable
 * for use with Qt containers and meta-object features.
 */

#include <QMetaType>


namespace Etrek::ScanProtocol::Data::Entity {

    /**
     * @class ProcedureView
     * @brief Join/association entity linking a Procedure to a View.
     *
     * Equality and inequality are defined over the composite key
     * (@ref procedure_id, @ref view_id). No additional payload is stored here.
     */
    class ProcedureView {
    public:
        /**
         * @brief Foreign key to Procedure (part of composite key).
         *
         * Defaults to -1 to indicate an unsaved/transient association.
         */
        int procedure_id = -1;

        /**
         * @brief Foreign key to View (part of composite key).
         *
         * Defaults to -1 to indicate an unsaved/transient association.
         */
        int view_id = -1;

        /**
         * @brief Default constructor.
         */
        ProcedureView() = default;

        /**
         * @brief Equality by composite key (procedure_id, view_id).
         */
        bool operator==(const ProcedureView& o) const noexcept {
            return procedure_id == o.procedure_id && view_id == o.view_id;
        }

        /**
         * @brief Inequality is the negation of @ref operator==.
         */
        bool operator!=(const ProcedureView& o) const noexcept { return !(*this == o); }
    };

} // namespace Etrek::ScanProtocol::Data::Entity

/**
 * @brief Enables ProcedureView for use with Qt's meta-object system (e.g., QVariant).
 */
Q_DECLARE_METATYPE(Etrek::ScanProtocol::Data::Entity::ProcedureView)

#endif // PROCEDURE_VIEW_H
