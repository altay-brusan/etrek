#ifndef DELEGATEPARAMETERS_H
#define DELEGATEPARAMETERS_H

#include <memory>
#include <QMap>
#include <QString>
#include "DatabaseConnectionSetting.h"
#include "IDelegate.h"

struct DelegateParameter
{
    std::shared_ptr<Etrek::Core::Data::Model::DatabaseConnectionSetting> dbConnection;
    QMap<QString, QWeakPointer<IDelegate>> delegates;
};

#endif // DELEGATEPARAMETERS_H

