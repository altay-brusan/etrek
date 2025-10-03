#ifndef FILELOGGERSETTING_H
#define FILELOGGERSETTING_H

#include <QObject>
#include <QDateTime>

namespace Etrek::Core::Data::Model
{
	/**
	 * @class FileLoggerSetting
	 * @brief Represents the settings for file-based logging.
	 *
	 * This class encapsulates the configuration for file logging, including the log directory,
	 * maximum file size, and number of log files to keep. It provides methods to set and retrieve
	 * these parameters.
	 */
    class FileLoggerSetting : public QObject
    {
        Q_OBJECT
    public:
        explicit FileLoggerSetting(QObject *parent = nullptr);
        void setLogDirectory(const QString &logDirectory);
        void setFileSize(size_t sizeMB);
        void setFileCount(size_t count);

        QString getLogDirectory() const;
	    size_t getFileSize() const;	
	    size_t getFileCount() const;

    private:
        QString m_logDirectory{"."};
	    size_t m_fileSizeMB{ 1}; // Default 1 MB
	    size_t m_fileCount{ 5 }; // Default 5 files
    
    };
}
#endif // FILELOGGERSETTING_H
