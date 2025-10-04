#include "ImageCommentRepository.h"
#include "AppLoggerFactory.h"
#include "DatabaseConnectionSetting.h"
#include "AppLogger.h"

using namespace Etrek::Core::Log;


static inline QString kRepoName() { return "CommentRepository"; }
static inline QString kTable() { return "image_comments"; }   // <- table name

ImageCommentRepository::ImageCommentRepository(std::shared_ptr<DatabaseConnectionSetting> connectionSetting,
    TranslationProvider* tr)
    : m_connectionSetting(std::move(connectionSetting))
    , translator(tr ? tr : &TranslationProvider::Instance())
{
    AppLoggerFactory factory(LoggerProvider::Instance(), translator);
    logger = factory.CreateLogger(kRepoName());
}

QSqlDatabase ImageCommentRepository::createConnection(const QString& connectionName) const {
    QSqlDatabase db = QSqlDatabase::addDatabase("QMYSQL", connectionName);
    db.setHostName(m_connectionSetting->getHostName());
    db.setDatabaseName(m_connectionSetting->getDatabaseName());
    db.setUserName(m_connectionSetting->getEtrekUserName());
    db.setPassword(m_connectionSetting->getPassword());
    db.setPort(m_connectionSetting->getPort());
    return db;
}

static inline QString errOpen(const QSqlDatabase& db, TranslationProvider* tr) {
    // If you have translator keys like FAILED_TO_OPEN_DB_MSG, swap this to use them.
    return QString("Failed to open database: %1").arg(db.lastError().text());
}

static inline QString errExec(const QSqlQuery& q, TranslationProvider* tr) {
    return QString("Query failed: %1").arg(q.lastError().text());
}

QVector<ImageComment> ImageCommentRepository::getAcceptedComments() const
{
    QVector<ImageComment> out;
    const QString cx = "comments_accepted_" + QString::number(QRandomGenerator::global()->generate());

    {
        QSqlDatabase db = createConnection(cx);
        if (!db.open()) {
            const auto err = errOpen(db, translator);
            logger->LogError(err);
            return out;
        }

        QSqlQuery q(db);
        q.prepare(QStringLiteral(R"(
            SELECT id, heading, comment, is_reject
            FROM %1
            WHERE is_reject = 0
            ORDER BY id
        )").arg(kTable()));

        if (!q.exec()) {
            const auto err = errExec(q, translator);
            logger->LogError(err);
            return out;
        }

        while (q.next()) {
            ImageComment c;
            c.Id = q.value("id").toInt();
            c.Heading = q.value("heading").toString();
            c.Comment = q.value("comment").toString();
            const bool reject = q.value("is_reject").toBool();
            c.IsRejectComment = fromRejectBool(reject); // "true"/"false"
            out.append(c);
        }
    }

    QSqlDatabase::removeDatabase(cx);
    return out;
}

QVector<ImageComment> ImageCommentRepository::getRejectComments() const
{
    QVector<ImageComment> out;
    const QString cx = "comments_reject_" + QString::number(QRandomGenerator::global()->generate());

    {
        QSqlDatabase db = createConnection(cx);
        if (!db.open()) {
            const auto err = errOpen(db, translator);
            logger->LogError(err);
            return out;
        }

        QSqlQuery q(db);
        q.prepare(QStringLiteral(R"(
            SELECT id, heading, comment, is_reject
            FROM %1
            WHERE is_reject = 1
            ORDER BY id DESC
        )").arg(kTable()));

        if (!q.exec()) {
            const auto err = errExec(q, translator);
            logger->LogError(err);
            return out;
        }

        while (q.next()) {
            ImageComment c;
            c.Id = q.value("id").toInt();
            c.Heading = q.value("heading").toString();
            c.Comment = q.value("comment").toString();
            const bool reject = q.value("is_reject").toBool();
            c.IsRejectComment = fromRejectBool(reject);
            out.append(c);
        }
    }

    QSqlDatabase::removeDatabase(cx);
    return out;
}

QVector<ImageComment> ImageCommentRepository::getAllComments() const
{
    QVector<ImageComment> out;
    const QString cx = "comments_reject_" + QString::number(QRandomGenerator::global()->generate());

    {
        QSqlDatabase db = createConnection(cx);
        if (!db.open()) {
            const auto err = errOpen(db, translator);
            logger->LogError(err);
            return out;
        }

        QSqlQuery q(db);
        q.prepare(QStringLiteral(R"(
            SELECT id, heading, comment, is_reject
            FROM %1            
            ORDER BY id DESC
        )").arg(kTable()));

        if (!q.exec()) {
            const auto err = errExec(q, translator);
            logger->LogError(err);
            return out;
        }

        while (q.next()) {
            ImageComment c;
            c.Id = q.value("id").toInt();
            c.Heading = q.value("heading").toString();
            c.Comment = q.value("comment").toString();
            const bool reject = q.value("is_reject").toBool();
            c.IsRejectComment = fromRejectBool(reject);
            out.append(c);
        }
    }

    QSqlDatabase::removeDatabase(cx);
    return out;
}

Etrek::Specification::Result<ImageComment> ImageCommentRepository::addComment(const ImageComment& comment) const
{
    const QString cx = "comments_add_" + QString::number(QRandomGenerator::global()->generate());
    ImageComment inserted = comment;

    {
        QSqlDatabase db = createConnection(cx);
        if (!db.open()) {
            const auto err = errOpen(db, translator);
            logger->LogError(err);
            return Etrek::Specification::Result<ImageComment>::Failure(err);
        }

        if (comment.Heading.trimmed().isEmpty() && comment.Comment.trimmed().isEmpty()) {
            const auto err = QStringLiteral("Either Heading or Comment must be provided.");
            return Etrek::Specification::Result<ImageComment>::Failure(err);
        }

        QSqlQuery q(db);
        q.prepare(QStringLiteral(R"(
            INSERT INTO %1 (heading, comment, is_reject)
            VALUES (?, ?, ?)
        )").arg(kTable()));

        q.addBindValue(comment.Heading);
        q.addBindValue(comment.Comment);
        q.addBindValue(toRejectBool(comment.IsRejectComment)); // bind as bool/int

        if (!q.exec()) {
            const auto err = errExec(q, translator);
            logger->LogError(err);
            return Etrek::Specification::Result<ImageComment>::Failure(err);
        }

        const QVariant newId = q.lastInsertId();
        if (newId.isValid()) inserted.Id = newId.toInt();
        // normalize stored flag back to "true"/"false"
        inserted.IsRejectComment = fromRejectBool(toRejectBool(comment.IsRejectComment));
    }

    QSqlDatabase::removeDatabase(cx);
    return Etrek::Specification::Result<ImageComment>::Success(inserted, "Inserted");
}

Etrek::Specification::Result<ImageComment> ImageCommentRepository::updateComment(const ImageComment& comment) const
{
    if (comment.Id <= 0) {
        return Etrek::Specification::Result<ImageComment>::Failure("Invalid comment id.");
    }

    const QString cx = "comments_update_" + QString::number(QRandomGenerator::global()->generate());

    {
        QSqlDatabase db = createConnection(cx);
        if (!db.open()) {
            const auto err = errOpen(db, translator);
            logger->LogError(err);
            return Etrek::Specification::Result<ImageComment>::Failure(err);
        }

        QSqlQuery q(db);
        q.prepare(QStringLiteral(R"(
            UPDATE %1
            SET heading = ?,
                comment = ?,
                is_reject = ?
            WHERE id = ?
        )").arg(kTable()));

        q.addBindValue(comment.Heading);
        q.addBindValue(comment.Comment);
        q.addBindValue(toRejectBool(comment.IsRejectComment));
        q.addBindValue(comment.Id);

        if (!q.exec()) {
            const auto err = errExec(q, translator);
            logger->LogError(err);
            return Etrek::Specification::Result<ImageComment>::Failure(err);
        }

        if (q.numRowsAffected() == 0) {
            return Etrek::Specification::Result<ImageComment>::Failure("No rows updated (comment not found?).");
        }
    }

    QSqlDatabase::removeDatabase(cx);
    return Etrek::Specification::Result<ImageComment>::Success(comment, "Updated");
}

Etrek::Specification::Result<ImageComment> ImageCommentRepository::removeComment(const ImageComment& comment) const
{
    if (comment.Id <= 0) {
        return Etrek::Specification::Result<ImageComment>::Failure("Invalid comment id.");
    }

    const QString cx = "comments_delete_" + QString::number(QRandomGenerator::global()->generate());

    {
        QSqlDatabase db = createConnection(cx);
        if (!db.open()) {
            const auto err = errOpen(db, translator);
            logger->LogError(err);
            return Etrek::Specification::Result<ImageComment>::Failure(err);
        }

        QSqlQuery q(db);
        q.prepare(QStringLiteral(R"(
            DELETE FROM %1 WHERE id = ?
        )").arg(kTable()));

        q.addBindValue(comment.Id);

        if (!q.exec()) {
            const auto err = errExec(q, translator);
            logger->LogError(err);
            return Etrek::Specification::Result<ImageComment>::Failure(err);
        }

        if (q.numRowsAffected() == 0) {
            return Etrek::Specification::Result<ImageComment>::Failure("No rows deleted (comment not found?).");
        }
    }

    QSqlDatabase::removeDatabase(cx);
    return Etrek::Specification::Result<ImageComment>::Success(comment, "Deleted");
}

ImageCommentRepository::~ImageCommentRepository() = default;

// ---- Helpers ----
bool ImageCommentRepository::toRejectBool(const QString& s)
{
    const auto v = s.trimmed().toLower();
    return (v == "1" || v == "true" || v == "y" || v == "yes");
}

QString ImageCommentRepository::fromRejectBool(bool b)
{
    // Keep UI/model simple; you can switch to "1"/"0" if you prefer:
    return b ? QStringLiteral("true") : QStringLiteral("false");
}
