#include "MultiViewportManager.h"
#include "VtkViewportRenderer.h"

#include <QVTKOpenGLNativeWidget.h>

namespace Etrek::ImageViewer::Rendering {

MultiViewportManager::MultiViewportManager(QObject* parent)
    : QObject(parent)
{
}

MultiViewportManager::~MultiViewportManager() = default;

void MultiViewportManager::initialize(const std::array<QVTKOpenGLNativeWidget*, MAX_VIEWPORTS>& widgets) {
    for (int i = 0; i < MAX_VIEWPORTS; ++i) {
        if (widgets[i]) {
            m_renderers[i] = std::make_unique<VtkViewportRenderer>(widgets[i], this);
            connectRendererSignals(i);
        }
    }
    m_initialized = true;
}

bool MultiViewportManager::isInitialized() const {
    return m_initialized;
}

void MultiViewportManager::setLayout(ViewportLayout layout) {
    if (m_layout == layout) return;

    m_layout = layout;

    // Ensure active viewport is visible in new layout
    int visibleCount = visibleViewportCount();
    if (m_activeIndex >= visibleCount) {
        m_activeIndex = 0;
        emit activeViewportChanged(m_activeIndex);
    }

    emit layoutChanged(layout);
}

ViewportLayout MultiViewportManager::currentLayout() const {
    return m_layout;
}

int MultiViewportManager::visibleViewportCount() const {
    return viewportCount(m_layout);
}

VtkViewportRenderer* MultiViewportManager::getRenderer(int index) {
    if (index >= 0 && index < MAX_VIEWPORTS) {
        return m_renderers[index].get();
    }
    return nullptr;
}

const VtkViewportRenderer* MultiViewportManager::getRenderer(int index) const {
    if (index >= 0 && index < MAX_VIEWPORTS) {
        return m_renderers[index].get();
    }
    return nullptr;
}

int MultiViewportManager::activeViewportIndex() const {
    return m_activeIndex;
}

void MultiViewportManager::setActiveViewport(int index) {
    if (index < 0 || index >= visibleViewportCount()) {
        return;
    }

    if (m_activeIndex != index) {
        m_activeIndex = index;
        emit activeViewportChanged(index);
    }
}

VtkViewportRenderer* MultiViewportManager::activeRenderer() {
    return getRenderer(m_activeIndex);
}

void MultiViewportManager::setImageData(int viewportIndex, vtkImageData* imageData, bool autoWindowLevel) {
    if (auto renderer = getRenderer(viewportIndex)) {
        renderer->setImageData(imageData, autoWindowLevel);
    }
}

void MultiViewportManager::clearViewport(int viewportIndex) {
    if (auto renderer = getRenderer(viewportIndex)) {
        renderer->clearImage();
    }
}

void MultiViewportManager::clearAllViewports() {
    for (auto& renderer : m_renderers) {
        if (renderer) {
            renderer->clearImage();
        }
    }
}

void MultiViewportManager::setLinkedMode(bool linked) {
    m_linkedMode = linked;
}

bool MultiViewportManager::isLinkedMode() const {
    return m_linkedMode;
}

void MultiViewportManager::synchronizeWindowLevel() {
    auto active = activeRenderer();
    if (!active) return;

    double window, level;
    active->getWindowLevel(window, level);

    forEachVisibleViewport([&](VtkViewportRenderer* renderer, int index) {
        if (index != m_activeIndex && renderer->hasImage()) {
            renderer->setWindowLevel(window, level);
        }
    });
}

void MultiViewportManager::synchronizeZoom() {
    auto active = activeRenderer();
    if (!active) return;

    double zoom = active->getZoom();

    forEachVisibleViewport([&](VtkViewportRenderer* renderer, int index) {
        if (index != m_activeIndex && renderer->hasImage()) {
            renderer->setZoom(zoom);
        }
    });
}

void MultiViewportManager::synchronizePan() {
    auto active = activeRenderer();
    if (!active) return;

    double panX, panY;
    active->getPan(panX, panY);

    forEachVisibleViewport([&](VtkViewportRenderer* renderer, int index) {
        if (index != m_activeIndex && renderer->hasImage()) {
            double currentPanX, currentPanY;
            renderer->getPan(currentPanX, currentPanY);
            renderer->setPan(panX - currentPanX, panY - currentPanY);
        }
    });
}

void MultiViewportManager::forEachVisibleViewport(std::function<void(VtkViewportRenderer*, int)> operation) {
    int count = visibleViewportCount();
    for (int i = 0; i < count; ++i) {
        if (m_renderers[i]) {
            operation(m_renderers[i].get(), i);
        }
    }
}

void MultiViewportManager::forEachViewport(std::function<void(VtkViewportRenderer*, int)> operation) {
    for (int i = 0; i < MAX_VIEWPORTS; ++i) {
        if (m_renderers[i]) {
            operation(m_renderers[i].get(), i);
        }
    }
}

void MultiViewportManager::renderAll() {
    forEachVisibleViewport([](VtkViewportRenderer* renderer, int) {
        renderer->render();
    });
}

void MultiViewportManager::renderViewport(int index) {
    if (auto renderer = getRenderer(index)) {
        renderer->render();
    }
}

void MultiViewportManager::connectRendererSignals(int index) {
    if (!m_renderers[index]) return;

    connect(m_renderers[index].get(), &VtkViewportRenderer::windowLevelChanged,
            this, &MultiViewportManager::onViewportWindowLevelChanged);

    connect(m_renderers[index].get(), &VtkViewportRenderer::zoomChanged,
            this, &MultiViewportManager::onViewportZoomChanged);

    connect(m_renderers[index].get(), &VtkViewportRenderer::imageChanged,
            this, &MultiViewportManager::onViewportImageChanged);
}

void MultiViewportManager::onViewportWindowLevelChanged(double window, double level) {
    // Find which renderer emitted the signal
    auto* senderRenderer = qobject_cast<VtkViewportRenderer*>(sender());
    if (!senderRenderer) return;

    for (int i = 0; i < MAX_VIEWPORTS; ++i) {
        if (m_renderers[i].get() == senderRenderer) {
            emit viewportWindowLevelChanged(i, window, level);

            // If linked mode, synchronize to other viewports
            if (m_linkedMode && i == m_activeIndex) {
                synchronizeWindowLevel();
            }
            break;
        }
    }
}

void MultiViewportManager::onViewportZoomChanged(double factor) {
    auto* senderRenderer = qobject_cast<VtkViewportRenderer*>(sender());
    if (!senderRenderer) return;

    for (int i = 0; i < MAX_VIEWPORTS; ++i) {
        if (m_renderers[i].get() == senderRenderer) {
            emit viewportZoomChanged(i, factor);

            // If linked mode, synchronize to other viewports
            if (m_linkedMode && i == m_activeIndex) {
                synchronizeZoom();
            }
            break;
        }
    }
}

void MultiViewportManager::onViewportImageChanged() {
    auto* senderRenderer = qobject_cast<VtkViewportRenderer*>(sender());
    if (!senderRenderer) return;

    for (int i = 0; i < MAX_VIEWPORTS; ++i) {
        if (m_renderers[i].get() == senderRenderer) {
            emit viewportImageChanged(i);
            break;
        }
    }
}

} // namespace Etrek::ImageViewer::Rendering
