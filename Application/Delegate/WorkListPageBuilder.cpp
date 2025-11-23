#include "WorkListPageBuilder.h"
#include "ScanProtocolRepository.h"
#include "DicomRepository.h"
#include "DicomTagRepository.h"
#include "WorklistRepository.h"
#include "IWorklistRepository.h"

namespace Etrek::Application::Delegate {
	WorkListPageBuilder::WorkListPageBuilder()
	{
	}

	WorkListPageBuilder::~WorkListPageBuilder()
	{
	}

    std::pair<WorkListPage*, WorkListPageDelegate*>
        WorkListPageBuilder::build(const DelegateParameter& params,
            QWidget* parentWidget,
            QObject* parentDelegate)
    {
        auto repository = std::make_shared<Etrek::Worklist::Repository::WorklistRepository>(params.dbConnection);
        auto scanRepository = std::make_shared<Etrek::ScanProtocol::Repository::ScanProtocolRepository>(params.dbConnection);
        auto dicomRepository = std::make_shared<Etrek::Dicom::Repository::DicomRepository>(params.dbConnection);
        auto dicomTagRepository = std::make_shared<Etrek::Dicom::Repository::DicomTagRepository>(params.dbConnection);
        std::shared_ptr<Etrek::Worklist::Repository::IWorklistRepository> irepository = std::static_pointer_cast<Etrek::Worklist::Repository::IWorklistRepository>(repository);
        auto* widget = new WorkListPage(irepository, parentWidget);

        auto* delegate = new WorkListPageDelegate(widget, repository, scanRepository, dicomRepository, dicomTagRepository, params.contextManager, parentDelegate);

        // If you need to attach other delegates:
        // if (delegate) delegate->AttachDelegates(params.delegates.values());

        return { widget, delegate };
    }

}
