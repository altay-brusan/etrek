#ifndef SETTINGPROVIDER_H
#define SETTINGPROVIDER_H

#include "DatabaseConnectionSetting.h"
#include "FileLoggerSetting.h"
#include "RisConnectionSetting.h"

#include <QObject>
#include <QVector.h>
#include <QSharedPointer>

namespace Etrek::Core::Setting {

    namespace mdl = Etrek::Core::Data::Model;

    /**
    *  @file SettingProvider.h
    *  @brief Declaration of the SettingProvider class for managing application settings.
    *
    *  The SettingProvider class provides access to various application settings, including:
    *  - Database connection settings
    *  - File logger configuration
    *  - RIS connection settings
    *
    *  It is responsible for loading settings from a JSON file or QIODevice and provides
    *  methods to retrieve the current settings for database connections, file logging,
    *  and RIS connections.
    */
    class SettingProvider : public QObject
    {
        Q_OBJECT
    public:
        /** 
        *  @brief Constructs a SettingProvider instance.
        *  @param parent The parent QObject, defaults to nullptr.
        */
        explicit SettingProvider(QObject *parent = nullptr);
        
        /**
        *  @brief Loads settings from a QIODevice.
        *  @param device The QIODevice to read settings from.
        *  @return True if settings were loaded successfully, false otherwise.
        */
        bool loadSettingsFile(QIODevice& device);
        
        /**
        *   @brief Loads settings from a JSON file. 
        *   @param jsonFilePath The path to the JSON file. 
        *   @return True if settings were loaded successfully, false otherwise. 
        */        
        bool loadSettingsFile(const QString& jsonFilePath);

        /**
        *   @brief Retrieves the current database connection settings.
        *   @return A shared pointer to the current DatabaseConnectionSetting.
        */
        std::shared_ptr<mdl::DatabaseConnectionSetting> getDatabaseConnectionSettings() const;

        /**
        *  @brief Retrieves the current file logger settings.
        *  @return A shared pointer to the current FileLoggerSetting.
        */
        std::shared_ptr<mdl::FileLoggerSetting> getFileLoggerSettings() const;

        /**
        *  @brief Retrieves the current RIS connection settings.
        *  @return A QVector of shared pointers to the current RisConnectionSetting.
        */
        QVector<QSharedPointer<mdl::RisConnectionSetting>> getRisSettings() const;

    private:
        std::shared_ptr<mdl::DatabaseConnectionSetting> m_databaseSetting; ///< Database connection settings
        std::shared_ptr<mdl::FileLoggerSetting> m_fileLoggerSetting; ///< File logger settings
        QVector<QSharedPointer<mdl::RisConnectionSetting>> m_risSetting; ///< RIS connection settings
    };
}

#endif // SETTINGPROVIDER_H
