#ifndef EXAMPAGEDELEGATE_H
#define EXAMPAGEDELEGATE_H

#include <QObject>
#include <QPointer>
#include <memory>
#include <QVector>

#include "IDelegate.h"
#include "IPageAction.h"
#include "ExamPage.h"
#include "WorklistEntry.h"
#include "ImageViewerTypes.h"
#include "WorklistEnum.h"
#include "View.h"
#include "TechniqueParameter.h"
#include "Procedure.h"
#include "BodySizeWidget.h"
#include "ExposureApplicationControlWidget.h"
#include "Examination/ExaminationMode.h"
#include "Examination/IExaminationModeStrategy.h"

// Forward declarations for VTK
class vtkRenderer;
class vtkGenericOpenGLRenderWindow;
class vtkImageActor;
class vtkImageData;
class vtkCamera;
class QVTKOpenGLNativeWidget;

// Forward declarations for ImageViewer tools
namespace Etrek::ImageViewer::Tool {
    class WindowLevelTool;
    class ZoomTool;
    class PanTool;
    class RulerTool;
    class AngleTool;
    class ResetTool;
}

namespace Etrek::ImageViewer::Widget {
    class RulerOverlayWidget;
    class AngleOverlayWidget;
}

namespace Etrek::Worklist::Repository {
    class WorklistRepository;
}

namespace Etrek::Dicom::Repository {
    class DicomRepository;
    class MwlTaskMappingRepository;
}

namespace Etrek::ScanProtocol::Repository {
    class ScanProtocolRepository;
}

namespace Etrek::Device::Repository {
    class DeviceRepository;
}

namespace Etrek::Context {
    class IContextManager;
}

namespace Etrek::Core::Context {
    class ExaminationContext;
}

namespace Etrek::Core::Data::Model {
    class DatabaseConnectionSetting;
}

namespace Etrek::Application::Delegate {

/**
 * @class ExamPageDelegate
 * @brief Delegate for ExamPage following MVD pattern.
 *
 * Responsibilities:
 * - Load examination context (worklist entry, patient demographics)
 * - Create study and series records in database
 * - Load views and technique parameters
 * - Manage VTK image viewer
 * - Handle image acquisition workflow
 * - Update worklist status
 */
class ExamPageDelegate : public QObject, public IDelegate, public IPageAction
{
    Q_OBJECT
    Q_INTERFACES(IDelegate IPageAction)

public:
    explicit ExamPageDelegate(
        ExamPage* ui,
        std::shared_ptr<Etrek::Worklist::Repository::WorklistRepository> worklistRepo,
        std::shared_ptr<Etrek::Dicom::Repository::DicomRepository> dicomRepo,
        std::shared_ptr<Etrek::ScanProtocol::Repository::ScanProtocolRepository> scanProtocolRepo,
        std::shared_ptr<Etrek::Device::Repository::DeviceRepository> deviceRepo,
        std::shared_ptr<Etrek::Dicom::Repository::MwlTaskMappingRepository> mwlTaskMappingRepo,
        std::shared_ptr<Etrek::Core::Data::Model::DatabaseConnectionSetting> dbConnection,
        std::weak_ptr<Etrek::Context::IContextManager> contextManager,
        QObject* parent = nullptr
    );

    ~ExamPageDelegate() override;

    // --- IDelegate Implementation ---
    QString name() const override { return "ExamPageDelegate"; }
    void attachDelegates(const QVector<QObject*>& delegates) override;

    // --- IPageAction Implementation ---
    void apply() override;   // Save intermediate state
    void accept() override;  // Complete examination
    void reject() override;  // Cancel examination

signals:
    /**
     * @brief Emitted when examination is completed successfully.
     * @param studyId The ID of the created study.
     */
    void examinationCompleted(int studyId);

    /**
     * @brief Emitted when user requests to close examination page.
     */
    void closeExamination();

    /**
     * @brief Emitted when an error occurs.
     * @param message Error message.
     */
    void errorOccurred(const QString& message);

    /**
     * @brief Emitted when an image is acquired and should be opened in the image viewer.
     * @param imageData Raw image pixel data.
     * @param metadata DICOM metadata for the acquired image.
     */
    void openImageViewer(const QByteArray& imageData, const Etrek::ImageViewer::DicomMetadata& metadata);

private slots:
    // --- Initialization ---
    void onPageLoaded();
    void loadExaminationContext();
    void loadPatientDemographics();
    void loadViews();
    void initializeDefaults();
    void loadTechnicalParametersForPatientSize(PatientSize size);
    void loadTechniqueParameters();

    // --- View/Series Management ---
    void onViewSelected(int seriesId);
    void onNextView();
    void onPreviousView();

    // --- Image Acquisition ---
    void onReadyButtonClicked();
    void onExposeButtonClicked();
    void onImageReceived(const QByteArray& imageData);
    void onAcquisitionComplete();

    // --- Technical Parameter Changes ---
    void onPatientSizeChanged(PatientSize size);
    void onKvpChanged(int kvp);
    void onMaChanged(int ma);
    void onMasChanged(int mas);
    void onTimeChanged(int time);
    void onDensityChanged(int density);

    // --- Study/Series Operations ---
    void onNewStudyClicked();
    void onEditStudyClicked();
    void onNewViewClicked();
    void onDeleteViewRequested(int viewIndex);
    void onCancelStudyRequested();

    // --- Action Buttons ---
    void onCloseClicked();
    void onPrintClicked();

    // --- Image Tool Panel ---
    void onToolSelected(Etrek::ImageViewer::ToolType tool);
    void onInvertRequested();
    void onFitToWindowRequested();
    void onResetViewRequested();
    void onCursorChanged(Qt::CursorShape cursor);

    // --- Tool Event Handlers ---
    void onWindowLevelChanged(double window, double level);
    void onZoomRequested(double factor, const QPointF& center);
    void onPanRequested(double deltaX, double deltaY);

    // --- Measurement Handlers ---
    void onRulerMeasurementUpdated(const Etrek::ImageViewer::MeasurementLine& line);
    void onRulerMeasurementCompleted(const Etrek::ImageViewer::MeasurementLine& line);
    void onAngleMeasurementUpdated(const Etrek::ImageViewer::MeasurementAngle& angle);
    void onAngleMeasurementCompleted(const Etrek::ImageViewer::MeasurementAngle& angle);

protected:
    bool eventFilter(QObject* watched, QEvent* event) override;

private:
    // --- Database Operations ---
    void createStudy();
    void createSeriesForViews();
    void createSeries(const Etrek::ScanProtocol::Data::Entity::View& view, int sequenceNumber);
    void createImage(const QByteArray& imageData, int seriesId);
    void updateWorklistStatus(ProcedureStepStatus status,
                             const QString& studyInstanceUid = QString());
    int getOrCreatePatient();

    // --- VTK Operations ---
    void initializeVtkViewer();
    void displayImage(vtkImageData* imageData);
    void displayImage(const QByteArray& rawImageData, int width, int height, int bitsAllocated);
    void updateThumbnails();
    void clearVtkViewer();

    // --- Equipment Control ---
    void configureGenerator();
    void configureDetector();
    void setTechniqueParameters(const Etrek::ScanProtocol::Data::Entity::TechniqueParameter& params);

    // --- Helper Methods ---
    Etrek::ScanProtocol::BodySize mapPatientSizeToBodySize(PatientSize patientSize) const;
    void setupConnections();
    void displayErrorMessage(const QString& title, const QString& message);
    void displayInfoMessage(const QString& title, const QString& message);
    QString generateDicomUid(const QString& type);

    // --- Tool Methods ---
    void initializeTools();
    void activateTool(Etrek::ImageViewer::ToolType tool);
    void initializeOverlays();
    void updateOverlayTransforms();
    QPointF imageToWidget(const QPointF& imagePos) const;

    // --- UI Components ---
    ExamPage* ui;

    // --- Repositories ---
    std::shared_ptr<Etrek::Worklist::Repository::WorklistRepository> m_worklistRepo;
    std::shared_ptr<Etrek::Dicom::Repository::DicomRepository> m_dicomRepo;
    std::shared_ptr<Etrek::ScanProtocol::Repository::ScanProtocolRepository> m_scanProtocolRepo;
    std::shared_ptr<Etrek::Device::Repository::DeviceRepository> m_deviceRepo;
    std::shared_ptr<Etrek::Dicom::Repository::MwlTaskMappingRepository> m_mwlTaskMappingRepo;
    std::shared_ptr<Etrek::Core::Data::Model::DatabaseConnectionSetting> m_dbConnection;

    // --- Context ---
    std::weak_ptr<Etrek::Context::IContextManager> m_contextManager;
    std::shared_ptr<Etrek::Core::Context::ExaminationContext> m_examinationContext;

    // --- Current Examination State ---
    Etrek::Worklist::Data::Entity::WorklistEntry m_worklistEntry;
    int m_studyId = -1;
    int m_patientId = -1;
    QVector<int> m_seriesIds;
    int m_currentSeriesIndex = 0;
    QVector<Etrek::ScanProtocol::Data::Entity::View> m_views;
    Etrek::ScanProtocol::Data::Entity::Procedure m_procedure;
    Etrek::ScanProtocol::Data::Entity::TechniqueParameter m_currentTechnique;

    // --- VTK Members ---
    QVTKOpenGLNativeWidget* m_vtkWidget = nullptr;
    vtkRenderer* m_renderer = nullptr;
    vtkGenericOpenGLRenderWindow* m_renderWindow = nullptr;
    vtkImageActor* m_imageActor = nullptr;

    // --- Image Tools ---
    std::unique_ptr<Etrek::ImageViewer::Tool::WindowLevelTool> m_windowLevelTool;
    std::unique_ptr<Etrek::ImageViewer::Tool::ZoomTool> m_zoomTool;
    std::unique_ptr<Etrek::ImageViewer::Tool::PanTool> m_panTool;
    std::unique_ptr<Etrek::ImageViewer::Tool::RulerTool> m_rulerTool;
    std::unique_ptr<Etrek::ImageViewer::Tool::AngleTool> m_angleTool;
    std::unique_ptr<Etrek::ImageViewer::Tool::ResetTool> m_resetTool;
    Etrek::ImageViewer::ToolType m_currentTool = Etrek::ImageViewer::ToolType::WINDOW_LEVEL;

    // --- Measurement Overlays ---
    Etrek::ImageViewer::Widget::RulerOverlayWidget* m_rulerOverlay = nullptr;
    Etrek::ImageViewer::Widget::AngleOverlayWidget* m_angleOverlay = nullptr;

    // --- Image State ---
    double m_currentWindow = 400.0;
    double m_currentLevel = 40.0;
    double m_zoomLevel = 1.0;
    QPointF m_panOffset{0.0, 0.0};

    // --- Other Delegates ---
    QVector<QPointer<QObject>> m_childDelegates;

    // --- Examination Mode Strategy ---
    std::unique_ptr<Etrek::Examination::IExaminationModeStrategy> m_examinationStrategy;
};

} // namespace Etrek::Application::Delegate

#endif // EXAMPAGEDELEGATE_H
