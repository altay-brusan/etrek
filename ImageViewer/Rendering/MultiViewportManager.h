#ifndef MULTIVIEWPORTMANAGER_H
#define MULTIVIEWPORTMANAGER_H

#include <QObject>
#include <array>
#include <memory>
#include <functional>

#include "ViewportLayout.h"

// Forward declarations
class QVTKOpenGLNativeWidget;
class vtkImageData;

namespace Etrek::ImageViewer {
struct ViewportState;
struct DicomMetadata;
}

namespace Etrek::ImageViewer::Rendering {

class VtkViewportRenderer;

/**
 * @class MultiViewportManager
 * @brief Manages multiple viewport renderers (up to 4 viewports).
 *
 * Handles:
 * - Layout configuration (1x1, 1x2, 2x1, 2x2)
 * - Active viewport selection
 * - Optional synchronized operations (linked viewports)
 */
class MultiViewportManager : public QObject {
    Q_OBJECT

public:
    static constexpr int MAX_VIEWPORTS = 4;

    explicit MultiViewportManager(QObject* parent = nullptr);
    ~MultiViewportManager() override;

    /**
     * @brief Initialize viewports with VTK widgets.
     * @param widgets Array of 4 QVTKOpenGLNativeWidgets
     */
    void initialize(const std::array<QVTKOpenGLNativeWidget*, MAX_VIEWPORTS>& widgets);

    /**
     * @brief Check if manager is initialized.
     */
    bool isInitialized() const;

    // Layout management
    void setLayout(ViewportLayout layout);
    ViewportLayout currentLayout() const;

    /**
     * @brief Get number of visible viewports for current layout.
     */
    int visibleViewportCount() const;

    // Viewport access
    VtkViewportRenderer* getRenderer(int index);
    const VtkViewportRenderer* getRenderer(int index) const;

    // Active viewport management
    int activeViewportIndex() const;
    void setActiveViewport(int index);
    VtkViewportRenderer* activeRenderer();

    // Image operations
    void setImageData(int viewportIndex, vtkImageData* imageData, bool autoWindowLevel = true);
    void clearViewport(int viewportIndex);
    void clearAllViewports();

    // Linked mode operations (synchronized viewports)
    void setLinkedMode(bool linked);
    bool isLinkedMode() const;

    /**
     * @brief Synchronize window/level from active viewport to all others.
     */
    void synchronizeWindowLevel();

    /**
     * @brief Synchronize zoom from active viewport to all others.
     */
    void synchronizeZoom();

    /**
     * @brief Synchronize pan from active viewport to all others.
     */
    void synchronizePan();

    /**
     * @brief Apply an operation to all visible viewports.
     * @param operation Function to apply to each renderer
     */
    void forEachVisibleViewport(std::function<void(VtkViewportRenderer*, int)> operation);

    /**
     * @brief Apply an operation to all viewports.
     * @param operation Function to apply to each renderer
     */
    void forEachViewport(std::function<void(VtkViewportRenderer*, int)> operation);

    // Render operations
    void renderAll();
    void renderViewport(int index);

signals:
    /**
     * @brief Emitted when layout changes.
     */
    void layoutChanged(ViewportLayout layout);

    /**
     * @brief Emitted when active viewport changes.
     */
    void activeViewportChanged(int index);

    /**
     * @brief Emitted when a viewport's window/level changes.
     */
    void viewportWindowLevelChanged(int viewportIndex, double window, double level);

    /**
     * @brief Emitted when a viewport's zoom changes.
     */
    void viewportZoomChanged(int viewportIndex, double factor);

    /**
     * @brief Emitted when a viewport's image changes.
     */
    void viewportImageChanged(int viewportIndex);

private slots:
    void onViewportWindowLevelChanged(double window, double level);
    void onViewportZoomChanged(double factor);
    void onViewportImageChanged();

private:
    void connectRendererSignals(int index);

    std::array<std::unique_ptr<VtkViewportRenderer>, MAX_VIEWPORTS> m_renderers;
    ViewportLayout m_layout = ViewportLayout::SINGLE;
    int m_activeIndex = 0;
    bool m_linkedMode = false;
    bool m_initialized = false;
};

} // namespace Etrek::ImageViewer::Rendering

#endif // MULTIVIEWPORTMANAGER_H
