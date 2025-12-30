#include "ImageViewerPageBuilder.h"
#include "DicomRepository.h"

namespace Etrek::Application::Delegate {

ImageViewerPageBuilder::ImageViewerPageBuilder()
{
}

ImageViewerPageBuilder::~ImageViewerPageBuilder()
{
}

std::pair<ImageViewerPage*, ImageViewerPageDelegate*>
ImageViewerPageBuilder::build(const DelegateParameter& params,
                              QWidget* parentWidget,
                              QObject* parentDelegate)
{
    // Create the page widget
    auto* page = new ImageViewerPage(parentWidget);

    // Create repositories from dbConnection
    auto dicomRepo = std::make_shared<Etrek::Dicom::Repository::DicomRepository>(
        params.dbConnection
    );

    // Create the delegate with all dependencies
    auto* delegate = new ImageViewerPageDelegate(
        page,
        dicomRepo,
        params.dbConnection,
        params.contextManager,
        parentDelegate
    );

    return { page, delegate };
}

} // namespace Etrek::Application::Delegate
