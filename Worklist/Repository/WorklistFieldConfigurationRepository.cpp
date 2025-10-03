#include <QSqlQuery>
#include <QSqlError>
#include "WorklistFieldConfigurationRepository.h"
#include "DatabaseConnectionSetting.h"
#include "AppLoggerFactory.h"
#include "MessageKey.h"


using namespace Etrek::Worklist::Data::Entity;
namespace Etrek::Worklist::Repository 
{

    WorklistFieldConfigurationRepository::WorklistFieldConfigurationRepository(std::shared_ptr<DatabaseConnectionSetting> connectionSetting)
        : m_connectionSetting(connectionSetting)
    {
        translator = &TranslationProvider::Instance();
        AppLoggerFactory factory(LoggerProvider::Instance(), translator);
        logger = factory.CreateLogger("WorklistRepository");
    }

    QSqlDatabase WorklistFieldConfigurationRepository::createConnection(const QString& connectionName) const {
        QSqlDatabase db = QSqlDatabase::addDatabase("QMYSQL", connectionName);
        db.setHostName(m_connectionSetting->getHostName());
        db.setDatabaseName(m_connectionSetting->getDatabaseName());
        db.setUserName(m_connectionSetting->getEtrekUserName());
        db.setPassword(m_connectionSetting->getPassword());
        db.setPort(m_connectionSetting->getPort());
        return db;
    }

    Result<QVector<WorklistFieldConfiguration>> WorklistFieldConfigurationRepository::getAll() const
    {
        QVector<WorklistFieldConfiguration> result;

        QString connectionName = "worklist_field_configuration_get_all";
		
        {
			QSqlDatabase db = createConnection(connectionName);
			if (!db.open()) {
				QString error = translator->getErrorMessage(FAILED_TO_OPEN_DB_MSG).arg(db.lastError().text());
				logger->LogError(error);
				qDebug() << error;
                return Result<QVector<WorklistFieldConfiguration>>::Failure(error); // Return empty vector on failure
			}

            QSqlQuery query(db);
            query.prepare("SELECT Id, field_name, is_enabled FROM worklist_field_configurations");
            if (!query.exec()) {
                QString error = translator->getErrorMessage(MWL_FAILED_TO_RETRIEVE_IDENTIFIER_ERROR);
                logger->LogError(error);
                qDebug() << error;
                return Result<QVector<WorklistFieldConfiguration>>::Failure(error);
            }
            while (query.next()) {
                WorklistFieldConfiguration config;
                config.Id = query.value(0).toInt();
                config.FieldName = QStringToWorklistFieldName(query.value(1).toString());
                config.IsEnabled = query.value(2).toBool();
                result.append(config);
            }

		}
     
        return Result<QVector<WorklistFieldConfiguration>>::Success(result);
    }

    Result<WorklistFieldConfiguration> WorklistFieldConfigurationRepository::getByFieldName(WorklistFieldName fieldName) const
    {
        WorklistFieldConfiguration config;
        QString connectionName = "worklist_field_configuration_get_by_field_name";

        {
            QSqlDatabase db = createConnection(connectionName);

            if (!db.open()) {
                QString error = translator->getErrorMessage(FAILED_TO_OPEN_DB_MSG);
                logger->LogError(error);
                qDebug() << error;
                return Result<WorklistFieldConfiguration>::Failure(error); // Return empty vector on failure
            }
            QSqlQuery query(db);
            query.prepare("SELECT Id, field_name, is_enabled FROM worklist_field_configurations WHERE field_name = :field_name");
            query.bindValue(":field_name", WorklistFieldNameToString(fieldName));

            if (!query.exec()) {
                QString error = translator->getErrorMessage(MWL_FAILED_TO_RETRIEVE_IDENTIFIER_ERROR);
                logger->LogError(error);
                qDebug() << error;
                return Result<WorklistFieldConfiguration>::Failure(error);
            }

            config.Id = query.value(0).toInt();
            config.FieldName = QStringToWorklistFieldName(query.value(1).toString());
            config.IsEnabled = query.value(2).toBool();
        }
      
        return Result<WorklistFieldConfiguration>::Success(config);
    }

    Result<bool> WorklistFieldConfigurationRepository::updateIsEnabled(WorklistFieldName fieldName, bool isEnabled)
    {
        WorklistFieldConfiguration config;
        bool result = false;
        QString connectionName = "worklist_field_configuration_get_by_field_name";
        {
            QSqlDatabase db = createConnection(connectionName);
            QSqlQuery query(db);
            query.prepare("UPDATE worklist_field_configurations SET is_enabled = :is_enabled WHERE field_name = :field_name");
            query.bindValue(":is_enabled", isEnabled);
            query.bindValue(":field_name", WorklistFieldNameToString(fieldName));
            result = query.exec();
        }
        return Result<bool>::Success(result);
    }

}

