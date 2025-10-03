#ifndef WORKLISTATTRIBUTE_H
#define WORKLISTATTRIBUTE_H
#include <QString>
#include "DicomTag.h"

namespace Etrek::Worklist::Data::Entity {

   class WorklistAttribute{
    public:
       int id = -1;
       int EntryId = -1;
       DicomTag Tag;
       QString TagValue;
    };
}

#endif // WORKLISTATTRIBUTE_H
