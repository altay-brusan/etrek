#ifndef DATABASECONNECTIONSETTING_H
#define DATABASECONNECTIONSETTING_H

#include <QObject>


namespace Etrek::Core::Data::Model
{
    /**
     * @class DatabaseConnectionSetting
     * @brief Stores configuration parameters for a database connection.
     *
     * Encapsulates details such as host name, database name, user credentials,
     * port number, and a flag indicating whether the password is encrypted.
     * Designed to provide convenient getters and setters for these parameters.
     *
     * Inherits from QObject to support signal-slot connections and enable
     * live change tracking from JSON settings files, which may be useful for
     * future dynamic updates or notifications.
     *
     * @note Do not assign a parent to the QObject. Always manage instances
     *       using standard smart pointers to ensure proper memory handling.
     *
     * @see SettingProvider.h for an example of usage.
     */


    class DatabaseConnectionSetting : public QObject
    {
        Q_OBJECT

    public:
        explicit DatabaseConnectionSetting(QObject* parent = nullptr);
        void setHostName(const QString& hostname);
        void setDatabaseName(const QString& dbname);
        void setEtrektUserName(const QString& username);
        void setPassword(const QString& password);
        void setPort(int port);
        void setIsPasswordEncrypted(bool encrypted);

        QString getHostName() const;
        QString getDatabaseName() const;
        QString getEtrekUserName() const;
        QString getPassword() const;
        int getPort() const;
        bool getIsPasswordEncrypted() const;

    private:
        QString m_hostName;
        QString m_databaseName;
        QString m_userName;
        QString m_password;
        int m_port;
        bool m_isPasswordEncrypted = false;
    };
}

#endif // DATABASECONNECTIONSETTING_H
