#include "ImageCommentConfigurationBuilder.h"
#include "ImageCommentRepository.h"
namespace Etrek::Dicom::Delegate {

    ImageCommentConfigurationBuilder::ImageCommentConfigurationBuilder()
    {
    }

    ImageCommentConfigurationBuilder::~ImageCommentConfigurationBuilder()
    {
    }

    std::pair<ImageCommentConfigurationWidget*, ImageCommentConfigurationDelegate*>
        ImageCommentConfigurationBuilder::build(const DelegateParameter& params,
            QWidget* parentWidget,
            QObject* parentDelegate)
    {
		ImageCommentRepository* repository = new  ImageCommentRepository(params.dbConnection);
		auto allComments = repository->getAllComments();
        auto widget = new ImageCommentConfigurationWidget(allComments,parentWidget);
        auto delegate = new ImageCommentConfigurationDelegate(widget, parentDelegate);

        // If you need to attach other delegates:
        // if (delegate) delegate->AttachDelegates(params.delegates.values());

        return { widget, delegate };
    }
}
