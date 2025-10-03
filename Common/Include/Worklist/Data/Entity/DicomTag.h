#ifndef DICOMTAG_H
#define DICOMTAG_H
#include <QString>

namespace Etrek::Worklist::Data::Entity {
    class DicomTag {
    public:
        int Id = -1;
        QString Name;
        QString DisplayName;
        uint16_t GroupHex;     // Store the group as uint16_t
        uint16_t ElementHex;   // Store the element as uint16_t
        uint16_t PgroupHex;    // Store the parent group as uint16_t
        uint16_t PelementHex;  // Store the parent element as uint16_t
        bool IsActive;
        bool IsRetired;

        DicomTag() = default;
    };
}

#endif // DICOMTAG_H
