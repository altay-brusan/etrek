#ifndef IMAGEVIEWERPAGEBUILDER_H
#define IMAGEVIEWERPAGEBUILDER_H

#include "IWidgetDelegateBuilder.h"
#include "ImageViewerPageDelegate.h"
#include "ImageViewerPage.h"
#include "DelegateParameter.h"
#include <memory>

namespace Etrek::Application::Delegate {

/**
 * @class ImageViewerPageBuilder
 * @brief Builder for ImageViewerPage and ImageViewerPageDelegate following MVD pattern.
 *
 * Responsibilities:
 * - Create ImageViewerPage widget
 * - Instantiate DicomRepository
 * - Create ImageViewerPageDelegate with injected dependencies
 * - Return widget-delegate pair
 */
class ImageViewerPageBuilder
    : public IWidgetDelegateBuilder<ImageViewerPage, ImageViewerPageDelegate>
{
public:
    ImageViewerPageBuilder();
    ~ImageViewerPageBuilder();

    /**
     * @brief Builds ImageViewerPage and ImageViewerPageDelegate with all dependencies.
     * @param params DelegateParameter containing dbConnection and contextManager
     * @param parentWidget Optional parent widget
     * @param parentDelegate Optional parent delegate
     * @return Pair of ImageViewerPage widget and ImageViewerPageDelegate
     */
    std::pair<ImageViewerPage*, ImageViewerPageDelegate*>
        build(const DelegateParameter& params,
              QWidget* parentWidget = nullptr,
              QObject* parentDelegate = nullptr) override;
};

} // namespace Etrek::Application::Delegate

#endif // IMAGEVIEWERPAGEBUILDER_H
