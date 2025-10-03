#ifndef COMMENTREPOSITORY_H
#define COMMENTREPOSITORY_H

#include <QSqlDatabase>
#include <QVector>
#include <QSqlQuery>
#include <QRandomGenerator>
#include <QSqlError>
#include <memory>

#include "Result.h"
#include "DatabaseConnectionSetting.h"
#include "TranslationProvider.h"
#include "AppLogger.h"
#include "ImageComment.h"
#include "DatabaseConnectionSetting.h"


using namespace Etrek::Dicom::Data::Entity;
using namespace Etrek::Core::Log;
using namespace Etrek::Core::Data::Model;

class ImageCommentRepository
{
public:
	explicit ImageCommentRepository(std::shared_ptr<DatabaseConnectionSetting> connectionSetting,
		TranslationProvider* tr = nullptr);

	QVector<ImageComment> getAcceptedComments() const;
	QVector<ImageComment> getRejectComments() const;
	QVector<ImageComment> getAllComments() const;
	Result<ImageComment> addComment(const ImageComment& comment) const;
	Result<ImageComment> removeComment(const ImageComment& comment) const;
	Result<ImageComment> updateComment(const ImageComment& comment) const;

	~ImageCommentRepository();

private:
	// Helpers
	QSqlDatabase createConnection(const QString& connectionName) const;
	static bool toRejectBool(const QString& s);       // "true", "1", "y", "yes" => true
	static QString fromRejectBool(bool b);            // "true"/"false" (or "1"/"0" if you prefer)

	std::shared_ptr<DatabaseConnectionSetting> m_connectionSetting;
	TranslationProvider* translator = nullptr;
	std::shared_ptr<AppLogger> logger;
};

#endif // !COMMENTREPOSITORY_H




