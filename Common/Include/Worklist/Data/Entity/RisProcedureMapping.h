#ifndef RISPROCEDUREMAPPING_H
#define RISPROCEDUREMAPPING_H

#include <QString>
#include <QDateTime>

namespace Etrek::Worklist::Data::Entity {

/**
 * @brief Represents a mapping from external RIS procedure code to internal view.
 *
 * This entity maps procedure codes received from RIS systems (via MWL C-FIND)
 * to internal view IDs used in the application. Each RIS connection can have
 * its own set of mappings since different RIS systems may use different
 * coding schemes.
 */
class RisProcedureMapping {
public:
    int Id = -1;
    QString ConnectionName;         // RIS connection identifier from settings
    QString ExternalCode;           // Procedure code from RIS (e.g., "SRT2133", "CHEST-PA")
    QString ExternalCodingScheme;   // Optional coding scheme (e.g., "SRT", "99LOCAL")
    QString ExternalCodeMeaning;    // Human-readable description from RIS
    int InternalViewId = -1;        // FK to views table
    bool IsActive = true;
    QString Notes;
    QDateTime CreatedAt;
    QDateTime UpdatedAt;

    RisProcedureMapping() = default;

    /**
     * @brief Checks if this mapping is valid (has been persisted to database)
     * @return true if the mapping has a valid database ID (Id >= 0)
     */
    bool isValid() const { return Id >= 0; }
};

} // namespace Etrek::Worklist::Data::Entity

#endif // RISPROCEDUREMAPPING_H
