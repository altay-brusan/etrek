#ifndef IMAGECOMMENTCONFIGURATIONBUILDER_H
#define IMAGECOMMENTCONFIGURATIONBUILDER_H

#include "IWidgetDelegateBuilder.h"
#include "ImageCommentConfigurationWidget.h"
#include "ImageCommentConfigurationDelegate.h"
#include "DelegateParameter.h"
#include <memory>

namespace Etrek::Dicom::Delegate {

    class ImageCommentConfigurationBuilder :
        public IWidgetDelegateBuilder<ImageCommentConfigurationWidget, ImageCommentConfigurationDelegate>
    {
    public:
        ImageCommentConfigurationBuilder();
        ~ImageCommentConfigurationBuilder();

        std::pair<ImageCommentConfigurationWidget*, ImageCommentConfigurationDelegate*>
            build(const DelegateParameter& params,
                QWidget* parentWidget = nullptr,
                QObject* parentDelegate = nullptr) override;
    };
}
#endif // IMAGECOMMENTCONFIGURATIONBUILDER_H
