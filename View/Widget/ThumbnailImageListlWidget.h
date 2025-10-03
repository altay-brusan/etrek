#ifndef THUMBNAILIMAGELISTLWIDGET_H
#define THUMBNAILIMAGELISTLWIDGET_H

#include <QWidget>

namespace Ui {
class ThumbnailImageListlWidget;
}

class ThumbnailImageListlWidget : public QWidget
{
    Q_OBJECT

public:
    explicit ThumbnailImageListlWidget(QWidget *parent = nullptr);
    ~ThumbnailImageListlWidget();

private:
    Ui::ThumbnailImageListlWidget *ui;
};

#endif // THUMBNAILIMAGELISTLWIDGET_H
