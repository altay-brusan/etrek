#include "ImageCommentConfigurationBuilder.h"
#include "ImageCommentRepository.h"
namespace Etrek::Dicom::Delegate 
{
	using Etrek::Dicom::Repository::ImageCommentRepository;

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
        // Builder creates repository from dbConnection using smart pointer (FIXES MEMORY LEAK!)
        auto repository = std::make_shared<ImageCommentRepository>(params.dbConnection);
        
        auto allComments = repository->getAllComments();
        auto widget = new ImageCommentConfigurationWidget(allComments, parentWidget);
        
        // If delegate needs to perform CRUD operations, pass repository to it:
        // auto delegate = new ImageCommentConfigurationDelegate(widget, repository, parentDelegate);
        // For now, delegate doesn't receive repository (read-only widget)
        auto delegate = new ImageCommentConfigurationDelegate(widget, parentDelegate);

        // If you need to attach other delegates:
        // if (delegate) delegate->AttachDelegates(params.delegates.values());

        return { widget, delegate };
    }
}
