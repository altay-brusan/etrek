#ifndef IMAGECOMMENTCONFIGURATIONWIDGET_H
#define IMAGECOMMENTCONFIGURATIONWIDGET_H

#include <QWidget>
#include <QVector>
#include "ImageComment.h"

using namespace Etrek::Dicom::Data::Entity;

namespace Ui {
class ImageCommentConfigurationWidget;
}

class ImageCommentConfigurationWidget : public QWidget
{
    Q_OBJECT

public:
    explicit ImageCommentConfigurationWidget(const QVector<ImageComment>& comments,QWidget *parent = nullptr);
    ~ImageCommentConfigurationWidget();

private:
	QVector<ImageComment> m_comments;
    QVector<ImageComment> m_acceptedComments;
    QVector<ImageComment> m_rejectedComments;

    bool isNewAcceptedHeading() const;
    bool isNewRejectedHeading() const;

    Ui::ImageCommentConfigurationWidget *ui;
};

#endif // IMAGECOMMENTCONFIGURATIONWIDGET_H
