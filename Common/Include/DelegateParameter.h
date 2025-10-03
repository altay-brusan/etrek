#ifndef DELEGATEPARAMETERS_H
#define DELEGATEPARAMETERS_H

#include <memory>
#include <QMap>
#include <QString>
#include "DatabaseConnectionSetting.h"
#include "IDelegate.h"

using namespace Etrek::Core::Data::Model;
struct DelegateParameter
{
    std::shared_ptr<DatabaseConnectionSetting> dbConnection;
    QMap<QString, QWeakPointer<IDelegate>> delegates;
};

#endif // DELEGATEPARAMETERS_H

