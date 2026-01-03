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
#include "MagnifierWidget.h"
#include "RulerOverlayWidget.h"
#include "AngleOverlayWidget.h"
#include "EdgeRulerWidget.h"

#include <QTimer>
#include <QFileDialog>
#include <QMessageBox>
#include <QEvent>
#include <QVTKOpenGLNativeWidget.h>

using namespace Etrek::ImageViewer;
using namespace Etrek::ImageViewer::Rendering;
using namespace Etrek::ImageViewer::Service;
using namespace Etrek::ImageViewer::Tool;
using namespace Etrek::ImageViewer::Widget;

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

bool ImageViewerPageDelegate::eventFilter(QObject* watched, QEvent* event) {
    if (event->type() == QEvent::Resize) {
        // Find which viewport widget was resized
        auto widgets = m_ui->getAllVtkWidgets();
        for (int i = 0; i < 4; ++i) {
            if (widgets[i] == watched) {
                // Resize ruler overlay to match the VTK widget
                if (m_rulerOverlays[i]) {
                    m_rulerOverlays[i]->setGeometry(0, 0, widgets[i]->width(), widgets[i]->height());
                    m_rulerOverlays[i]->refresh();
                }
                // Resize angle overlay to match the VTK widget
                if (m_angleOverlays[i]) {
                    m_angleOverlays[i]->setGeometry(0, 0, widgets[i]->width(), widgets[i]->height());
                    m_angleOverlays[i]->refresh();
                }
                // Resize edge ruler overlay to match the VTK widget
                if (m_edgeRulers[i]) {
                    m_edgeRulers[i]->setGeometry(0, 0, widgets[i]->width(), widgets[i]->height());
                    m_edgeRulers[i]->update();
                }
                break;
            }
        }
    }
    return QObject::eventFilter(watched, event);
}

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
    initializeRulerOverlays();
    initializeAngleOverlays();
    initializeEdgeRulers();

    // Check if there's context data to load
    if (auto ctx = m_contextManager.lock()) {
        // TODO: Load from context if available
    }
}

void ImageViewerPageDelegate::initializeViewports() {
    auto widgets = m_ui->getAllVtkWidgets();
    m_viewportManager->initialize(widgets);
}

void ImageViewerPageDelegate::initializeRulerOverlays() {
    auto widgets = m_ui->getAllVtkWidgets();

    for (int i = 0; i < 4; ++i) {
        if (widgets[i]) {
            // Parent to the VTK widget directly so overlay matches its size
            m_rulerOverlays[i] = new RulerOverlayWidget(widgets[i]);
            m_rulerOverlays[i]->setGeometry(0, 0, widgets[i]->width(), widgets[i]->height());
            m_rulerOverlays[i]->raise();
            m_rulerOverlays[i]->show();

            // Install event filter on the VTK widget to track resize
            widgets[i]->installEventFilter(this);

            // Set up coordinate transform using the renderer
            auto* renderer = m_viewportManager->getRenderer(i);
            if (renderer) {
                m_rulerOverlays[i]->setImageToWidgetTransform(
                    [renderer](const QPointF& imagePos) {
                        return renderer->imageToWidgetCoords(imagePos);
                    }
                );
            }
        }
    }
}

void ImageViewerPageDelegate::refreshRulerOverlays() {
    for (int i = 0; i < 4; ++i) {
        if (m_rulerOverlays[i]) {
            // Update measurements from the ruler tool
            m_rulerOverlays[i]->setMeasurements(m_rulerTool->getMeasurements());
            m_rulerOverlays[i]->setSelectedRuler(m_rulerTool->selectedRuler());
            m_rulerOverlays[i]->refresh();
        }
    }
}

void ImageViewerPageDelegate::updateRulerOverlayTransforms() {
    for (int i = 0; i < 4; ++i) {
        if (m_rulerOverlays[i]) {
            auto* renderer = m_viewportManager->getRenderer(i);
            if (renderer) {
                m_rulerOverlays[i]->setImageToWidgetTransform(
                    [renderer](const QPointF& imagePos) {
                        return renderer->imageToWidgetCoords(imagePos);
                    }
                );
            }
            m_rulerOverlays[i]->refresh();
        }
    }
}

void ImageViewerPageDelegate::initializeAngleOverlays() {
    auto widgets = m_ui->getAllVtkWidgets();

    for (int i = 0; i < 4; ++i) {
        if (widgets[i]) {
            // Parent to the VTK widget directly so overlay matches its size
            m_angleOverlays[i] = new AngleOverlayWidget(widgets[i]);
            m_angleOverlays[i]->setGeometry(0, 0, widgets[i]->width(), widgets[i]->height());
            m_angleOverlays[i]->raise();
            m_angleOverlays[i]->show();

            // Set up coordinate transform using the renderer
            auto* renderer = m_viewportManager->getRenderer(i);
            if (renderer) {
                m_angleOverlays[i]->setImageToWidgetTransform(
                    [renderer](const QPointF& imagePos) {
                        return renderer->imageToWidgetCoords(imagePos);
                    }
                );
            }
        }
    }
}

void ImageViewerPageDelegate::initializeEdgeRulers() {
    auto widgets = m_ui->getAllVtkWidgets();

    for (int i = 0; i < 4; ++i) {
        if (widgets[i]) {
            // Parent to the VTK widget directly so overlay matches its size
            m_edgeRulers[i] = new EdgeRulerWidget(widgets[i]);
            m_edgeRulers[i]->setGeometry(0, 0, widgets[i]->width(), widgets[i]->height());
            m_edgeRulers[i]->raise();
            m_edgeRulers[i]->show();

            // Initially hide until an image is loaded
            m_edgeRulers[i]->setVisible(false);
        }
    }
}

void ImageViewerPageDelegate::updateEdgeRulers(int viewportIndex) {
    if (viewportIndex < 0 || viewportIndex >= 4) return;
    if (!m_edgeRulers[viewportIndex]) return;

    auto* renderer = m_viewportManager->getRenderer(viewportIndex);
    if (!renderer || !renderer->hasImage()) {
        m_edgeRulers[viewportIndex]->setVisible(false);
        return;
    }

    // Get pixel spacing from metadata
    const auto& meta = m_viewportMetadata[viewportIndex];
    double spacingX = meta.pixelSpacingX;
    double spacingY = meta.pixelSpacingY;

    // If pixel spacing is invalid, hide the ruler
    if (spacingX <= 0 || spacingY <= 0) {
        m_edgeRulers[viewportIndex]->setVisible(false);
        return;
    }

    // Get image dimensions
    int imageWidth, imageHeight;
    renderer->getImageDimensions(imageWidth, imageHeight);

    // Update edge ruler with image info
    m_edgeRulers[viewportIndex]->setPixelSpacing(spacingX, spacingY);
    m_edgeRulers[viewportIndex]->setImageDimensions(imageWidth, imageHeight);
    m_edgeRulers[viewportIndex]->setZoomLevel(renderer->getZoom());

    // Set coordinate transform for converting image coords to widget coords
    m_edgeRulers[viewportIndex]->setImageToWidgetTransform(
        [renderer](const QPointF& imagePos) {
            return renderer->imageToWidgetCoords(imagePos);
        }
    );

    m_edgeRulers[viewportIndex]->setVisible(true);
    m_edgeRulers[viewportIndex]->update();
}

void ImageViewerPageDelegate::refreshAngleOverlays() {
    for (int i = 0; i < 4; ++i) {
        if (m_angleOverlays[i]) {
            // Update measurements from the angle tool
            m_angleOverlays[i]->setMeasurements(m_angleTool->getMeasurements());
            m_angleOverlays[i]->setSelectedAngle(m_angleTool->selectedAngle());
            m_angleOverlays[i]->refresh();
        }
    }
}

void ImageViewerPageDelegate::updateAngleOverlayTransforms() {
    for (int i = 0; i < 4; ++i) {
        if (m_angleOverlays[i]) {
            auto* renderer = m_viewportManager->getRenderer(i);
            if (renderer) {
                m_angleOverlays[i]->setImageToWidgetTransform(
                    [renderer](const QPointF& imagePos) {
                        return renderer->imageToWidgetCoords(imagePos);
                    }
                );
            }
            m_angleOverlays[i]->refresh();
        }
    }
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
    connect(m_rulerTool.get(), &RulerTool::measurementsChanged,
            this, &ImageViewerPageDelegate::onRulerMeasurementsChanged);
    connect(m_rulerTool.get(), &RulerTool::rulerSelected,
            this, &ImageViewerPageDelegate::onRulerSelected);
    connect(m_rulerTool.get(), &RulerTool::rulerDeleted,
            this, &ImageViewerPageDelegate::onRulerDeleted);
    connect(m_rulerTool.get(), &RulerTool::handleHovered,
            this, [this](int rulerId, bool isStartHandle) {
                for (int i = 0; i < 4; ++i) {
                    if (m_rulerOverlays[i]) {
                        m_rulerOverlays[i]->setHoveredHandle(rulerId, isStartHandle);
                    }
                }
            });
    connect(m_rulerTool.get(), &RulerTool::handleHoverCleared,
            this, [this]() {
                for (int i = 0; i < 4; ++i) {
                    if (m_rulerOverlays[i]) {
                        m_rulerOverlays[i]->clearHoveredHandle();
                    }
                }
            });
    connect(m_resetTool.get(), &ResetTool::resetRequested,
            this, &ImageViewerPageDelegate::onResetRequested);

    // Connect cursor change signals from all tools
    connect(m_windowLevelTool.get(), &WindowLevelTool::cursorChanged,
            this, &ImageViewerPageDelegate::onCursorChanged);
    connect(m_zoomTool.get(), &ZoomTool::cursorChanged,
            this, &ImageViewerPageDelegate::onCursorChanged);
    connect(m_panTool.get(), &PanTool::cursorChanged,
            this, &ImageViewerPageDelegate::onCursorChanged);
    connect(m_rulerTool.get(), &RulerTool::cursorChanged,
            this, &ImageViewerPageDelegate::onCursorChanged);
    connect(m_angleTool.get(), &AngleTool::cursorChanged,
            this, &ImageViewerPageDelegate::onCursorChanged);
    connect(m_resetTool.get(), &ResetTool::cursorChanged,
            this, &ImageViewerPageDelegate::onCursorChanged);

    // Connect angle tool signals
    connect(m_angleTool.get(), &AngleTool::measurementAngleUpdated,
            this, &ImageViewerPageDelegate::onAngleUpdated);
    connect(m_angleTool.get(), &AngleTool::measurementAngleCompleted,
            this, &ImageViewerPageDelegate::onAngleCompleted);
    connect(m_angleTool.get(), &AngleTool::measurementsChanged,
            this, &ImageViewerPageDelegate::onAngleMeasurementsChanged);
    connect(m_angleTool.get(), &AngleTool::angleSelected,
            this, &ImageViewerPageDelegate::onAngleSelected);
    connect(m_angleTool.get(), &AngleTool::angleDeleted,
            this, &ImageViewerPageDelegate::onAngleDeleted);
    connect(m_angleTool.get(), &AngleTool::handleHovered,
            this, [this](int angleId, AngleHandleType handleType) {
                for (int i = 0; i < 4; ++i) {
                    if (m_angleOverlays[i]) {
                        m_angleOverlays[i]->setHoveredHandle(angleId, handleType);
                    }
                }
            });
    connect(m_angleTool.get(), &AngleTool::handleHoverCleared,
            this, [this]() {
                for (int i = 0; i < 4; ++i) {
                    if (m_angleOverlays[i]) {
                        m_angleOverlays[i]->clearHoveredHandle();
                    }
                }
            });

    // Set pan tool sensitivity for more responsive panning
    m_panTool->setSensitivity(2.0);

    // Create magnifier widget (parented to the UI for proper overlay)
    m_magnifier = new MagnifierWidget(m_ui);
    m_magnifier->hide();

    // Connect magnifier signals from ZoomTool
    connect(m_zoomTool.get(), &ZoomTool::magnifierRequested,
            this, &ImageViewerPageDelegate::onMagnifierRequested);
    connect(m_zoomTool.get(), &ZoomTool::magnifierHideRequested,
            this, &ImageViewerPageDelegate::onMagnifierHideRequested);
    connect(m_zoomTool.get(), &ZoomTool::magnifierToggleRequested,
            this, &ImageViewerPageDelegate::onMagnifierToggleRequested);

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

    // Ruler/Angle deletion via Delete/Backspace key
    connect(m_ui, &ImageViewerPage::deleteSelectedRulerRequested,
            this, [this]() {
                if (m_currentTool == ToolType::RULER && m_rulerTool) {
                    m_rulerTool->deleteSelectedRuler();
                } else if (m_currentTool == ToolType::ANGLE && m_angleTool) {
                    m_angleTool->deleteSelectedAngle();
                }
            });

    // Clear all rulers/angles via Ctrl+Delete
    connect(m_ui, &ImageViewerPage::clearAllRulersRequested,
            this, [this]() {
                if (m_currentTool == ToolType::RULER && m_rulerTool) {
                    m_rulerTool->clearMeasurements();
                    refreshRulerOverlays();
                } else if (m_currentTool == ToolType::ANGLE && m_angleTool) {
                    m_angleTool->clearMeasurements();
                    refreshAngleOverlays();
                }
            });
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

            // Auto-fit image to fill the viewport
            renderer->fitToWindow();

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

            // Auto-fit image to fill the viewport
            renderer->fitToWindow();

            updateOverlay(viewportIndex);
            updateEdgeRulers(viewportIndex);
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

    // For ruler tool, convert widget coords to image coords
    QPointF toolPos = pos;
    bool validImageCoords = true;
    if (m_currentTool == ToolType::RULER || m_currentTool == ToolType::ANGLE) {
        if (auto* renderer = m_viewportManager->activeRenderer()) {
            toolPos = renderer->widgetToImageCoords(pos);
            // Check if coordinates are valid (inside image bounds)
            if (toolPos.x() < 0 || toolPos.y() < 0) {
                validImageCoords = false;
            }
        }
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
            // Only forward if coordinates are valid (inside image)
            if (validImageCoords) {
                m_rulerTool->onMousePress(toolPos, button);
            }
            break;
        case ToolType::ANGLE:
            if (validImageCoords) {
                m_angleTool->onMousePress(toolPos, button);
            }
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

    // For ruler tool, convert widget coords to image coords
    QPointF toolPos = pos;
    if (m_currentTool == ToolType::RULER || m_currentTool == ToolType::ANGLE) {
        if (auto* renderer = m_viewportManager->activeRenderer()) {
            toolPos = renderer->widgetToImageCoords(pos);
            // If out of bounds (-1,-1), clamp to image edges
            if (toolPos.x() < 0 || toolPos.y() < 0) {
                int imgWidth, imgHeight;
                renderer->getImageDimensions(imgWidth, imgHeight);
                // Re-calculate but clamp to bounds
                // For now, just skip if invalid
                toolPos = QPointF(
                    qBound(0.0, toolPos.x() < 0 ? 0.0 : toolPos.x(), static_cast<double>(imgWidth - 1)),
                    qBound(0.0, toolPos.y() < 0 ? 0.0 : toolPos.y(), static_cast<double>(imgHeight - 1))
                );
            }
        }
    }

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
            m_rulerTool->onMouseMove(toolPos);
            break;
        case ToolType::ANGLE:
            m_angleTool->onMouseMove(toolPos);
            break;
        default:
            break;
    }
}

void ImageViewerPageDelegate::onViewportMouseReleased(int viewportIndex, const QPointF& pos,
                                                       Qt::MouseButton button, Qt::KeyboardModifiers modifiers) {
    Q_UNUSED(viewportIndex)
    Q_UNUSED(modifiers)

    // For ruler tool, convert widget coords to image coords
    QPointF toolPos = pos;
    if (m_currentTool == ToolType::RULER || m_currentTool == ToolType::ANGLE) {
        if (auto* renderer = m_viewportManager->activeRenderer()) {
            toolPos = renderer->widgetToImageCoords(pos);
            // If out of bounds (-1,-1), clamp to image edges
            if (toolPos.x() < 0 || toolPos.y() < 0) {
                int imgWidth, imgHeight;
                renderer->getImageDimensions(imgWidth, imgHeight);
                toolPos = QPointF(
                    qBound(0.0, toolPos.x() < 0 ? 0.0 : toolPos.x(), static_cast<double>(imgWidth - 1)),
                    qBound(0.0, toolPos.y() < 0 ? 0.0 : toolPos.y(), static_cast<double>(imgHeight - 1))
                );
            }
        }
    }

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
            m_rulerTool->onMouseRelease(toolPos, button);
            break;
        case ToolType::ANGLE:
            m_angleTool->onMouseRelease(toolPos, button);
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
        updateEdgeRulers(m_activeViewportIndex);
        refreshRulerOverlays();
        refreshAngleOverlays();
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
            updateEdgeRulers(m_activeViewportIndex);
            refreshRulerOverlays();
            refreshAngleOverlays();
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
        updateEdgeRulers(m_activeViewportIndex);
    }

    // Clear all ruler measurements
    if (m_rulerTool) {
        m_rulerTool->clearMeasurements();
    }
    for (int i = 0; i < 4; ++i) {
        if (m_rulerOverlays[i]) {
            m_rulerOverlays[i]->setMeasurements(QVector<MeasurementLine>());
            m_rulerOverlays[i]->clearCurrentMeasurement();
            m_rulerOverlays[i]->refresh();
        }
    }

    // Clear all angle measurements
    if (m_angleTool) {
        m_angleTool->clearMeasurements();
    }
    for (int i = 0; i < 4; ++i) {
        if (m_angleOverlays[i]) {
            m_angleOverlays[i]->setMeasurements(QVector<MeasurementAngle>());
            m_angleOverlays[i]->clearCurrentMeasurement();
            m_angleOverlays[i]->refresh();
        }
    }

    // Switch back to Window/Level tool (default tool)
    activateTool(ToolType::WINDOW_LEVEL);

    // Update the tool panel UI to show Window/Level as selected
    if (m_ui->getToolPanel()) {
        m_ui->getToolPanel()->setSelectedTool(ToolType::WINDOW_LEVEL);
    }
}

void ImageViewerPageDelegate::onFitToWindowRequested() {
    if (auto* renderer = m_viewportManager->activeRenderer()) {
        renderer->fitToWindow();
        updateOverlay(m_activeViewportIndex);
        updateEdgeRulers(m_activeViewportIndex);
        refreshRulerOverlays();
        refreshAngleOverlays();
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
        updateEdgeRulers(m_activeViewportIndex);
        refreshRulerOverlays();
        refreshAngleOverlays();
    }
}

void ImageViewerPageDelegate::onPanRequested(double deltaX, double deltaY) {
    if (auto* renderer = m_viewportManager->activeRenderer()) {
        renderer->setPan(deltaX, deltaY);
        refreshRulerOverlays();
        refreshAngleOverlays();
    }
}

void ImageViewerPageDelegate::onResetRequested() {
    onResetViewRequested();
}

void ImageViewerPageDelegate::onMeasurementLineUpdated(const MeasurementLine& line) {
    // Update the ruler overlay with the in-progress measurement
    if (m_activeViewportIndex >= 0 && m_activeViewportIndex < 4) {
        if (m_rulerOverlays[m_activeViewportIndex]) {
            m_rulerOverlays[m_activeViewportIndex]->setCurrentMeasurement(line);
            m_rulerOverlays[m_activeViewportIndex]->refresh();
        }
    }
}

void ImageViewerPageDelegate::onMeasurementLineCompleted(const MeasurementLine& line) {
    Q_UNUSED(line)
    // Clear the in-progress measurement and refresh with all measurements
    if (m_activeViewportIndex >= 0 && m_activeViewportIndex < 4) {
        if (m_rulerOverlays[m_activeViewportIndex]) {
            m_rulerOverlays[m_activeViewportIndex]->clearCurrentMeasurement();
            m_rulerOverlays[m_activeViewportIndex]->setMeasurements(m_rulerTool->getMeasurements());
            m_rulerOverlays[m_activeViewportIndex]->refresh();
        }
    }
}

void ImageViewerPageDelegate::onRulerMeasurementsChanged() {
    // Refresh all ruler overlays when measurements change
    refreshRulerOverlays();
}

void ImageViewerPageDelegate::onRulerSelected(int rulerId) {
    // Update the selected ruler in the overlay
    for (int i = 0; i < 4; ++i) {
        if (m_rulerOverlays[i]) {
            m_rulerOverlays[i]->setSelectedRuler(rulerId);
            m_rulerOverlays[i]->refresh();
        }
    }
}

void ImageViewerPageDelegate::onRulerDeleted(int rulerId) {
    Q_UNUSED(rulerId)
    // Refresh overlays after deletion
    refreshRulerOverlays();
}

void ImageViewerPageDelegate::onAngleUpdated(const MeasurementAngle& angle) {
    // Update the angle overlay with the in-progress measurement
    if (m_activeViewportIndex >= 0 && m_activeViewportIndex < 4) {
        if (m_angleOverlays[m_activeViewportIndex]) {
            m_angleOverlays[m_activeViewportIndex]->setCurrentMeasurement(angle);
            m_angleOverlays[m_activeViewportIndex]->refresh();
        }
    }
}

void ImageViewerPageDelegate::onAngleCompleted(const MeasurementAngle& angle) {
    Q_UNUSED(angle)
    // Clear the in-progress measurement and refresh with all measurements
    if (m_activeViewportIndex >= 0 && m_activeViewportIndex < 4) {
        if (m_angleOverlays[m_activeViewportIndex]) {
            m_angleOverlays[m_activeViewportIndex]->clearCurrentMeasurement();
            m_angleOverlays[m_activeViewportIndex]->setMeasurements(m_angleTool->getMeasurements());
            m_angleOverlays[m_activeViewportIndex]->refresh();
        }
    }
}

void ImageViewerPageDelegate::onAngleMeasurementsChanged() {
    // Refresh all angle overlays when measurements change
    refreshAngleOverlays();
}

void ImageViewerPageDelegate::onAngleSelected(int angleId) {
    // Update the selected angle in the overlay
    for (int i = 0; i < 4; ++i) {
        if (m_angleOverlays[i]) {
            m_angleOverlays[i]->setSelectedAngle(angleId);
            m_angleOverlays[i]->refresh();
        }
    }
}

void ImageViewerPageDelegate::onAngleDeleted(int angleId) {
    Q_UNUSED(angleId)
    // Refresh overlays after deletion
    refreshAngleOverlays();
}

void ImageViewerPageDelegate::onCursorChanged(Qt::CursorShape cursor) {
    // Apply cursor to all visible viewport widgets and their parents
    auto widgets = m_ui->getAllVtkWidgets();
    int visibleCount = m_viewportManager->visibleViewportCount();

    for (int i = 0; i < visibleCount; ++i) {
        if (widgets[i]) {
            widgets[i]->setCursor(cursor);
            // Also set on parent widget (ImageViewport)
            if (widgets[i]->parentWidget()) {
                widgets[i]->parentWidget()->setCursor(cursor);
            }
        }
    }
}

void ImageViewerPageDelegate::onMagnifierRequested(const QPointF& pos) {
    auto* renderer = m_viewportManager->activeRenderer();
    if (!renderer || !renderer->hasImage() || !m_magnifier) {
        return;
    }

    // Get the current image as QImage
    QImage sourceImage = renderer->getImageAsQImage();
    if (sourceImage.isNull()) {
        return;
    }

    // Get the active viewport widget to calculate screen position
    auto widgets = m_ui->getAllVtkWidgets();
    if (m_activeViewportIndex >= 0 && m_activeViewportIndex < widgets.size()) {
        QWidget* viewportWidget = widgets[m_activeViewportIndex];
        if (viewportWidget) {
            // Convert widget coordinates to image pixel coordinates using renderer's method
            // This properly accounts for zoom, pan, and coordinate system flips
            QPointF imagePos = renderer->widgetToImageCoords(pos);

            // If outside image bounds, hide magnifier and return
            if (imagePos.x() < 0 || imagePos.y() < 0) {
                if (m_magnifier->isVisible()) {
                    m_magnifier->hide();
                }
                return;
            }

            // Convert widget coordinates to parent (m_ui) coordinates for positioning
            QPoint widgetPos = pos.toPoint();
            QPoint globalPos = viewportWidget->mapToGlobal(widgetPos);
            QPoint parentPos = m_ui->mapFromGlobal(globalPos);

            // Update magnifier - pass image coordinates for sampling, parent coords for positioning
            m_magnifier->updateMagnifier(sourceImage, imagePos, parentPos);

            // Show magnifier if not visible
            if (!m_magnifier->isVisible()) {
                m_magnifier->show();
                m_magnifier->raise();
            }
        }
    }
}

void ImageViewerPageDelegate::onMagnifierHideRequested() {
    if (m_magnifier) {
        m_magnifier->hideMagnifier();
    }
}

void ImageViewerPageDelegate::onMagnifierToggleRequested() {
    if (m_magnifier) {
        m_magnifier->toggleMagnification();
    }
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
            // Patient demographics
            QString text;
            if (!meta.patientName.isEmpty()) {
                text += meta.patientName + "\n";
            }
            if (!meta.patientId.isEmpty()) {
                text += "ID: " + meta.patientId + "\n";
            }
            // Add birth date and sex on same line if available
            QString demographics;
            if (!meta.patientBirthDate.isEmpty()) {
                demographics += "DOB: " + meta.patientBirthDate;
            }
            if (!meta.patientSex.isEmpty()) {
                if (!demographics.isEmpty()) demographics += "  ";
                demographics += meta.patientSex;
            }
            if (!demographics.isEmpty()) {
                text += demographics;
            }
            // Remove trailing newline if present
            while (text.endsWith('\n')) {
                text.chop(1);
            }
            return text;
        }
        case OverlayCorner::TOP_RIGHT: {
            // Study information
            QString text;
            if (!meta.studyDate.isEmpty()) {
                text += meta.studyDate;
                if (!meta.studyTime.isEmpty()) {
                    // Format time if available (take first 6 chars: HHMMSS)
                    QString time = meta.studyTime.left(6);
                    if (time.length() >= 4) {
                        text += " " + time.left(2) + ":" + time.mid(2, 2);
                    }
                }
                text += "\n";
            }
            if (!meta.studyDescription.isEmpty()) {
                text += meta.studyDescription + "\n";
            }
            if (!meta.accessionNumber.isEmpty()) {
                text += "Acc#: " + meta.accessionNumber;
            }
            // Remove trailing newline if present
            while (text.endsWith('\n')) {
                text.chop(1);
            }
            return text;
        }
        case OverlayCorner::BOTTOM_LEFT: {
            // Technical parameters
            double window, level;
            renderer->getWindowLevel(window, level);
            double zoom = renderer->getZoom();

            QString text = QString("Zoom: %1%\nWW/WL: %2/%3")
                .arg(static_cast<int>(zoom * 100))
                .arg(static_cast<int>(window))
                .arg(static_cast<int>(level));

            // Add modality if available
            if (!meta.modality.isEmpty()) {
                text += "\n" + meta.modality;
            }

            return text;
        }
        case OverlayCorner::BOTTOM_RIGHT: {
            // Image/Series information
            QString text;
            if (meta.instanceNumber > 0) {
                if (meta.totalImages > 1) {
                    text += QString("Image: %1/%2").arg(meta.instanceNumber).arg(meta.totalImages);
                } else {
                    text += QString("Image: %1").arg(meta.instanceNumber);
                }
            }
            if (!meta.seriesDescription.isEmpty()) {
                if (!text.isEmpty()) text += "\n";
                text += meta.seriesDescription;
            }
            if (!meta.bodyPartExamined.isEmpty()) {
                if (!text.isEmpty()) text += "\n";
                text += meta.bodyPartExamined;
            }
            return text;
        }
        default:
            return QString();
    }
}

} // namespace Etrek::Application::Delegate
