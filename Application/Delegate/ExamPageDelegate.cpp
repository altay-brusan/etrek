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

// Widget includes
#include "ExamTitleWidget.h"
#include "StudyControlWidget.h"
#include "ExposureControlWidget.h"

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
#include <vtkObject.h>

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
    initializeVtkViewer();

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

    // Set default values (patient size to Normal, application to Diagnostic)
    initializeDefaults();

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

    // Update UI widgets with patient demographics
    if (ui->getExamTitleWidget()) {
        ui->getExamTitleWidget()->setPatientName(patientName);
        ui->getExamTitleWidget()->setPatientId(patientId);
        ui->getExamTitleWidget()->setGender(patientGender);
        ui->getExamTitleWidget()->setAge(age);
        ui->getExamTitleWidget()->setAccessionNumber(accessionNumber);
    } else {
        qWarning() << "[ExamPageDelegate] ExamTitleWidget not available";
    }
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

    qDebug() << "[ExamPageDelegate] Total views loaded:" << m_views.size();

    // Update UI with view list
    if (ui->getStudyControlWidget()) {
        ui->getStudyControlWidget()->setViews(m_views);
    } else {
        qWarning() << "[ExamPageDelegate] StudyControlWidget not available";
    }
}

void ExamPageDelegate::initializeDefaults()
{
    qDebug() << "[ExamPageDelegate] initializeDefaults()";

    // Set default patient size to Normal
    if (ui->getBodySizeWidget()) {
        ui->getBodySizeWidget()->setPatientSize(PatientSize::Normal);
        qDebug() << "[ExamPageDelegate] Set default patient size to Normal";

        // Connect patient size change signal to update technical parameters
        connect(ui->getBodySizeWidget(), &BodySizeWidget::patientSizeChanged,
                this, &ExamPageDelegate::onPatientSizeChanged);
    } else {
        qWarning() << "[ExamPageDelegate] BodySizeWidget not available";
    }

    // Set default application mode to Diagnostic
    if (ui->getExposureApplicationControlWidget()) {
        ui->getExposureApplicationControlWidget()->setApplicationMode(ApplicationMode::Diagnostic);
        qDebug() << "[ExamPageDelegate] Set default application mode to Diagnostic";
    } else {
        qWarning() << "[ExamPageDelegate] ExposureApplicationControlWidget not available";
    }

    // Load technical parameters for default patient size (Normal)
    loadTechnicalParametersForPatientSize(PatientSize::Normal);
}

void ExamPageDelegate::loadTechnicalParametersForPatientSize(PatientSize size)
{
    qDebug() << "[ExamPageDelegate] ========== loadTechnicalParametersForPatientSize START ==========";
    qDebug() << "[ExamPageDelegate] Requested PatientSize:" << static_cast<int>(size);

    // TEMPORARY: Visual confirmation that this method is called
    // Remove this after debugging
    // QMessageBox::information(nullptr, "Debug", QString("Loading parameters for patient size: %1").arg(static_cast<int>(size)));

    if (!ui->getExposureControlWidget()) {
        qWarning() << "[ExamPageDelegate] ExposureControlWidget not available";
        return;
    }

    // Check if we have views loaded
    if (m_views.isEmpty()) {
        qWarning() << "[ExamPageDelegate] No views available, cannot load technique parameters";
        qWarning() << "[ExamPageDelegate] View count:" << m_views.size();
        return;
    }

    // Validate current series index
    if (m_currentSeriesIndex < 0 || m_currentSeriesIndex >= m_views.size()) {
        qWarning() << "[ExamPageDelegate] Invalid series index:" << m_currentSeriesIndex
                   << "View count:" << m_views.size();
        return;
    }

    // Get current view
    const auto& currentView = m_views[m_currentSeriesIndex];
    qDebug() << "[ExamPageDelegate] Current view ID:" << currentView.Id
             << "Name:" << currentView.Name
             << "Body part ID:" << currentView.BodyPart.Id
             << "Body part name:" << currentView.BodyPart.Name;

    // Map PatientSize to BodySize
    Etrek::ScanProtocol::BodySize bodySize = mapPatientSizeToBodySize(size);
    qDebug() << "[ExamPageDelegate] Mapped to BodySize:" << static_cast<int>(bodySize)
             << "(" << (bodySize == Etrek::ScanProtocol::BodySize::Paediatric ? "Paediatric" :
                       bodySize == Etrek::ScanProtocol::BodySize::Thin ? "Thin" :
                       bodySize == Etrek::ScanProtocol::BodySize::Medium ? "Medium" : "Fat") << ")";

    // Get view techniques for the current view
    auto viewTechniquesResult = m_scanProtocolRepo->getViewTechniques(currentView.Id);
    if (!viewTechniquesResult.isSuccess) {
        qWarning() << "[ExamPageDelegate] Failed to get view techniques:" << viewTechniquesResult.message;
        return;
    }

    qDebug() << "[ExamPageDelegate] Found" << viewTechniquesResult.value.size() << "view techniques for view ID:" << currentView.Id;

    // Check if view has any techniques defined
    if (viewTechniquesResult.value.isEmpty()) {
        qWarning() << "[ExamPageDelegate] ========== NO VIEW TECHNIQUES ==========";
        qWarning() << "[ExamPageDelegate] View" << currentView.Name << "(ID:" << currentView.Id << ") has no technique parameters linked!";
        qWarning() << "[ExamPageDelegate] Please configure technique parameters for this view in the database.";
        qWarning() << "[ExamPageDelegate] Table: view_techniques should have entries linking this view to technique_parameters.";
        return;
    }

    // Get all technique parameters
    auto allParamsResult = m_scanProtocolRepo->getAllTechniqueParameters();
    if (!allParamsResult.isSuccess) {
        qWarning() << "[ExamPageDelegate] Failed to get technique parameters:" << allParamsResult.message;
        return;
    }

    qDebug() << "[ExamPageDelegate] Total technique parameters in database:" << allParamsResult.value.size();

    // Find the technique parameter that matches our view, body part, and body size
    const auto& viewTechniques = viewTechniquesResult.value;
    const auto& allParams = allParamsResult.value;

    // Log all view techniques for debugging
    qDebug() << "[ExamPageDelegate] View techniques details:";
    for (const auto& vt : viewTechniques) {
        qDebug() << "  - ViewTechnique: view_id=" << vt.view_id
                 << "technique_parameter_id=" << vt.technique_parameter_id
                 << "seq=" << vt.seq
                 << "IsActive=" << vt.IsActive;
    }

    Etrek::ScanProtocol::Data::Entity::TechniqueParameter* matchedParam = nullptr;

    // First, find technique parameter IDs from view techniques
    qDebug() << "[ExamPageDelegate] Searching for matching technique parameter...";
    qDebug() << "[ExamPageDelegate] Looking for: BodySize=" << static_cast<int>(bodySize)
             << "BodyPartId=" << currentView.BodyPart.Id;

    for (const auto& vt : viewTechniques) {
        if (!vt.IsActive) {
            qDebug() << "  - Skipping inactive view technique, param_id=" << vt.technique_parameter_id;
            continue;
        }

        qDebug() << "  - Checking technique_parameter_id=" << vt.technique_parameter_id;

        // Find the corresponding technique parameter
        for (const auto& param : allParams) {
            if (param.Id == vt.technique_parameter_id) {
                qDebug() << "    Found param ID:" << param.Id
                         << "Size:" << static_cast<int>(param.Size)
                         << "BodyPartId:" << param.BodyPart.Id
                         << "kVp:" << param.Kvp << "mA:" << param.Ma << "Ms:" << param.Ms;

                if (param.Size == bodySize && param.BodyPart.Id == currentView.BodyPart.Id) {
                    qDebug() << "    ✓ MATCH FOUND!";
                    matchedParam = const_cast<Etrek::ScanProtocol::Data::Entity::TechniqueParameter*>(&param);
                    break;
                } else {
                    qDebug() << "    ✗ No match - Size:" << (param.Size == bodySize ? "OK" : "MISMATCH")
                             << "BodyPart:" << (param.BodyPart.Id == currentView.BodyPart.Id ? "OK" : "MISMATCH");
                }
            }
        }

        if (matchedParam) break;
    }

    if (!matchedParam) {
        qWarning() << "[ExamPageDelegate] ========== NO MATCH FOUND ==========";
        qWarning() << "[ExamPageDelegate] No technique parameter found for:";
        qWarning() << "  - Body size:" << static_cast<int>(bodySize);
        qWarning() << "  - Body part ID:" << currentView.BodyPart.Id;
        qWarning() << "  - Body part name:" << currentView.BodyPart.Name;
        qWarning() << "[ExamPageDelegate] Please check your database to ensure technique parameters exist for this combination";

        // List all available technique parameters for this view's body part
        qDebug() << "[ExamPageDelegate] Available technique parameters for body part" << currentView.BodyPart.Name << ":";
        for (const auto& param : allParams) {
            if (param.BodyPart.Id == currentView.BodyPart.Id) {
                qDebug() << "  - Param ID:" << param.Id
                         << "Size:" << static_cast<int>(param.Size)
                         << "kVp:" << param.Kvp << "mA:" << param.Ma;
            }
        }

        return;
    }

    // Apply the matched parameters to the UI
    int calculatedMas = matchedParam->Ma * matchedParam->Ms / 1000;

    qDebug() << "[ExamPageDelegate] ========== APPLYING PARAMETERS ==========";
    qDebug() << "[ExamPageDelegate] Setting exposure parameters:";
    qDebug() << "  - kVp:" << matchedParam->Kvp;
    qDebug() << "  - mA:" << matchedParam->Ma;
    qDebug() << "  - Time (Ms):" << matchedParam->Ms;
    qDebug() << "  - mAs (calculated):" << calculatedMas;

    ui->getExposureControlWidget()->setExposureParameters(
        matchedParam->Kvp,
        matchedParam->Ma,
        matchedParam->Ms,
        calculatedMas
    );

    qDebug() << "[ExamPageDelegate] ✓ Parameters successfully applied to ExposureControlWidget";
    qDebug() << "[ExamPageDelegate] ========== loadTechnicalParametersForPatientSize END ==========";
}

Etrek::ScanProtocol::BodySize ExamPageDelegate::mapPatientSizeToBodySize(PatientSize patientSize) const
{
    switch (patientSize) {
        case PatientSize::Infant:
            return Etrek::ScanProtocol::BodySize::Paediatric;
        case PatientSize::Small:
            return Etrek::ScanProtocol::BodySize::Thin;
        case PatientSize::Normal:
            return Etrek::ScanProtocol::BodySize::Medium;
        case PatientSize::Large:
            return Etrek::ScanProtocol::BodySize::Fat;
        default:
            return Etrek::ScanProtocol::BodySize::Medium;
    }
}

void ExamPageDelegate::onPatientSizeChanged(PatientSize size)
{
    qDebug() << "[ExamPageDelegate] onPatientSizeChanged() - New size:" << static_cast<int>(size);

    // Reload technical parameters for the new patient size
    loadTechnicalParametersForPatientSize(size);
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

    // Build metadata for image viewer
    Etrek::ImageViewer::DicomMetadata metadata;
    // Extract patient info from worklist attributes
    for (const auto& attr : m_worklistEntry.Attributes) {
        if (attr.Tag.Name == "PatientName") {
            metadata.patientName = attr.TagValue;
        } else if (attr.Tag.Name == "PatientID") {
            metadata.patientId = attr.TagValue;
        }
    }
    metadata.studyDate = QDateTime::currentDateTime().date().toString("yyyyMMdd");
    metadata.studyTime = QDateTime::currentDateTime().time().toString("HHmmss");
    metadata.modality = "DX";  // Digital X-Ray
    if (m_currentSeriesIndex < m_views.size()) {
        metadata.seriesDescription = m_views[m_currentSeriesIndex].Name;
    }
    metadata.imageNumber = 1;
    metadata.totalImages = 1;
    // Window/Level and dimensions would be set based on actual image data

    // Emit signal to open image viewer with the acquired image
    emit openImageViewer(imageData, metadata);

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

void ExamPageDelegate::onDeleteViewRequested(int viewIndex)
{
    qDebug() << "[ExamPageDelegate] onDeleteViewRequested() - viewIndex:" << viewIndex;

    // Validate view index
    if (viewIndex < 0 || viewIndex >= m_views.size()) {
        qWarning() << "[ExamPageDelegate] Invalid view index:" << viewIndex;
        return;
    }

    // Check if this is the last view
    if (m_views.size() == 1) {
        // Show confirmation for cancelling the study
        QMessageBox::StandardButton reply = QMessageBox::question(
            ui,
            "Cancel Study",
            "Study has no more views. Do you want to cancel study?",
            QMessageBox::Yes | QMessageBox::No
        );

        if (reply == QMessageBox::Yes) {
            // User confirmed - cancel the entire study
            onCancelStudyRequested();
        }
        // If No, do nothing - keep the last view
        return;
    }

    // Not the last view - delete it
    QString viewName = m_views[viewIndex].Name;

    // Remove view from the list
    m_views.remove(viewIndex);

    // Update the UI to reflect the change
    if (ui->getStudyControlWidget()) {
        ui->getStudyControlWidget()->setViews(m_views);
    }

    // Remove corresponding series ID if it exists
    if (viewIndex < m_seriesIds.size()) {
        m_seriesIds.remove(viewIndex);
    }

    qDebug() << "[ExamPageDelegate] Deleted view:" << viewName << "- Remaining views:" << m_views.size();
}

void ExamPageDelegate::onCancelStudyRequested()
{
    qDebug() << "[ExamPageDelegate] onCancelStudyRequested()";

    // Show confirmation dialog
    QMessageBox::StandardButton reply = QMessageBox::question(
        ui,
        "Cancel Study",
        "Are you sure you want to cancel this study?\nAll views will be cancelled and you will return to the worklist.",
        QMessageBox::Yes | QMessageBox::No
    );

    if (reply == QMessageBox::Yes) {
        // Update worklist status to CANCELLED
        updateWorklistStatus(ProcedureStepStatus::CANCELLED);

        qDebug() << "[ExamPageDelegate] Study cancelled by user";

        // Close examination page
        emit closeExamination();
    }
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
    qDebug() << "[ExamPageDelegate] updateWorklistStatus() - Status:"
             << ProcedureStepStatusToString(status)
             << "for entry ID:" << m_worklistEntry.Id;

    if (m_worklistEntry.Id < 0) {
        qWarning() << "[ExamPageDelegate] Cannot update worklist status - invalid entry ID";
        return;
    }

    // Update local worklist entry status
    m_worklistEntry.Status = status;

    // Update status in database using repository
    auto result = m_worklistRepo->updateWorklistStatus(m_worklistEntry.Id, status);
    if (result.isSuccess) {
        qDebug() << "[ExamPageDelegate] Worklist status updated successfully in database to:"
                 << ProcedureStepStatusToString(status);
    } else {
        qWarning() << "[ExamPageDelegate] Failed to update worklist status in database:" << result.message;
    }

    // TODO: Update StudyInstanceUID in worklist attributes
    if (!studyInstanceUid.isEmpty()) {
        qDebug() << "[ExamPageDelegate] Study Instance UID:" << studyInstanceUid;
        // Need to update the StudyInstanceUID attribute in the database
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

    // Disable VTK output window (suppress VTK warnings/errors popups)
    vtkObject::GlobalWarningDisplayOff();

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

    // Connect StudyControlWidget signals
    if (ui->getStudyControlWidget()) {
        connect(ui->getStudyControlWidget(), &StudyControlWidget::deleteViewRequested,
                this, &ExamPageDelegate::onDeleteViewRequested);
        connect(ui->getStudyControlWidget(), &StudyControlWidget::cancelStudyRequested,
                this, &ExamPageDelegate::onCancelStudyRequested);
    }

    // TODO: Connect other UI widget signals to delegate slots
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
