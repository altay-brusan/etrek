#include "ThumbnailImageListlWidget.h"
#include "ui_ThumbnailImageListlWidget.h"

ThumbnailImageListlWidget::ThumbnailImageListlWidget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::ThumbnailImageListlWidget)
{
    ui->setupUi(this);
}

ThumbnailImageListlWidget::~ThumbnailImageListlWidget()
{
    delete ui;
}
