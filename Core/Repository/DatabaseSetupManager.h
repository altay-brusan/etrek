#ifndef DATABASEINITIALIZER_H
#define DATABASEINITIALIZER_H

#include <memory>
#include <QSqlDatabase>
#include <QFile>
#include "Result.h"
#include "TranslationProvider.h"
#include "DatabaseConnectionSetting.h"
#include "AppLogger.h"

using namespace Etrek::Core::Log;
using namespace Etrek::Core::Data;

namespace Etrek::Core::Repository {

    using Etrek::Specification::Result;

    /**
     * @class DatabaseSetupManager
     * @brief Manages the initialization and setup of the database.
     *
     * This class is responsible for creating the database if it does not exist,
     * running the setup script to initialize the database schema, and providing
     * methods to handle database setup operations.
     */
    class DatabaseSetupManager
    {
    public:
        /**
         * @brief Constructs a DatabaseSetupManager with the given connection settings.
         * @param connectionSetting Shared pointer to the database connection settings.
         */
        explicit DatabaseSetupManager(std::shared_ptr<Model::DatabaseConnectionSetting> connectionSetting);

        /**
         * @brief Initializes the database using the provided setup script file.
         * @param setupScript Unique pointer to a QFile containing the SQL setup script.
         *        If nullptr, a default resource script will be used.
         * @return Result containing a success or error message.
         */
        Result<QString> initializeDatabase(std::unique_ptr<QFile> setupScript = nullptr);

        /**
         * @brief Initializes the database using the setup script at the given file path.
         * @param setupScriptPath Path to the SQL setup script file.
         * @return Result containing a success or error message.
         */
        Result<QString> initializeDatabase(const QString& setupScriptPath);

    private:
        /**
         * @brief Creates the database if it does not already exist.
         * @return True if the database exists or was created successfully, false otherwise.
         */
        bool createDatabaseIfMissing();

        /**
         * @brief Executes the setup script to initialize the database schema.
         * @param db Reference to an open QSqlDatabase connection.
         * @param setupScript Unique pointer to a QFile containing the SQL setup script.
         *        If nullptr, a default resource script will be used.
         * @return Result containing a success or error message.
         */
        Result<QString> runSetupScript(QSqlDatabase& db, std::unique_ptr<QFile> setupScript = nullptr);

        /**
         * @brief Creates a new database connection with the specified database and connection name.
         * @param dbName The name of the database to connect to.
         * @param connectionName The name for the database connection.
         * @return The QSqlDatabase object for the connection.
         */
        QSqlDatabase createConnection(const QString& dbName, const QString& connectionName);

        /**
         * @brief Pointer to the translation provider for localized messages.
         */
        TranslationProvider* translator;

        /**
         * @brief Shared pointer to the application logger.
         */
        std::shared_ptr<AppLogger> logger;

        /**
         * @brief Shared pointer to the database connection settings.
         */
        std::shared_ptr<Model::DatabaseConnectionSetting> m_connectionSetting;
    };

} // namespace Etrek::Core::Repository

#endif // DATABASEINITIALIZER_H
