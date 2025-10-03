#ifndef IMAGECOMMENT_H
#define IMAGECOMMENT_H

#include <QString>
#include <QMetaType>
#include <QDate>
#include <QDateTime>


namespace Etrek::Dicom::Data::Entity {

    class ImageComment {
    public:
        int Id = -1;                        ///< Primary key       
        QString Heading;                    ///< short heading of the comment
        QString Comment;                    ///< full image note
		QString IsRejectComment;            ///< is reject comment flag
       
        QDateTime CreateDate;
        QDateTime UpdateDate;

        ImageComment() = default;

        bool operator==(const ImageComment& other) const {
            return Id == other.Id;
        }
    };

} // namespace Etrek::Dicom::Data::ImageComment

Q_DECLARE_METATYPE(Etrek::Dicom::Data::Entity::ImageComment)

#endif // IMAGECOMMENT_H
