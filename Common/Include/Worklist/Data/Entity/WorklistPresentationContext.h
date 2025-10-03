#ifndef WORKLISTPRESENTATIONCONTEXT_H
#define WORKLISTPRESENTATIONCONTEXT_H
#include <QString>
namespace Etrek::Worklist::Data::Entity {

class WorklistPresentationContext{
public:
    int Id = -1;
    int ProfileId = -1;
    QString TransferSyntaxUid;

    WorklistPresentationContext() = default;
};
}
#endif // WORKLISTPRESENTATIONCONTEXT_H
