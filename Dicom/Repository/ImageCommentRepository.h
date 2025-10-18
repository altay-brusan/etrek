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

namespace Etrek::Dicom::Repository {


    class ImageCommentRepository
    {
    public:
        explicit ImageCommentRepository(std::shared_ptr<Etrek::Core::Data::Model::DatabaseConnectionSetting> connectionSetting,
            Etrek::Core::Globalization::TranslationProvider* tr = nullptr);

        QVector<Etrek::Dicom::Data::Entity::ImageComment> getAcceptedComments() const;
        QVector<Etrek::Dicom::Data::Entity::ImageComment> getRejectComments() const;
        QVector<Etrek::Dicom::Data::Entity::ImageComment> getAllComments() const;
        Etrek::Specification::Result<Etrek::Dicom::Data::Entity::ImageComment> addComment(const Etrek::Dicom::Data::Entity::ImageComment& comment) const;
        Etrek::Specification::Result<Etrek::Dicom::Data::Entity::ImageComment> removeComment(const Etrek::Dicom::Data::Entity::ImageComment& comment) const;
        Etrek::Specification::Result<Etrek::Dicom::Data::Entity::ImageComment> updateComment(const Etrek::Dicom::Data::Entity::ImageComment& comment) const;

        ~ImageCommentRepository();

    private:
        // Helpers
        QSqlDatabase createConnection(const QString& connectionName) const;
        static bool toRejectBool(const QString& s);       // "true", "1", "y", "yes" => true
        static QString fromRejectBool(bool b);            // "true"/"false" (or "1"/"0" if you prefer)

        std::shared_ptr<Etrek::Core::Data::Model::DatabaseConnectionSetting> m_connectionSetting;
        Etrek::Core::Globalization::TranslationProvider* translator = nullptr;
        std::shared_ptr<Etrek::Core::Log::AppLogger> logger;
    };

} // namespace Etrek::Dicom::Repository

#endif // !COMMENTREPOSITORY_H




