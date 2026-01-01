#include "ImageViewerPageDelegate.h"
#include "ImageViewerPage.h"
#include "ViewportGrid.h"
#include "ImageToolPanel.h"
#include "SeriesThumbnailPanel.h"

#include "VtkViewportRenderer.h"
#include "MultiViewportManager.h"
#include "ImageLoaderService.h"

#include "WindowLevelTool.h"
#include "ZoomTool.h"
#include "PanTool.h"
#include "RulerTool.h"
#include "AngleTool.h"
#include "ResetTool.h"

#include <QTimer>
#include <QFileDialog>
#include <QMessageBox>
#include <QVTKOpenGLNativeWidget.h>

using namespace Etrek::ImageViewer;
using namespace Etrek::ImageViewer::Rendering;
using namespace Etrek::ImageViewer::Service;
using namespace Etrek::ImageViewer::Tool;

namespace Etrek::Application::Delegate {

ImageViewerPageDelegate::ImageViewerPageDelegate(
    ImageViewerPage* ui,
    std::shared_ptr<Etrek::Dicom::Repository::DicomRepository> dicomRepo,
    std::shared_ptr<Etrek::Core::Data::Model::DatabaseConnectionSetting> dbConnection,
    std::weak_ptr<Etrek::Context::IContextManager> contextManager,
    QObject* parent)
    : QObject(parent)
    , m_ui(ui)
    , m_dicomRepo(dicomRepo)
    , m_dbConnection(dbConnection)
    , m_contextManager(contextManager)
    , m_imageLoader(std::make_unique<ImageLoaderService>())
    , m_viewportManager(std::make_unique<MultiViewportManager>(this))
{
    setupConnections();
    initializeTools();

    // Deferred initialization
    QTimer::singleShot(0, this, &ImageViewerPageDelegate::onPageLoaded);
}

ImageViewerPageDelegate::~ImageViewerPageDelegate() = default;

void ImageViewerPageDelegate::attachDelegates(const QVector<QObject*>& delegates) {
    Q_UNUSED(delegates)
    // No child delegates needed for image viewer
}

void ImageViewerPageDelegate::apply() {
    // Save intermediate state if needed
}

void ImageViewerPageDelegate::accept() {
    // Complete viewing session
    emit closeRequested();
}

void ImageViewerPageDelegate::reject() {
    // Cancel viewing session
    emit closeRequested();
}

void ImageViewerPageDelegate::onPageLoaded() {
    initializeViewports();

    // Check if there's context data to load
    if (auto ctx = m_contextManager.lock()) {
        // TODO: Load from context if available
    }
}

void ImageViewerPageDelegate::initializeViewports() {
    auto widgets = m_ui->getAllVtkWidgets();
    m_viewportManager->initialize(widgets);
}

void ImageViewerPageDelegate::initializeTools() {
    m_windowLevelTool = std::make_unique<WindowLevelTool>(this);
    m_zoomTool = std::make_unique<ZoomTool>(this);
    m_panTool = std::make_unique<PanTool>(this);
    m_rulerTool = std::make_unique<RulerTool>(this);
    m_angleTool = std::make_unique<AngleTool>(this);
    m_resetTool = std::make_unique<ResetTool>(this);

    // Connect tool signals
    connect(m_windowLevelTool.get(), &WindowLevelTool::windowLevelChanged,
            this, &ImageViewerPageDelegate::onWindowLevelChanged);
    connect(m_zoomTool.get(), &ZoomTool::zoomRequested,
            this, &ImageViewerPageDelegate::onZoomRequested);
    connect(m_panTool.get(), &PanTool::panRequested,
            this, &ImageViewerPageDelegate::onPanRequested);
    connect(m_rulerTool.get(), &RulerTool::measurementLineUpdated,
            this, &ImageViewerPageDelegate::onMeasurementLineUpdated);
    connect(m_rulerTool.get(), &RulerTool::measurementLineCompleted,
            this, &ImageViewerPageDelegate::onMeasurementLineCompleted);
    connect(m_resetTool.get(), &ResetTool::resetRequested,
            this, &ImageViewerPageDelegate::onResetRequested);

    // Activate default tool
    activateTool(ToolType::WINDOW_LEVEL);
}

void ImageViewerPageDelegate::setupConnections() {
    // Tool selection
    connect(m_ui, &ImageViewerPage::toolSelected,
            this, &ImageViewerPageDelegate::onToolSelected);

    // Layout
    connect(m_ui, &ImageViewerPage::layoutSelected,
            this, &ImageViewerPageDelegate::onLayoutSelected);

    // Viewport interactions
    connect(m_ui, &ImageViewerPage::viewportMousePressed,
            this, &ImageViewerPageDelegate::onViewportMousePressed);
    connect(m_ui, &ImageViewerPage::viewportMouseMoved,
            this, &ImageViewerPageDelegate::onViewportMouseMoved);
    connect(m_ui, &ImageViewerPage::viewportMouseReleased,
            this, &ImageViewerPageDelegate::onViewportMouseReleased);
    connect(m_ui, &ImageViewerPage::viewportMouseWheel,
            this, &ImageViewerPageDelegate::onViewportMouseWheel);
    connect(m_ui, &ImageViewerPage::viewportDoubleClicked,
            this, &ImageViewerPageDelegate::onViewportDoubleClicked);
    connect(m_ui, &ImageViewerPage::activeViewportChanged,
            this, &ImageViewerPageDelegate::onActiveViewportChanged);

    // Actions
    connect(m_ui, &ImageViewerPage::openFileRequested,
            this, &ImageViewerPageDelegate::onOpenFileRequested);
    connect(m_ui, &ImageViewerPage::invertRequested,
            this, &ImageViewerPageDelegate::onInvertRequested);
    connect(m_ui, &ImageViewerPage::resetViewRequested,
            this, &ImageViewerPageDelegate::onResetViewRequested);
    connect(m_ui, &ImageViewerPage::fitToWindowRequested,
            this, &ImageViewerPageDelegate::onFitToWindowRequested);

    // Drag and drop
    connect(m_ui, &ImageViewerPage::filesDropped,
            this, &ImageViewerPageDelegate::onFilesDropped);

    // Thumbnails
    connect(m_ui, &ImageViewerPage::thumbnailClicked,
            this, &ImageViewerPageDelegate::onThumbnailClicked);

    // Close request - user wants to close ImageViewer and return to main window
    connect(m_ui, &ImageViewerPage::closeRequested,
            this, &ImageViewerPageDelegate::reject);
}

void ImageViewerPageDelegate::loadFromFile(const QString& filePath) {
    loadImageIntoViewport(m_activeViewportIndex, filePath);
}

void ImageViewerPageDelegate::loadFromFiles(const QStringList& filePaths) {
    int viewportIndex = m_activeViewportIndex;
    int maxViewports = m_viewportManager->visibleViewportCount();

    for (const QString& path : filePaths) {
        if (viewportIndex >= maxViewports) break;
        loadImageIntoViewport(viewportIndex, path);
        ++viewportIndex;
    }
}

void ImageViewerPageDelegate::loadFromStudy(int studyId) {
    Q_UNUSED(studyId)
    // TODO: Load series list from database and populate thumbnails
}

void ImageViewerPageDelegate::loadFromSeries(int seriesId) {
    Q_UNUSED(seriesId)
    // TODO: Load images from series
}

void ImageViewerPageDelegate::loadFromExamPage(const QByteArray& imageData, const DicomMetadata& metadata) {
    auto result = m_imageLoader->loadFromBytes(imageData, ImageFormat::DICOM);

    if (result.success) {
        auto* renderer = m_viewportManager->getRenderer(m_activeViewportIndex);
        if (renderer) {
            renderer->setImageData(result.imageData, true);
            m_viewportMetadata[m_activeViewportIndex] = metadata;
            updateOverlay(m_activeViewportIndex);
            emit imageLoaded(m_activeViewportIndex);
        }
    } else {
        emit errorOccurred(result.errorMessage);
    }
}

void ImageViewerPageDelegate::loadImageIntoViewport(int viewportIndex, const QString& filePath) {
    auto result = m_imageLoader->loadFromFile(filePath);

    if (result.success) {
        auto* renderer = m_viewportManager->getRenderer(viewportIndex);
        if (renderer) {
            renderer->setImageData(result.imageData, true);
            m_viewportMetadata[viewportIndex] = result.metadata;

            // Update tool with pixel spacing
            if (m_rulerTool) {
                m_rulerTool->setPixelSpacing(result.metadata.pixelSpacingX, result.metadata.pixelSpacingY);
            }

            // Update window/level tool with current values
            if (m_windowLevelTool) {
                double window, level;
                renderer->getWindowLevel(window, level);
                m_windowLevelTool->setCurrentWindowLevel(window, level);
            }

            updateOverlay(viewportIndex);
            emit imageLoaded(viewportIndex);
        }
    } else {
        emit errorOccurred(result.errorMessage);
    }
}

void ImageViewerPageDelegate::onToolSelected(ToolType tool) {
    activateTool(tool);
}

void ImageViewerPageDelegate::activateTool(ToolType tool) {
    // Deactivate all tools
    m_windowLevelTool->deactivate();
    m_zoomTool->deactivate();
    m_panTool->deactivate();
    m_rulerTool->deactivate();
    m_angleTool->deactivate();
    m_resetTool->deactivate();

    // Activate selected tool
    m_currentTool = tool;

    switch (tool) {
        case ToolType::WINDOW_LEVEL:
            m_windowLevelTool->activate();
            break;
        case ToolType::ZOOM:
            m_zoomTool->activate();
            break;
        case ToolType::PAN:
            m_panTool->activate();
            break;
        case ToolType::RULER:
            m_rulerTool->activate();
            break;
        case ToolType::ANGLE:
            m_angleTool->activate();
            break;
        case ToolType::RESET:
            m_resetTool->activate();
            break;
        default:
            break;
    }
}

void ImageViewerPageDelegate::onLayoutSelected(ViewportLayout layout) {
    m_currentLayout = layout;
    m_viewportManager->setLayout(layout);
}

void ImageViewerPageDelegate::onViewportMousePressed(int viewportIndex, const QPointF& pos,
                                                      Qt::MouseButton button, Qt::KeyboardModifiers modifiers) {
    Q_UNUSED(modifiers)

    // Update active viewport
    if (viewportIndex != m_activeViewportIndex) {
        m_viewportManager->setActiveViewport(viewportIndex);
        m_activeViewportIndex = viewportIndex;
    }

    // Forward to active tool
    switch (m_currentTool) {
        case ToolType::WINDOW_LEVEL:
            m_windowLevelTool->onMousePress(pos, button);
            break;
        case ToolType::ZOOM:
            m_zoomTool->onMousePress(pos, button);
            break;
        case ToolType::PAN:
            m_panTool->onMousePress(pos, button);
            break;
        case ToolType::RULER:
            m_rulerTool->onMousePress(pos, button);
            break;
        case ToolType::ANGLE:
            m_angleTool->onMousePress(pos, button);
            break;
        default:
            break;
    }
}

void ImageViewerPageDelegate::onViewportMouseMoved(int viewportIndex, const QPointF& pos,
                                                    Qt::MouseButtons buttons, Qt::KeyboardModifiers modifiers) {
    Q_UNUSED(viewportIndex)
    Q_UNUSED(buttons)
    Q_UNUSED(modifiers)

    switch (m_currentTool) {
        case ToolType::WINDOW_LEVEL:
            m_windowLevelTool->onMouseMove(pos);
            break;
        case ToolType::ZOOM:
            m_zoomTool->onMouseMove(pos);
            break;
        case ToolType::PAN:
            m_panTool->onMouseMove(pos);
            break;
        case ToolType::RULER:
            m_rulerTool->onMouseMove(pos);
            break;
        case ToolType::ANGLE:
            m_angleTool->onMouseMove(pos);
            break;
        default:
            break;
    }
}

void ImageViewerPageDelegate::onViewportMouseReleased(int viewportIndex, const QPointF& pos,
                                                       Qt::MouseButton button, Qt::KeyboardModifiers modifiers) {
    Q_UNUSED(viewportIndex)
    Q_UNUSED(modifiers)

    switch (m_currentTool) {
        case ToolType::WINDOW_LEVEL:
            m_windowLevelTool->onMouseRelease(pos, button);
            break;
        case ToolType::ZOOM:
            m_zoomTool->onMouseRelease(pos, button);
            break;
        case ToolType::PAN:
            m_panTool->onMouseRelease(pos, button);
            break;
        case ToolType::RULER:
            m_rulerTool->onMouseRelease(pos, button);
            break;
        case ToolType::ANGLE:
            m_angleTool->onMouseRelease(pos, button);
            break;
        default:
            break;
    }
}

void ImageViewerPageDelegate::onViewportMouseWheel(int viewportIndex, int delta,
                                                    const QPointF& pos, Qt::KeyboardModifiers modifiers) {
    Q_UNUSED(viewportIndex)
    Q_UNUSED(modifiers)

    // Mouse wheel always controls zoom regardless of tool
    if (auto* renderer = m_viewportManager->activeRenderer()) {
        double currentZoom = renderer->getZoom();
        double zoomFactor = delta > 0 ? 1.1 : 0.9;
        renderer->setZoom(currentZoom * zoomFactor);
        updateOverlay(m_activeViewportIndex);
    }
}

void ImageViewerPageDelegate::onViewportDoubleClicked(int viewportIndex, const QPointF& pos,
                                                       Qt::MouseButton button) {
    Q_UNUSED(viewportIndex)
    Q_UNUSED(pos)

    if (button == Qt::LeftButton) {
        // Double-click: fit to window
        if (auto* renderer = m_viewportManager->activeRenderer()) {
            renderer->fitToWindow();
            updateOverlay(m_activeViewportIndex);
        }
    }
}

void ImageViewerPageDelegate::onActiveViewportChanged(int index) {
    m_activeViewportIndex = index;
    m_viewportManager->setActiveViewport(index);

    // Update tools with current viewport state
    if (auto* renderer = m_viewportManager->getRenderer(index)) {
        if (renderer->hasImage()) {
            double window, level;
            renderer->getWindowLevel(window, level);
            m_windowLevelTool->setCurrentWindowLevel(window, level);
            m_zoomTool->setCurrentZoom(renderer->getZoom());
        }
    }
}

void ImageViewerPageDelegate::onOpenFileRequested() {
    QString filter = "DICOM Files (*.dcm *.dicom);;PNG Files (*.png);;JPEG Files (*.jpg *.jpeg);;All Files (*)";
    QStringList files = QFileDialog::getOpenFileNames(m_ui, tr("Open Image Files"), QString(), filter);

    if (!files.isEmpty()) {
        loadFromFiles(files);
    }
}

void ImageViewerPageDelegate::onInvertRequested() {
    if (auto* renderer = m_viewportManager->activeRenderer()) {
        renderer->setInverted(!renderer->isInverted());
    }
}

void ImageViewerPageDelegate::onResetViewRequested() {
    if (auto* renderer = m_viewportManager->activeRenderer()) {
        renderer->resetCamera();
        renderer->resetWindowLevel();
        updateOverlay(m_activeViewportIndex);
    }
}

void ImageViewerPageDelegate::onFitToWindowRequested() {
    if (auto* renderer = m_viewportManager->activeRenderer()) {
        renderer->fitToWindow();
        updateOverlay(m_activeViewportIndex);
    }
}

void ImageViewerPageDelegate::onFilesDropped(const QStringList& filePaths) {
    loadFromFiles(filePaths);
}

void ImageViewerPageDelegate::onThumbnailClicked(int id, bool isSeries) {
    if (isSeries) {
        loadFromSeries(id);
    } else {
        // Load specific image
        // TODO: Implement
    }
}

void ImageViewerPageDelegate::onWindowLevelChanged(double window, double level) {
    if (auto* renderer = m_viewportManager->activeRenderer()) {
        renderer->setWindowLevel(window, level);
        updateOverlay(m_activeViewportIndex);
    }
}

void ImageViewerPageDelegate::onZoomRequested(double factor, const QPointF& centerPos) {
    Q_UNUSED(centerPos)
    if (auto* renderer = m_viewportManager->activeRenderer()) {
        renderer->setZoom(factor);
        updateOverlay(m_activeViewportIndex);
    }
}

void ImageViewerPageDelegate::onPanRequested(double deltaX, double deltaY) {
    if (auto* renderer = m_viewportManager->activeRenderer()) {
        renderer->setPan(deltaX, deltaY);
    }
}

void ImageViewerPageDelegate::onResetRequested() {
    onResetViewRequested();
}

void ImageViewerPageDelegate::onMeasurementLineUpdated(const MeasurementLine& line) {
    Q_UNUSED(line)
    // TODO: Update measurement visualization in renderer
}

void ImageViewerPageDelegate::onMeasurementLineCompleted(const MeasurementLine& line) {
    Q_UNUSED(line)
    // TODO: Store measurement and update visualization
}

void ImageViewerPageDelegate::updateOverlay(int viewportIndex) {
    auto* renderer = m_viewportManager->getRenderer(viewportIndex);
    if (!renderer) return;

    renderer->setOverlayText(OverlayCorner::TOP_LEFT, formatOverlayText(OverlayCorner::TOP_LEFT, viewportIndex));
    renderer->setOverlayText(OverlayCorner::TOP_RIGHT, formatOverlayText(OverlayCorner::TOP_RIGHT, viewportIndex));
    renderer->setOverlayText(OverlayCorner::BOTTOM_LEFT, formatOverlayText(OverlayCorner::BOTTOM_LEFT, viewportIndex));
    renderer->setOverlayText(OverlayCorner::BOTTOM_RIGHT, formatOverlayText(OverlayCorner::BOTTOM_RIGHT, viewportIndex));
}

void ImageViewerPageDelegate::updateAllOverlays() {
    int count = m_viewportManager->visibleViewportCount();
    for (int i = 0; i < count; ++i) {
        updateOverlay(i);
    }
}

QString ImageViewerPageDelegate::formatOverlayText(OverlayCorner corner, int viewportIndex) const {
    const auto& meta = m_viewportMetadata[viewportIndex];
    auto* renderer = m_viewportManager->getRenderer(viewportIndex);

    if (!renderer || !renderer->hasImage()) {
        return QString();
    }

    switch (corner) {
        case OverlayCorner::TOP_LEFT: {
            QString text;
            if (!meta.patientName.isEmpty()) {
                text += meta.patientName + "\n";
            }
            if (!meta.patientId.isEmpty()) {
                text += "ID: " + meta.patientId;
            }
            return text;
        }
        case OverlayCorner::TOP_RIGHT: {
            QString text;
            if (!meta.studyDate.isEmpty()) {
                text += meta.studyDate;
            }
            return text;
        }
        case OverlayCorner::BOTTOM_LEFT: {
            double window, level;
            renderer->getWindowLevel(window, level);
            double zoom = renderer->getZoom();

            return QString("Zoom: %1%\nWW/WL: %2/%3")
                .arg(static_cast<int>(zoom * 100))
                .arg(static_cast<int>(window))
                .arg(static_cast<int>(level));
        }
        case OverlayCorner::BOTTOM_RIGHT: {
            QString text;
            if (meta.instanceNumber > 0) {
                text += QString("Image: %1").arg(meta.instanceNumber);
            }
            if (!meta.seriesDescription.isEmpty()) {
                if (!text.isEmpty()) text += "\n";
                text += meta.seriesDescription;
            }
            return text;
        }
        default:
            return QString();
    }
}

} // namespace Etrek::Application::Delegate
