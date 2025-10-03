#include "DatabaseSetupManager.h"
#include "Result.h"
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include <QFile>
#include <QTextStream>
#include <QDebug>
#include "MessageKey.h"
#include "AppLoggerFactory.h"

namespace Etrek::Core::Repository {

    DatabaseSetupManager::DatabaseSetupManager(std::shared_ptr<Model::DatabaseConnectionSetting> connectionSetting)
        :m_connectionSetting(std::move(connectionSetting))
        {
            translator = &TranslationProvider::Instance();
            AppLoggerFactory factory(LoggerProvider::Instance(), translator);
            logger = factory.CreateLogger("DatabaseInitializer");
        }

    Result<QString> DatabaseSetupManager::initializeDatabase(std::unique_ptr<QFile> setupScript)
    {

        if (!createDatabaseIfMissing())
        {
            auto errMsg = translator->getErrorMessage(DB_FAILED_TO_CONNECT_OR_CREATE_MSG);
            logger->LogError(errMsg);
            return Result<QString>::Failure(errMsg);
        }
        QSqlDatabase db = createConnection(m_connectionSetting->getDatabaseName(), "etrek_connection");

        if (!db.open()) {
            QString errMsg = QString(translator->getErrorMessage(DB_FAILED_TO_OPEN_AFTER_CREATION_MSG)).arg(db.lastError().text());
            logger->LogError(errMsg);
            return Result<QString>::Failure(errMsg);
        }

        QSqlQuery checkQuery("SHOW TABLES", db);
        if (!checkQuery.next()) {
            QString message = translator->getErrorMessage(SQL_SCRIPT_NO_TABLES_FOUND_MSG);
            logger->LogInfo(message);
            auto scriptResult = runSetupScript(db, std::move(setupScript));
            if (!scriptResult.isSuccess)
            {
                logger->LogError(scriptResult.message);
                return Result<QString>::Failure(scriptResult.message);
            }
        }

        QString message =translator->getInfoMessage(DB_INIT_SUCCESS_MSG);
        logger->LogInfo(message);
        return Result<QString>::Success(message);
    }

    Result<QString> DatabaseSetupManager::initializeDatabase(const QString& setupScriptPath)
    {
        return initializeDatabase(std::make_unique<QFile>(setupScriptPath));
    }

    bool DatabaseSetupManager::createDatabaseIfMissing()
    {
        QSqlDatabase db = createConnection(m_connectionSetting->getDatabaseName(), "check_connection");

        if (db.open()) {
            db.close();
            return true; // DB exists
        }

        QSqlDatabase rootDb = createConnection("mysql", "root_connection");

        if (!rootDb.open()) {
            QString message = QString("%1: %2")
            .arg(translator->getErrorMessage(DB_CANNOT_CONNECT_TO_MYSQL_MSG))
                .arg(rootDb.lastError().text());

            logger->LogError(message);
            qDebug() << message;
            return false;
        }

        QSqlQuery createDbQuery(rootDb);
        if (!createDbQuery.exec(QString("CREATE DATABASE IF NOT EXISTS `%1`")
                                    .arg(m_connectionSetting->getDatabaseName()))) {

            QString message = QString("%1: %2")
                                  .arg(translator->getErrorMessage(DB_CANNOT_CONNECT_TO_MYSQL_MSG))
                                  .arg(rootDb.lastError().text());

            logger->LogError(message);
            qDebug() << message;

            return false;
        }

        rootDb.close();
        return true;
    }

    QSqlDatabase DatabaseSetupManager::createConnection(const QString& dbName, const QString& connectionName)
    {
        QSqlDatabase db = QSqlDatabase::addDatabase("QMYSQL", connectionName);
        db.setHostName(m_connectionSetting->getHostName());
        db.setDatabaseName(dbName);
        db.setUserName(m_connectionSetting->getEtrekUserName());
        db.setPassword(m_connectionSetting->getPassword());
        int port = m_connectionSetting->getPort();
        db.setPort(port != 0 ? port : 3306);
        return db;
    }

    Result<QString> DatabaseSetupManager::runSetupScript(QSqlDatabase& db, std::unique_ptr<QFile> setupScript)
    {
        if (!setupScript) {
            setupScript = std::make_unique<QFile>(":/sql/Script/setup_database.sql");
        }

        if (!setupScript->open(QIODevice::ReadOnly | QIODevice::Text)) {
            QString message = QString(translator->getErrorMessage(SQL_SCRIPT_FAILED_TO_OPEN_MSG)).arg(setupScript->fileName());
            qDebug() << message;
            logger->LogError(message);
            return Result<QString>::Failure(message);
        }

        QTextStream stream(setupScript.get());
        QString sql = stream.readAll();
        const QStringList& commands = sql.split(';', Qt::SkipEmptyParts);

        for (const QString& command : commands) {
            QString trimmed = command.trimmed();
            if (!trimmed.isEmpty()) {
                QSqlQuery q(db);
                if (!q.exec(trimmed)) {
                    QString message = QString(translator->getErrorMessage(SQL_SCRIPT_EXECUTION_FAILED_ERROR_MSG)).arg(trimmed, q.lastError().text());
                    qDebug() << message;
                    logger->LogError(message);
                    return Result<QString>::Failure(message);
                }
            }
        }

        QString message =translator->getInfoMessage(SQL_SCRIPT_EXECUTION_SUCCEED_MSG);
        logger->LogInfo(message);
        return Result<QString>::Success(message);
    }
}
