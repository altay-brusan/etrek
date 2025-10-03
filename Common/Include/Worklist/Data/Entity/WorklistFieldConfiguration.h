#include "WorklistEnum.h"

namespace Etrek::Worklist::Data::Entity {

    class WorklistFieldConfiguration {
    public:
        int Id = -1;
        WorklistFieldName FieldName;
        bool IsEnabled = false;

        WorklistFieldConfiguration() = default;
    };

}
