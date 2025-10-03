#ifndef PROFILETAGASSOCIATION_H
#define PROFILETAGASSOCIATION_H
#include "DicomTag.h"


namespace Etrek::Worklist::Data::Entity {

class ProfileTagAssociation{
public:
    bool IsIdentifier;
    bool IsMandatoryIdentifier;
    DicomTag Tag;
    int ProfileId = -1;
};
}
#endif // PROFILETAGASSOCIATION_H
