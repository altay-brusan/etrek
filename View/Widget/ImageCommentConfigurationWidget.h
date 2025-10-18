#ifndef IMAGECOMMENTCONFIGURATIONWIDGET_H
#define IMAGECOMMENTCONFIGURATIONWIDGET_H

#include <QWidget>
#include <QVector>
#include "ImageComment.h"

namespace Ui {
class ImageCommentConfigurationWidget;
}

class ImageCommentConfigurationWidget : public QWidget
{
    Q_OBJECT

public:
    explicit ImageCommentConfigurationWidget(const QVector<Etrek::Dicom::Data::Entity::ImageComment>& comments,QWidget *parent = nullptr);
    ~ImageCommentConfigurationWidget();

private:
	QVector<Etrek::Dicom::Data::Entity::ImageComment> m_comments;
    QVector<Etrek::Dicom::Data::Entity::ImageComment> m_acceptedComments;
    QVector<Etrek::Dicom::Data::Entity::ImageComment> m_rejectedComments;

    bool isNewAcceptedHeading() const;
    bool isNewRejectedHeading() const;

    Ui::ImageCommentConfigurationWidget *ui;
};

#endif // IMAGECOMMENTCONFIGURATIONWIDGET_H
