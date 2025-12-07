#include "ExamPageBuilder.h"
#include "WorklistRepository.h"
#include "DicomRepository.h"
#include "ScanProtocolRepository.h"
#include "DeviceRepository.h"

namespace Etrek::Application::Delegate {

ExamPageBuilder::ExamPageBuilder()
{
}

ExamPageBuilder::~ExamPageBuilder()
{
}

std::pair<ExamPage*, ExamPageDelegate*>
ExamPageBuilder::build(const DelegateParameter& params,
                       QWidget* parentWidget,
                       QObject* parentDelegate)
{
    // Create the page widget
    auto* page = new ExamPage(parentWidget);

    // Create repositories from dbConnection
    auto worklistRepo = std::make_shared<Etrek::Worklist::Repository::WorklistRepository>(
        params.dbConnection
    );

    auto dicomRepo = std::make_shared<Etrek::Dicom::Repository::DicomRepository>(
        params.dbConnection
    );

    auto scanProtocolRepo = std::make_shared<Etrek::ScanProtocol::Repository::ScanProtocolRepository>(
        params.dbConnection,
        nullptr  // QObject parent
    );

    auto deviceRepo = std::make_shared<Etrek::Device::Repository::DeviceRepository>(
        params.dbConnection,
        nullptr  // QObject parent
    );

    // Create the delegate with all dependencies
    auto* delegate = new ExamPageDelegate(
        page,
        worklistRepo,
        dicomRepo,
        scanProtocolRepo,
        deviceRepo,
        params.dbConnection,
        params.contextManager,
        parentDelegate
    );

    return { page, delegate };
}

} // namespace Etrek::Application::Delegate
