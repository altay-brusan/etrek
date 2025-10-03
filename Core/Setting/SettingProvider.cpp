#include <QFile>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include "SettingProvider.h"
#include "CryptoManager.h"
#include "DatabaseConnectionSetting.h"
#include "RisConnectionSetting.h"


using namespace Etrek::Core::Data;
using namespace Etrek::Core::Security;

namespace Etrek::Core::Setting {
    SettingProvider::SettingProvider(QObject *parent)
        : QObject{parent},
        m_databaseSetting(std::make_shared<Model::DatabaseConnectionSetting>()),
        m_fileLoggerSetting(std::make_shared<Model::FileLoggerSetting>()),
        m_risSetting(QVector<QSharedPointer<Model::RisConnectionSetting>>())
        {}

    bool SettingProvider::loadSettingsFile(QIODevice& device)
    {
        CryptoManager securityServiceProvider;

        if (!device.isOpen() && !device.open(QIODevice::ReadOnly)) {
            qWarning("Couldn't open device.");
            return false;
        }

        QByteArray data = device.readAll();
        QJsonDocument doc(QJsonDocument::fromJson(data));
        if (!doc.isObject())
            return false;

        QJsonObject rootObj = doc.object();

        if (rootObj.contains("DatabaseConnection") && rootObj["DatabaseConnection"].isObject()) {
            QJsonObject dbObj = rootObj["DatabaseConnection"].toObject();

            m_databaseSetting->setDatabaseName(dbObj["DatabaseName"].toString());
            m_databaseSetting->setHostName(dbObj["HostName"].toString());
            m_databaseSetting->setIsPasswordEncrypted(dbObj["IsPasswordEncrypted"].toBool());
            m_databaseSetting->setPassword(dbObj["Password"].toString());
            m_databaseSetting->setEtrektUserName(dbObj["UserName"].toString());



            if (m_databaseSetting->getIsPasswordEncrypted()) {
                QString decryptedPassword = securityServiceProvider.decryptPassword(m_databaseSetting->getPassword());
                m_databaseSetting->setPassword(decryptedPassword);
            }
        }

        if (rootObj.contains("FileLogger") && rootObj["FileLogger"].isObject()) {
            QJsonObject dbObj = rootObj["FileLogger"].toObject();

            m_fileLoggerSetting->setLogDirectory(dbObj["LogDirectory"].toString());
            m_fileLoggerSetting->setFileSize(dbObj["FileSize"].toInt(1));
            m_fileLoggerSetting->setFileCount(dbObj["FileCount"].toInt(5));

        }

        // Load multiple ModalityWorklistConnections as a vector
        if (rootObj.contains("ModalityWorklistConnection") && rootObj["ModalityWorklistConnection"].isArray()) {
            QJsonArray modalityArray = rootObj["ModalityWorklistConnection"].toArray();

            for (const QJsonValue& value : modalityArray) {
                if (value.isObject()) {
                    QJsonObject dbObj = value.toObject();
                    auto connection = QSharedPointer<Model::RisConnectionSetting>::create();
                    connection->setConnectionName(dbObj["ConnectionName"].toString());
                    connection->setCallingAETitle(dbObj["CallingAETitle"].toString());
                    connection->setCalledAETitle(dbObj["CalledAETitle"].toString());
                    connection->setHostIP(dbObj["HostIP"].toString());
                    connection->setPort(dbObj["Port"].toInt());
                    connection->setState(dbObj["State"].toInt());
                    connection->setModality(dbObj["Modality"].toString());
                    connection->setWorklistDestination(dbObj["WorklistDestination"].toInt());
                    connection->setEchoFailProcess(dbObj["EchoFailProcess"].toInt());
                    connection->setNameSeparator(dbObj["NameSeparator"].toString());
                    connection->setNameDirection(dbObj["NameDirection"].toInt());
                    connection->setProtocolCode(dbObj["ProtocolCode"].toInt());
                    connection->setActiveFlag(dbObj["IsActive"].toBool());

                    m_risSetting.append(connection);
                }
            }
        }


        return true;
    }

    bool SettingProvider::loadSettingsFile(const QString& jsonFilePath)
    {
        QFile file(jsonFilePath);
        return loadSettingsFile(file); // Passing by reference
    }

    std::shared_ptr<Model::DatabaseConnectionSetting> SettingProvider::getDatabaseConnectionSettings() const
    {
        return m_databaseSetting;
    }

    std::shared_ptr<Model::FileLoggerSetting> SettingProvider::getFileLoggerSettings() const
    {
        return m_fileLoggerSetting;
    }

    QVector< QSharedPointer<Model::RisConnectionSetting>> SettingProvider::getRisSettings() const
    {
        return m_risSetting;
    }
}
