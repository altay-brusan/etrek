#ifndef IDATABASECONNECTIONSETTING_H
#define IDATABASECONNECTIONSETTING_H

#include <QObject>
#include <QString>

namespace Etrek::Core::Data::Model
{
    class IDatabaseConnectionSetting
    {

    public:        
        virtual QString getHostName() const = 0;
        virtual QString getDatabaseName() const = 0;
        virtual QString getEtrekUserName() const = 0;
        virtual QString getPassword() const = 0;
        virtual int getPort() const = 0;
        virtual bool getIsPasswordEncrypted() const = 0;

    };
}

#endif // IDATABASECONNECTIONSETTING_H
