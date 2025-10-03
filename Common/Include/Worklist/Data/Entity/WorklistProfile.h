#ifndef WORKLISTPROFILE_H
#define WORKLISTPROFILE_H
#include <QString>
#include <QList>
#include "ProfileTagAssociation.h"
#include "WorklistPresentationContext.h"

namespace Etrek::Worklist::Data::Entity {

class WorklistProfile{
public:
    int Id = -1;
    QString Name;  // e.g., 'DefaultMWLQuery', 'UltrasoundTags'
    WorklistPresentationContext Context; // Presentation context for the profile
    QList<ProfileTagAssociation> Tags;   // Associated tags for the profile


    WorklistProfile() = default;
};
}
#endif // WORKLISTPROFILE_H
