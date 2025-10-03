#include "RisConnectionSetting.h"

namespace Etrek::Core::Data::Model
{
    RisConnectionSetting::RisConnectionSetting(QObject* parent)
        : QObject{ parent },
          m_state(0),
          m_worklistDestination(0),
          m_echoFailProcess(0),
		  m_nameDirection(0), 
          m_protocolCode(0), 
          m_isActive(false)
    {
    }

    // Setter method definitions
    void RisConnectionSetting::setCallingAETitle(const QString& callingAETitle) {
        m_callingAETitle = callingAETitle;
    }
    void RisConnectionSetting::setCalledAETitle(const QString& calledAETitle) {
        m_calledAETitle = calledAETitle;
    }
    void RisConnectionSetting::setHostIP(const QString& hostIP) {
        m_host = hostIP;
    }
    void RisConnectionSetting::setPort(const int port) {
        m_port = port;
    }
    void RisConnectionSetting::setState(const int state) {
        m_state = state;
    }
    void RisConnectionSetting::setModality(const QString& modality) {
        m_modality = modality;
    }
    void RisConnectionSetting::setWorklistDestination(const int worklistDestination) {
        m_worklistDestination = worklistDestination;
    }
    void RisConnectionSetting::setEchoFailProcess(const int echoFailProcess) {
        m_echoFailProcess = echoFailProcess;
    }
    void RisConnectionSetting::setNameSeparator(const QString& nameSeparator) {
        m_nameSeparator = nameSeparator;
    }
    void RisConnectionSetting::setNameDirection(const int nameDirection) {
        m_nameDirection = nameDirection;
    }
    void RisConnectionSetting::setProtocolCode(const int protocolCode) {
        m_protocolCode = protocolCode;
    }


    void RisConnectionSetting::setConnectionName(const QString& connectionName) {
        m_connectionName = connectionName;
    }

    void RisConnectionSetting::setActiveFlag(const bool isActive)
    {
        m_isActive = isActive;
    }



    // Getter method definitions
    QString RisConnectionSetting::getCallingAETitle() const {
        return m_callingAETitle;
    }
    QString RisConnectionSetting::getCalledAETitle() const {
        return m_calledAETitle;
    }
    QString RisConnectionSetting::getHostIP() const {
        return m_host;
    }
    int RisConnectionSetting::getPort() const {
        return m_port;
    }
    int RisConnectionSetting::getState() const {
        return m_state;
    }
    QString RisConnectionSetting::getModality() const {
        return m_modality;
    }
    int RisConnectionSetting::getWorklistDestination() const {
        return m_worklistDestination;
    }
    int RisConnectionSetting::getEchoFailProcess() const {
        return m_echoFailProcess;
    }
    QString RisConnectionSetting::getNameSeparator() const {
        return m_nameSeparator;
    }
    int RisConnectionSetting::getNameDirection() const {
        return m_nameDirection;
    }
    int RisConnectionSetting::getProtocolCode() const {
        return m_protocolCode;
    }

    QString RisConnectionSetting::getConnectionName() const {
        return m_connectionName;
    }

    bool RisConnectionSetting::getActiveFlag() const
    {
        return m_isActive;
    }

}



