#include "ExamPageDelegate.h"
#include "ExamPage.h"
#include "WorklistRepository.h"
#include "DicomRepository.h"
#include "ScanProtocolRepository.h"
#include "DeviceRepository.h"
#include "IContextManager.h"
#include "ExaminationContext.h"
#include "DatabaseConnectionSetting.h"
#include "WorklistEnum.h"
#include "Result.h"

#include <QMessageBox>
#include <QDateTime>
#include <QDebug>
#include <QUuid>
#include <QTimer>

// VTK includes
#include <QVTKOpenGLNativeWidget.h>
#include <vtkGenericOpenGLRenderWindow.h>
#include <vtkRenderer.h>
#include <vtkImageActor.h>
#include <vtkImageData.h>
#include <vtkInteractorStyleImage.h>
#include <vtkSmartPointer.h>

namespace Etrek::Application::Delegate {

ExamPageDelegate::ExamPageDelegate(
    ExamPage* ui,
    std::shared_ptr<Etrek::Worklist::Repository::WorklistRepository> worklistRepo,
    std::shared_ptr<Etrek::Dicom::Repository::DicomRepository> dicomRepo,
    std::shared_ptr<Etrek::ScanProtocol::Repository::ScanProtocolRepository> scanProtocolRepo,
    std::shared_ptr<Etrek::Device::Repository::DeviceRepository> deviceRepo,
    std::shared_ptr<Etrek::Core::Data::Model::DatabaseConnectionSetting> dbConnection,
    std::weak_ptr<Etrek::Context::IContextManager> contextManager,
    QObject* parent)
    : QObject(parent)
    , ui(ui)
    , m_worklistRepo(worklistRepo)
    , m_dicomRepo(dicomRepo)
    , m_scanProtocolRepo(scanProtocolRepo)
    , m_deviceRepo(deviceRepo)
    , m_dbConnection(dbConnection)
    , m_contextManager(contextManager)
{
    qDebug() << "[ExamPageDelegate] Constructor called";

    // Setup signal/slot connections
    setupConnections();

    // Initialize VTK viewer
    // initializeVtkViewer();  // TODO: Uncomment when VTK widget is added to UI

    // Load examination context and initialize page
    QTimer::singleShot(0, this, &ExamPageDelegate::onPageLoaded);
}

ExamPageDelegate::~ExamPageDelegate()
{
    qDebug() << "[ExamPageDelegate] Destructor called";

    // Clean up VTK objects
    if (m_imageActor) {
        m_imageActor->Delete();
        m_imageActor = nullptr;
    }

    if (m_renderer) {
        m_renderer->Delete();
        m_renderer = nullptr;
    }

    if (m_renderWindow) {
        m_renderWindow->Delete();
        m_renderWindow = nullptr;
    }
}

// --- IDelegate Implementation ---

void ExamPageDelegate::attachDelegates(const QVector<QObject*>& delegates)
{
    for (auto* delegate : delegates) {
        if (delegate && !m_childDelegates.contains(delegate)) {
            m_childDelegates.append(QPointer<QObject>(delegate));
        }
    }
}

// --- IPageAction Implementation ---

void ExamPageDelegate::apply()
{
    qDebug() << "[ExamPageDelegate] apply() - Saving intermediate state";
    // Save current technique parameters, image adjustments, etc.
    // This is called when user clicks "Apply" without completing examination
}

void ExamPageDelegate::accept()
{
    qDebug() << "[ExamPageDelegate] accept() - Completing examination";

    // Mark examination as complete
    if (m_examinationContext) {
        m_examinationContext->markComplete();
    }

    // Update worklist status to COMPLETED
    updateWorklistStatus(ProcedureStepStatus::COMPLETED);

    // Emit completion signal
    emit examinationCompleted(m_studyId);

    // Close examination page
    emit closeExamination();
}

void ExamPageDelegate::reject()
{
    qDebug() << "[ExamPageDelegate] reject() - Cancelling examination";

    // Confirm cancellation
    QMessageBox::StandardButton reply = QMessageBox::question(
        ui,
        "Cancel Examination",
        "Are you sure you want to cancel this examination?\nAny unsaved changes will be lost.",
        QMessageBox::Yes | QMessageBox::No
    );

    if (reply == QMessageBox::Yes) {
        // Update worklist status back to PENDING or SCHEDULED
        updateWorklistStatus(ProcedureStepStatus::CANCELLED);

        // Close examination page
        emit closeExamination();
    }
}

// --- Private Slots: Initialization ---

void ExamPageDelegate::onPageLoaded()
{
    qDebug() << "[ExamPageDelegate] onPageLoaded()";

    // Load examination context from ContextManager
    loadExaminationContext();

    // Load patient demographics into UI
    loadPatientDemographics();

    // Load views/series for this examination
    loadViews();

    // Load technique parameters for first view
    if (!m_views.isEmpty()) {
        loadTechniqueParameters();
    }

    // Create study and series in database
    createStudy();
    createSeriesForViews();
}

void ExamPageDelegate::loadExaminationContext()
{
    qDebug() << "[ExamPageDelegate] loadExaminationContext()";

    if (auto ctxMgr = m_contextManager.lock()) {
        auto workflowCtx = ctxMgr->workflowContext("Examination");
        m_examinationContext = std::dynamic_pointer_cast<Etrek::Core::Context::ExaminationContext>(workflowCtx);

        if (!m_examinationContext) {
            displayErrorMessage("Context Error", "Examination context is not available.");
            return;
        }

        // Get worklist entry from context
        auto entryOpt = m_examinationContext->worklistEntry();
        if (entryOpt.has_value()) {
            m_worklistEntry = entryOpt.value();
            qDebug() << "[ExamPageDelegate] Loaded worklist entry ID:" << m_worklistEntry.Id;
        } else {
            displayErrorMessage("Context Error", "Worklist entry not found in examination context.");
        }
    } else {
        displayErrorMessage("Context Error", "Context manager is not available.");
    }
}

void ExamPageDelegate::loadPatientDemographics()
{
    qDebug() << "[ExamPageDelegate] loadPatientDemographics()";

    if (!m_examinationContext) {
        qWarning() << "[ExamPageDelegate] Cannot load demographics - no examination context";
        return;
    }

    // Extract patient information from examination context
    QString patientName = m_examinationContext->patientName();
    QString patientId = m_examinationContext->patientId();
    QString patientGender = m_examinationContext->patientGender();
    QDate birthDate = m_examinationContext->patientBirthDate();
    QString accessionNumber = m_examinationContext->accessionNumber();

    // Calculate age
    int age = 0;
    if (birthDate.isValid()) {
        age = birthDate.daysTo(QDate::currentDate()) / 365;
    }

    qDebug() << "[ExamPageDelegate] Patient:" << patientName
             << "ID:" << patientId
             << "Gender:" << patientGender
             << "Age:" << age;

    // TODO: Update UI widgets with patient demographics
    // ui->getExamTitleWidget()->setPatientName(patientName);
    // ui->getExamTitleWidget()->setPatientId(patientId);
    // ui->getExamTitleWidget()->setGender(patientGender);
    // ui->getExamTitleWidget()->setAge(age);
    // ui->getExamTitleWidget()->setAccessionNumber(accessionNumber);
}

void ExamPageDelegate::loadViews()
{
    qDebug() << "[ExamPageDelegate] loadViews()";

    if (!m_examinationContext) {
        qWarning() << "[ExamPageDelegate] Cannot load views - no examination context";
        return;
    }

    // Get view IDs from examination context
    QVector<int> viewIds = m_examinationContext->viewIds();

    if (viewIds.isEmpty()) {
        qDebug() << "[ExamPageDelegate] No views selected - this is a RIS worklist entry";
        // TODO: For RIS entries, extract views from DICOM attributes
        return;
    }

    // Load view details from repository
    for (int viewId : viewIds) {
        auto viewResult = m_scanProtocolRepo->getViewById(viewId);
        if (viewResult.isSuccess) {
            m_views.append(viewResult.value);
            qDebug() << "[ExamPageDelegate] Loaded view:" << viewResult.value.Name;
        } else {
            qWarning() << "[ExamPageDelegate] Failed to load view ID:" << viewId;
        }
    }

    // TODO: Update UI with view list/thumbnails
    qDebug() << "[ExamPageDelegate] Total views loaded:" << m_views.size();
}

void ExamPageDelegate::loadTechniqueParameters()
{
    qDebug() << "[ExamPageDelegate] loadTechniqueParameters()";

    if (m_views.isEmpty()) {
        qWarning() << "[ExamPageDelegate] No views available to load technique parameters";
        return;
    }

    // Get current view
    const auto& currentView = m_views[m_currentSeriesIndex];

    // TODO: Load technique parameters for this view based on:
    // - Body part
    // - Patient size
    // - View position (AP, LAT, etc.)

    // For now, use placeholder
    qDebug() << "[ExamPageDelegate] Loading technique parameters for view:" << currentView.Name;

    // TODO: Update ExposureDetailWidget with technique parameters
    // ui->getExposureDetailWidget()->load(m_currentTechnique);
}

// --- Private Slots: View/Series Management ---

void ExamPageDelegate::onViewSelected(int seriesId)
{
    qDebug() << "[ExamPageDelegate] onViewSelected() - Series ID:" << seriesId;

    // Find index of this series
    int index = m_seriesIds.indexOf(seriesId);
    if (index >= 0 && index < m_views.size()) {
        m_currentSeriesIndex = index;
        loadTechniqueParameters();

        // TODO: Display appropriate image for this series in VTK viewer
    }
}

void ExamPageDelegate::onNextView()
{
    if (m_currentSeriesIndex < m_views.size() - 1) {
        m_currentSeriesIndex++;
        loadTechniqueParameters();
        qDebug() << "[ExamPageDelegate] Moved to next view - index:" << m_currentSeriesIndex;
    }
}

void ExamPageDelegate::onPreviousView()
{
    if (m_currentSeriesIndex > 0) {
        m_currentSeriesIndex--;
        loadTechniqueParameters();
        qDebug() << "[ExamPageDelegate] Moved to previous view - index:" << m_currentSeriesIndex;
    }
}

// --- Private Slots: Image Acquisition ---

void ExamPageDelegate::onReadyButtonClicked()
{
    qDebug() << "[ExamPageDelegate] onReadyButtonClicked() - Preparing for exposure";

    // Configure generator with current technique parameters
    configureGenerator();

    // Configure detector
    configureDetector();

    // Enable expose button
    // TODO: ui->getExposureApplicationControlWidget()->enableExposeButton();

    displayInfoMessage("Ready", "System is ready for exposure. Press EXPOSE to acquire image.");
}

void ExamPageDelegate::onExposeButtonClicked()
{
    qDebug() << "[ExamPageDelegate] onExposeButtonClicked() - Starting exposure";

    // TODO: Send exposure command to generator
    // This would typically interface with hardware via DeviceRepository
    // For now, simulate image acquisition

    displayInfoMessage("Exposure", "Acquiring image...");

    // TODO: Actual hardware integration
    // Hardware callback would eventually call onImageReceived()
}

void ExamPageDelegate::onImageReceived(const QByteArray& imageData)
{
    qDebug() << "[ExamPageDelegate] onImageReceived() - Image size:" << imageData.size() << "bytes";

    if (m_seriesIds.isEmpty() || m_currentSeriesIndex >= m_seriesIds.size()) {
        displayErrorMessage("Acquisition Error", "No active series to store image.");
        return;
    }

    // Get current series ID
    int currentSeriesId = m_seriesIds[m_currentSeriesIndex];

    // Create image record in database
    createImage(imageData, currentSeriesId);

    // Display image in VTK viewer
    // TODO: Convert imageData to vtkImageData and display
    // displayImage(imageData, width, height, bitsAllocated);

    // Update thumbnails
    updateThumbnails();

    // Move to next view if available
    if (m_currentSeriesIndex < m_views.size() - 1) {
        onNextView();
    } else {
        displayInfoMessage("Acquisition Complete", "All views have been acquired.");
    }
}

void ExamPageDelegate::onAcquisitionComplete()
{
    qDebug() << "[ExamPageDelegate] onAcquisitionComplete()";
    displayInfoMessage("Complete", "Image acquisition completed successfully.");
}

// --- Private Slots: Technical Parameters ---

void ExamPageDelegate::onKvpChanged(int kvp)
{
    qDebug() << "[ExamPageDelegate] KVP changed to:" << kvp;
    m_currentTechnique.Kvp = kvp;
}

void ExamPageDelegate::onMaChanged(int ma)
{
    qDebug() << "[ExamPageDelegate] mA changed to:" << ma;
    m_currentTechnique.Ma = ma;
}

void ExamPageDelegate::onMasChanged(int mas)
{
    qDebug() << "[ExamPageDelegate] mAs changed to:" << mas;
    // mAs = mA * time, so update time if needed
}

void ExamPageDelegate::onTimeChanged(int time)
{
    qDebug() << "[ExamPageDelegate] Time changed to:" << time;
    m_currentTechnique.Ms = time;
}

void ExamPageDelegate::onDensityChanged(int density)
{
    qDebug() << "[ExamPageDelegate] Density changed to:" << density;
    m_currentTechnique.AecDensity = density;
}

// --- Private Slots: Study/Series Operations ---

void ExamPageDelegate::onNewStudyClicked()
{
    qDebug() << "[ExamPageDelegate] onNewStudyClicked()";
    // TODO: Allow creating additional studies for same patient
}

void ExamPageDelegate::onEditStudyClicked()
{
    qDebug() << "[ExamPageDelegate] onEditStudyClicked()";
    // TODO: Edit current study metadata
}

void ExamPageDelegate::onNewViewClicked()
{
    qDebug() << "[ExamPageDelegate] onNewViewClicked()";
    // TODO: Add additional view/series to current study
}

// --- Private Slots: Action Buttons ---

void ExamPageDelegate::onCloseClicked()
{
    qDebug() << "[ExamPageDelegate] onCloseClicked()";
    reject();  // Confirm and close
}

void ExamPageDelegate::onPrintClicked()
{
    qDebug() << "[ExamPageDelegate] onPrintClicked()";
    // TODO: Print current image or study
}

// --- Private Methods: Database Operations ---

void ExamPageDelegate::createStudy()
{
    qDebug() << "[ExamPageDelegate] createStudy()";

    if (!m_examinationContext) {
        displayErrorMessage("Database Error", "Cannot create study - no examination context.");
        return;
    }

    // Get or create patient record
    m_patientId = getOrCreatePatient();
    if (m_patientId < 0) {
        displayErrorMessage("Database Error", "Failed to create or retrieve patient record.");
        return;
    }

    // Generate DICOM Study Instance UID
    QString studyInstanceUid = generateDicomUid("study");

    // TODO: Create study record using DicomRepository
    // This is a placeholder - actual implementation depends on your repository structure
    /*
    Study study;
    study.patientId = m_patientId;
    study.studyInstanceUid = studyInstanceUid;
    study.studyId = QString::number(QDateTime::currentMSecsSinceEpoch());
    study.accessionNumber = m_examinationContext->accessionNumber();
    study.studyDate = QDate::currentDate();
    study.studyTime = QTime::currentTime();
    study.studyDescription = m_examinationContext->studyDescription();
    study.referringPhysicianName = m_examinationContext->referringPhysician();

    auto result = m_dicomRepo->createStudy(study);
    if (result.isSuccess) {
        m_studyId = result.value;
        qDebug() << "[ExamPageDelegate] Study created with ID:" << m_studyId;

        // Update worklist entry with study UID and status
        updateWorklistStatus(ProcedureStepStatus::IN_PROGRESS, studyInstanceUid);
    } else {
        displayErrorMessage("Database Error", "Failed to create study: " + result.message);
    }
    */

    // Placeholder: Just log for now
    qDebug() << "[ExamPageDelegate] Study creation - Patient ID:" << m_patientId
             << "Study UID:" << studyInstanceUid;

    // Update worklist status to IN_PROGRESS
    updateWorklistStatus(ProcedureStepStatus::IN_PROGRESS, studyInstanceUid);
}

void ExamPageDelegate::createSeriesForViews()
{
    qDebug() << "[ExamPageDelegate] createSeriesForViews() - View count:" << m_views.size();

    if (m_studyId < 0) {
        qWarning() << "[ExamPageDelegate] Cannot create series - no study ID";
        return;
    }

    // Create one series for each view
    for (int i = 0; i < m_views.size(); ++i) {
        createSeries(m_views[i], i + 1);
    }

    qDebug() << "[ExamPageDelegate] Created" << m_seriesIds.size() << "series records";
}

void ExamPageDelegate::createSeries(const Etrek::ScanProtocol::Data::Entity::View& view, int sequenceNumber)
{
    qDebug() << "[ExamPageDelegate] createSeries() - View:" << view.Name << "Sequence:" << sequenceNumber;

    // Generate DICOM Series Instance UID
    QString seriesInstanceUid = generateDicomUid("series");

    // TODO: Create series record using DicomRepository
    /*
    Series series;
    series.studyId = m_studyId;
    series.seriesInstanceUid = seriesInstanceUid;
    series.seriesNumber = sequenceNumber;
    series.modality = "DX";  // Digital Radiography
    series.seriesDescription = view.Name;
    series.bodyPartExamined = view.BodyPart;
    series.patientPosition = view.PatientPosition;
    series.viewPosition = view.ViewPosition;
    series.imageLaterality = view.ImageLaterality;

    auto result = m_dicomRepo->createSeries(series);
    if (result.isSuccess) {
        m_seriesIds.append(result.value);
        qDebug() << "[ExamPageDelegate] Series created with ID:" << result.value;
    } else {
        displayErrorMessage("Database Error", "Failed to create series: " + result.message);
    }
    */

    // Placeholder: Just log for now
    qDebug() << "[ExamPageDelegate] Series creation - Study ID:" << m_studyId
             << "Series UID:" << seriesInstanceUid
             << "View:" << view.Name;

    // Placeholder: Add dummy series ID
    m_seriesIds.append(sequenceNumber);
}

void ExamPageDelegate::createImage(const QByteArray& imageData, int seriesId)
{
    qDebug() << "[ExamPageDelegate] createImage() - Series ID:" << seriesId << "Data size:" << imageData.size();

    // Generate DICOM SOP Instance UID
    QString sopInstanceUid = generateDicomUid("image");

    // TODO: Create image record using DicomRepository
    /*
    Image image;
    image.studyId = m_studyId;
    image.seriesId = seriesId;
    image.sopInstanceUid = sopInstanceUid;
    image.instanceNumber = 1;  // Increment for multiple images per series
    image.contentDate = QDate::currentDate();
    image.contentTime = QTime::currentTime();
    image.rows = height;
    image.columns = width;
    image.bitsAllocated = bitsAllocated;
    image.kvp = m_currentTechnique.Kvp;
    // ... more DICOM attributes

    auto result = m_dicomRepo->createImage(image);
    if (result.isSuccess) {
        qDebug() << "[ExamPageDelegate] Image created with ID:" << result.value;

        // Store image pixel data to file system
        QString imageFilePath = QString("path/to/images/%1.dcm").arg(sopInstanceUid);
        // Save imageData to file
    } else {
        displayErrorMessage("Database Error", "Failed to create image: " + result.message);
    }
    */

    qDebug() << "[ExamPageDelegate] Image creation placeholder - SOP UID:" << sopInstanceUid;
}

void ExamPageDelegate::updateWorklistStatus(
    ProcedureStepStatus status,
    const QString& studyInstanceUid)
{
    qDebug() << "[ExamPageDelegate] updateWorklistStatus() - Status:" << static_cast<int>(status);

    if (m_worklistEntry.Id < 0) {
        qWarning() << "[ExamPageDelegate] Cannot update worklist status - invalid entry ID";
        return;
    }

    // Update worklist entry status
    m_worklistEntry.Status = status;

    // TODO: Update StudyInstanceUID in worklist attributes
    // StudyInstanceUID should be added/updated in m_worklistEntry.Attributes list
    if (!studyInstanceUid.isEmpty()) {
        // m_worklistEntry.StudyInstanceUid = studyInstanceUid;  // Field doesn't exist
        // Need to find/update attribute with tag name "StudyInstanceUID"
        qDebug() << "[ExamPageDelegate] Study Instance UID:" << studyInstanceUid;
    }

    auto result = m_worklistRepo->updateWorklistEntry(m_worklistEntry);
    if (result.isSuccess) {
        qDebug() << "[ExamPageDelegate] Worklist status updated successfully";
    } else {
        qWarning() << "[ExamPageDelegate] Failed to update worklist status:" << result.message;
    }
}

int ExamPageDelegate::getOrCreatePatient()
{
    qDebug() << "[ExamPageDelegate] getOrCreatePatient()";

    if (!m_examinationContext) {
        return -1;
    }

    QString patientId = m_examinationContext->patientId();
    QString patientName = m_examinationContext->patientName();

    // TODO: Check if patient exists in database
    // TODO: If not, create patient record
    // TODO: Return patient database ID

    // Placeholder: Return dummy ID
    qDebug() << "[ExamPageDelegate] Patient placeholder - ID:" << patientId << "Name:" << patientName;
    return 1;  // Placeholder patient ID
}

// --- Private Methods: VTK Operations ---

void ExamPageDelegate::initializeVtkViewer()
{
    qDebug() << "[ExamPageDelegate] initializeVtkViewer()";

    // Get VTK widget from ExamPage UI
    m_vtkWidget = ui->getVtkImageViewer();
    if (!m_vtkWidget) {
        qWarning() << "[ExamPageDelegate] VTK widget not found in UI";
        return;
    }

    // Create render window
    m_renderWindow = vtkGenericOpenGLRenderWindow::New();
    m_vtkWidget->setRenderWindow(m_renderWindow);

    // Create renderer
    m_renderer = vtkRenderer::New();
    m_renderer->SetBackground(0.1, 0.1, 0.1);  // Dark gray background
    m_renderWindow->AddRenderer(m_renderer);

    // Create image actor
    m_imageActor = vtkImageActor::New();
    m_renderer->AddActor(m_imageActor);

    // Setup camera
    m_renderer->ResetCamera();

    // Set interactor style for medical imaging (window/level, zoom, pan)
    vtkSmartPointer<vtkInteractorStyleImage> style =
        vtkSmartPointer<vtkInteractorStyleImage>::New();
    m_vtkWidget->interactor()->SetInteractorStyle(style);

    qDebug() << "[ExamPageDelegate] VTK viewer initialized successfully";
}

void ExamPageDelegate::displayImage(vtkImageData* imageData)
{
    qDebug() << "[ExamPageDelegate] displayImage(vtkImageData*)";

    if (!m_imageActor || !imageData) {
        qWarning() << "[ExamPageDelegate] Cannot display image - invalid actor or data";
        return;
    }

    m_imageActor->SetInputData(imageData);
    m_renderer->ResetCamera();
    m_renderWindow->Render();
}

void ExamPageDelegate::displayImage(const QByteArray& rawImageData, int width, int height, int bitsAllocated)
{
    qDebug() << "[ExamPageDelegate] displayImage() - Size:" << width << "x" << height
             << "Bits:" << bitsAllocated;

    // TODO: Convert raw image data to vtkImageData
    // TODO: Apply window/level settings
    // TODO: Display in VTK viewer
}

void ExamPageDelegate::updateThumbnails()
{
    qDebug() << "[ExamPageDelegate] updateThumbnails()";
    // TODO: Update thumbnail list widget with acquired images
}

void ExamPageDelegate::clearVtkViewer()
{
    qDebug() << "[ExamPageDelegate] clearVtkViewer()";

    if (m_renderer) {
        m_renderer->RemoveAllViewProps();
        m_renderWindow->Render();
    }
}

// --- Private Methods: Equipment Control ---

void ExamPageDelegate::configureGenerator()
{
    qDebug() << "[ExamPageDelegate] configureGenerator()";
    // TODO: Send configuration to generator via DeviceRepository
    // Set kVp, mA, exposure time, etc.
}

void ExamPageDelegate::configureDetector()
{
    qDebug() << "[ExamPageDelegate] configureDetector()";
    // TODO: Configure detector via DeviceRepository
    // Set resolution, calibration, etc.
}

void ExamPageDelegate::setTechniqueParameters(const Etrek::ScanProtocol::Data::Entity::TechniqueParameter& params)
{
    qDebug() << "[ExamPageDelegate] setTechniqueParameters()";
    m_currentTechnique = params;

    // TODO: Update UI widgets with parameters
    // ui->getExposureDetailWidget()->load(params);
}

// --- Private Methods: Helper Functions ---

void ExamPageDelegate::setupConnections()
{
    qDebug() << "[ExamPageDelegate] setupConnections()";

    // TODO: Connect UI widget signals to delegate slots
    // Example:
    // connect(ui->getExposureApplicationControlWidget(), &ExposureApplicationControlWidget::readyClicked,
    //         this, &ExamPageDelegate::onReadyButtonClicked);
    // connect(ui->getExposureApplicationControlWidget(), &ExposureApplicationControlWidget::exposeClicked,
    //         this, &ExamPageDelegate::onExposeButtonClicked);
}

void ExamPageDelegate::displayErrorMessage(const QString& title, const QString& message)
{
    qCritical() << "[ExamPageDelegate] Error:" << title << "-" << message;
    QMessageBox::critical(ui, title, message);
    emit errorOccurred(message);
}

void ExamPageDelegate::displayInfoMessage(const QString& title, const QString& message)
{
    qInfo() << "[ExamPageDelegate] Info:" << title << "-" << message;
    QMessageBox::information(ui, title, message);
}

QString ExamPageDelegate::generateDicomUid(const QString& type)
{
    // Generate a DICOM UID
    // Format: <org root>.<timestamp>.<random>
    // For now, use Qt's UUID as placeholder
    QString uid = QUuid::createUuid().toString(QUuid::WithoutBraces);
    uid = uid.replace('-', '.');

    qDebug() << "[ExamPageDelegate] Generated DICOM UID for" << type << ":" << uid;
    return uid;
}

} // namespace Etrek::Application::Delegate
