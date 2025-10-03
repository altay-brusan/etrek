#include "DatabaseConnectionSetting.h"
#include "DatabaseConnectionSetting.h"


namespace Etrek::Core::Data::Model
{
DatabaseConnectionSetting::DatabaseConnectionSetting(QObject* parent)
: QObject{ nullptr }, // enforce to not use parent. 
  m_port{ 0 }, 
  m_isPasswordEncrypted{ false }, 
  m_hostName{}, 
  m_databaseName{}, 
  m_userName{}, 
  m_password{}
{
}

void DatabaseConnectionSetting::setHostName(const QString& hostname) { m_hostName = hostname; }
void DatabaseConnectionSetting::setDatabaseName(const QString& dbname) { m_databaseName = dbname; }
void DatabaseConnectionSetting::setEtrektUserName(const QString& username) { m_userName = username; }
void DatabaseConnectionSetting::setPassword(const QString& password) { m_password = password; }
void DatabaseConnectionSetting::setPort(int port) { m_port = port; }
void DatabaseConnectionSetting::setIsPasswordEncrypted(bool encrypted) { m_isPasswordEncrypted = encrypted; }

QString DatabaseConnectionSetting::getHostName() const { return m_hostName; }
QString DatabaseConnectionSetting::getDatabaseName() const { return m_databaseName; }
QString DatabaseConnectionSetting::getEtrekUserName() const { return m_userName; }
QString DatabaseConnectionSetting::getPassword() const { return m_password; }
int DatabaseConnectionSetting::getPort() const { return m_port; }
bool DatabaseConnectionSetting::getIsPasswordEncrypted() const { return m_isPasswordEncrypted; }
}
