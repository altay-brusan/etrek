#ifndef IMAGEVIEWERPAGEDELEGATE_H
#define IMAGEVIEWERPAGEDELEGATE_H

#include <QObject>
#include <QPointer>
#include <memory>
#include <array>

#include "IDelegate.h"
#include "IPageAction.h"
#include "ImageViewerTypes.h"
#include "ViewportLayout.h"

// Forward declarations
class ImageViewerPage;

namespace Etrek::ImageViewer::Rendering {
class VtkViewportRenderer;
class MultiViewportManager;
}

namespace Etrek::ImageViewer::Service {
class ImageLoaderService;
}

namespace Etrek::ImageViewer::Tool {
class WindowLevelTool;
class ZoomTool;
class PanTool;
class RulerTool;
class AngleTool;
class ResetTool;
}

namespace Etrek::ImageViewer::Widget {
class MagnifierWidget;
}

namespace Etrek::Dicom::Repository {
class DicomRepository;
}

namespace Etrek::Context {
class IContextManager;
}

namespace Etrek::Core::Data::Model {
class DatabaseConnectionSetting;
}

namespace Etrek::Application::Delegate {

/**
 * @class ImageViewerPageDelegate
 * @brief Delegate for ImageViewerPage following MVD pattern.
 *
 * Responsibilities:
 * - Manage VTK viewport rendering
 * - Handle tool interactions
 * - Load images from file, database, or ExamPage
 * - Update DICOM overlays
 * - Manage viewport layouts
 */
class ImageViewerPageDelegate : public QObject, public IDelegate, public IPageAction
{
    Q_OBJECT
    Q_INTERFACES(IDelegate IPageAction)

public:
    explicit ImageViewerPageDelegate(
        ImageViewerPage* ui,
        std::shared_ptr<Etrek::Dicom::Repository::DicomRepository> dicomRepo,
        std::shared_ptr<Etrek::Core::Data::Model::DatabaseConnectionSetting> dbConnection,
        std::weak_ptr<Etrek::Context::IContextManager> contextManager,
        QObject* parent = nullptr
    );

    ~ImageViewerPageDelegate() override;

    // --- IDelegate Implementation ---
    QString name() const override { return "ImageViewerPageDelegate"; }
    void attachDelegates(const QVector<QObject*>& delegates) override;

    // --- IPageAction Implementation ---
    void apply() override;
    void accept() override;
    void reject() override;

    // --- Image Loading ---
    void loadFromFile(const QString& filePath);
    void loadFromFiles(const QStringList& filePaths);
    void loadFromStudy(int studyId);
    void loadFromSeries(int seriesId);
    void loadFromExamPage(const QByteArray& imageData, const Etrek::ImageViewer::DicomMetadata& metadata);

signals:
    void imageLoaded(int viewportIndex);
    void errorOccurred(const QString& message);
    void closeRequested();

private slots:
    // Initialization
    void onPageLoaded();

    // Tool selection
    void onToolSelected(Etrek::ImageViewer::ToolType tool);

    // Layout changes
    void onLayoutSelected(Etrek::ImageViewer::ViewportLayout layout);

    // Viewport interactions
    void onViewportMousePressed(int viewportIndex, const QPointF& pos, Qt::MouseButton button, Qt::KeyboardModifiers modifiers);
    void onViewportMouseMoved(int viewportIndex, const QPointF& pos, Qt::MouseButtons buttons, Qt::KeyboardModifiers modifiers);
    void onViewportMouseReleased(int viewportIndex, const QPointF& pos, Qt::MouseButton button, Qt::KeyboardModifiers modifiers);
    void onViewportMouseWheel(int viewportIndex, int delta, const QPointF& pos, Qt::KeyboardModifiers modifiers);
    void onViewportDoubleClicked(int viewportIndex, const QPointF& pos, Qt::MouseButton button);
    void onActiveViewportChanged(int index);

    // Actions
    void onOpenFileRequested();
    void onInvertRequested();
    void onResetViewRequested();
    void onFitToWindowRequested();

    // Drag and drop
    void onFilesDropped(const QStringList& filePaths);

    // Thumbnails
    void onThumbnailClicked(int id, bool isSeries);

    // Tool signals
    void onWindowLevelChanged(double window, double level);
    void onZoomRequested(double factor, const QPointF& centerPos);
    void onPanRequested(double deltaX, double deltaY);
    void onResetRequested();
    void onMeasurementLineUpdated(const Etrek::ImageViewer::MeasurementLine& line);
    void onMeasurementLineCompleted(const Etrek::ImageViewer::MeasurementLine& line);
    void onCursorChanged(Qt::CursorShape cursor);

    // Magnifier slots
    void onMagnifierRequested(const QPointF& pos);
    void onMagnifierHideRequested();
    void onMagnifierToggleRequested();

private:
    void initializeViewports();
    void initializeTools();
    void setupConnections();
    void updateOverlay(int viewportIndex);
    void updateAllOverlays();
    void activateTool(Etrek::ImageViewer::ToolType tool);
    void loadImageIntoViewport(int viewportIndex, const QString& filePath);
    QString formatOverlayText(Etrek::ImageViewer::OverlayCorner corner, int viewportIndex) const;

    // UI
    ImageViewerPage* m_ui;

    // Repositories
    std::shared_ptr<Etrek::Dicom::Repository::DicomRepository> m_dicomRepo;
    std::shared_ptr<Etrek::Core::Data::Model::DatabaseConnectionSetting> m_dbConnection;
    std::weak_ptr<Etrek::Context::IContextManager> m_contextManager;

    // Services
    std::unique_ptr<Etrek::ImageViewer::Service::ImageLoaderService> m_imageLoader;

    // Rendering
    std::unique_ptr<Etrek::ImageViewer::Rendering::MultiViewportManager> m_viewportManager;

    // Tools
    std::unique_ptr<Etrek::ImageViewer::Tool::WindowLevelTool> m_windowLevelTool;
    std::unique_ptr<Etrek::ImageViewer::Tool::ZoomTool> m_zoomTool;
    std::unique_ptr<Etrek::ImageViewer::Tool::PanTool> m_panTool;
    std::unique_ptr<Etrek::ImageViewer::Tool::RulerTool> m_rulerTool;
    std::unique_ptr<Etrek::ImageViewer::Tool::AngleTool> m_angleTool;
    std::unique_ptr<Etrek::ImageViewer::Tool::ResetTool> m_resetTool;

    // Magnifier widget
    Etrek::ImageViewer::Widget::MagnifierWidget* m_magnifier = nullptr;

    // State
    Etrek::ImageViewer::ToolType m_currentTool = Etrek::ImageViewer::ToolType::WINDOW_LEVEL;
    Etrek::ImageViewer::ViewportLayout m_currentLayout = Etrek::ImageViewer::ViewportLayout::SINGLE;
    int m_activeViewportIndex = 0;

    // Metadata per viewport
    std::array<Etrek::ImageViewer::DicomMetadata, 4> m_viewportMetadata;
    std::array<Etrek::ImageViewer::ViewportState, 4> m_viewportStates;
};

} // namespace Etrek::Application::Delegate

#endif // IMAGEVIEWERPAGEDELEGATE_H
