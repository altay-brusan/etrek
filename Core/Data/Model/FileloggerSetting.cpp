#include "FileloggerSetting.h"


namespace Etrek::Core::Data::Model
{
	FileLoggerSetting::FileLoggerSetting(QObject* parent)
		: QObject{ parent }
	{
	}

	void FileLoggerSetting::setLogDirectory(const QString& logDir) { m_logDirectory = logDir; }

	void FileLoggerSetting::setFileSize(size_t sizeMB)
	{
		if (sizeMB > 0) {
			m_fileSizeMB = sizeMB;
		}
		else {
			m_fileSizeMB = 1;
		}
	}

	void FileLoggerSetting::setFileCount(size_t count)
	{
		if (count > 0) {
			m_fileCount = count;
		}
		else {
			m_fileCount = 5;
		}
	}


	QString FileLoggerSetting::getLogDirectory() const { return m_logDirectory; }

	size_t FileLoggerSetting::getFileSize() const { return m_fileSizeMB; }

	size_t FileLoggerSetting::getFileCount() const { return m_fileCount; }
}
