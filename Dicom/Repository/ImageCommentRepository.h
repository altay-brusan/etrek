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

namespace Etrek::Dicom::Repository {

    namespace spc = Etrek::Specification;
    namespace ent = Etrek::Dicom::Data::Entity;
    namespace mdl = Etrek::Core::Data::Model;
    namespace glob = Etrek::Core::Globalization;
    namespace lg = Etrek::Core::Log;

    class ImageCommentRepository
    {
    public:
        explicit ImageCommentRepository(std::shared_ptr<mdl::DatabaseConnectionSetting> connectionSetting,
            glob::TranslationProvider* tr = nullptr);

        QVector<ent::ImageComment> getAcceptedComments() const;
        QVector<ent::ImageComment> getRejectComments() const;
        QVector<ent::ImageComment> getAllComments() const;
        spc::Result<ent::ImageComment> addComment(const ent::ImageComment& comment) const;
        spc::Result<ent::ImageComment> removeComment(const ent::ImageComment& comment) const;
        spc::Result<ent::ImageComment> updateComment(const ent::ImageComment& comment) const;

        ~ImageCommentRepository();

    private:
        // Helpers
        QSqlDatabase createConnection(const QString& connectionName) const;
        static bool toRejectBool(const QString& s);       // "true", "1", "y", "yes" => true
        static QString fromRejectBool(bool b);            // "true"/"false" (or "1"/"0" if you prefer)

        std::shared_ptr<mdl::DatabaseConnectionSetting> m_connectionSetting;
        glob::TranslationProvider* translator = nullptr;
        std::shared_ptr<lg::AppLogger> logger;
    };

} // namespace Etrek::Dicom::Repository

#endif // !COMMENTREPOSITORY_H




