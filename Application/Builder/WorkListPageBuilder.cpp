#include "WorkListPageBuilder.h"
#include "IWorklistRepository.h"
#include "WorklistRepository.h"
#include "ScanProtocolRepository.h"
#include "DicomRepository.h"
#include "DicomTagRepository.h"

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
        // Builder creates repositories from dbConnection
        auto worklistRepository = std::make_shared<Etrek::Worklist::Repository::WorklistRepository>(params.dbConnection);
        auto scanRepository = std::make_shared<Etrek::ScanProtocol::Repository::ScanProtocolRepository>(params.dbConnection, nullptr);
        auto dicomRepository = std::make_shared<Etrek::Dicom::Repository::DicomRepository>(params.dbConnection);
        auto dicomTagRepository = std::make_shared<Etrek::Dicom::Repository::DicomTagRepository>(params.dbConnection);

        std::shared_ptr<Etrek::Worklist::Repository::IWorklistRepository> irepository =
            std::static_pointer_cast<Etrek::Worklist::Repository::IWorklistRepository>(worklistRepository);

        auto* widget = new WorkListPage(irepository, parentWidget);

        // Delegate receives repositories for CRUD operations and dbConnection
        auto* delegate = new WorkListPageDelegate(
            widget,
            worklistRepository,
            scanRepository,
            dicomRepository,
            dicomTagRepository,
            params.dbConnection,
            params.contextManager,
            parentDelegate);

        // If you need to attach other delegates:
        // if (delegate) delegate->AttachDelegates(params.delegates.values());

        return { widget, delegate };
    }

}
