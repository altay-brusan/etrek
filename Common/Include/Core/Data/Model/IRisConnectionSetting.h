#ifndef IRISCONNECTIONENTITYMODEL_H
#define IRISCONNECTIONENTITYMODEL_H

#include <QVector>
#include <QString>
#include <QObject>


namespace Etrek::Core::Data::Model
{
	// TODO: check if inheritance from QObject is necessary for this interface
    class IRisConnectionSetting
    {    

    public:

        // Getters
        virtual QString getCallingAETitle() const = 0;
        virtual QString getCalledAETitle() const = 0;
        virtual QString getHostIP() const = 0;
        virtual int getPort() const = 0;
        virtual int getState() const = 0;
        virtual QString getModality() const = 0;
        virtual int getWorklistDestination() const = 0;
        virtual int getEchoFailProcess() const = 0;
        virtual QString getNameSeparator() const = 0;
        virtual int getNameDirection() const = 0;
        virtual int getProtocolCode() const = 0;
        virtual QString getConnectionName() const = 0;
        virtual bool getActiveFlag() const = 0;

    };
}

#endif // IRISCONNECTIONENTITYMODEL_H
