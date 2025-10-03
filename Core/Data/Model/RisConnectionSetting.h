#ifndef RISCONNECTIONENTITYMODEL_H
#define RISCONNECTIONENTITYMODEL_H

#include <QVector>
#include <QString>
#include <QObject>


namespace Etrek::Core::Data::Model
{
    /**
     * @class RisConnectionSetting
     * @brief Represents the connection settings for a RIS (Radiology Information System).
     *
     * This class encapsulates various parameters required to establish a connection
     * to a RIS, including AETitles, host information, port, modality, and other
     * relevant settings. It also provides methods to set and retrieve these parameters.
     *
     * @brief Constructs the RisConnectionSetting and initializes connection parameters.
     */

    class RisConnectionSetting :public QObject
    {
        Q_OBJECT

    public:
        explicit RisConnectionSetting(QObject* parent = nullptr);

        // Setters
        void setCallingAETitle(const QString& callingAETitle);
        void setCalledAETitle(const QString& calledAETitle);
        void setHostIP(const QString& hostIP);
        void setPort(const int port);
        void setState(const int state);
        void setModality(const QString& modality);
        void setWorklistDestination(const int worklistDestination);
        void setEchoFailProcess(const int echoFailProcess);
        void setNameSeparator(const QString& nameSeparator);
        void setNameDirection(const int nameDirection);
        void setProtocolCode(const int protocolCode);
        void setConnectionName(const QString& connectionName);
        void setActiveFlag(const bool isConnected);

        // Getters
        QString getCallingAETitle() const;
        QString getCalledAETitle() const;
        QString getHostIP() const;
        int getPort() const;
        int getState() const;
        QString getModality() const;
        int getWorklistDestination() const;
        int getEchoFailProcess() const;
        QString getNameSeparator() const;
        int getNameDirection() const;
        int getProtocolCode() const;
        QString getConnectionName() const;
        bool getActiveFlag() const;

    private:
        // Connection parameters
        QString m_callingAETitle;
        QString m_calledAETitle;
        QString m_host;
        int m_port;

        int m_state;        //Optional status flag (e.g., active/inactive); use as needed
        QString m_modality;
        int m_worklistDestination;
        int m_echoFailProcess; //How to handle echo failures (e.g., retry, skip)
        QString m_nameSeparator;//Character to split person names ("^" is default in DICOM)
        int m_nameDirection;    //Display order of names (0 = First^Last, 1 = Last^First)
        int m_protocolCode;     //Internal protocol identifier (e.g., maps to a clinical workflow definition)
        QString m_connectionName;
        bool m_isActive;
    };
}

#endif // RISCONNECTIONENTITYMODEL_H
