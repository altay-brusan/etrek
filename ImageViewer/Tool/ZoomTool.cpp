#include "ZoomTool.h"

#include <QIcon>
#include <cmath>
#include <algorithm>

namespace Etrek::ImageViewer::Tool {

ZoomTool::ZoomTool(QObject* parent)
    : IImageTool(parent)
{
}

QIcon ZoomTool::icon() const {
    return QIcon(":/Images/Asset/Icon/zoom.png");
}

void ZoomTool::activate() {
    m_active = true;
    emit cursorChanged(Qt::SizeBDiagCursor);
}

void ZoomTool::deactivate() {
    m_active = false;
    m_dragging = false;
}

void ZoomTool::onMousePress(const QPointF& pos, Qt::MouseButton button) {
    if (!m_active) return;

    if (button == Qt::LeftButton) {
        m_dragging = true;
        m_lastPos = pos;
        m_dragStartPos = pos;
    }
}

void ZoomTool::onMouseMove(const QPointF& pos) {
    if (!m_active || !m_dragging) return;

    // Vertical drag controls zoom
    double deltaY = m_lastPos.y() - pos.y();  // Inverted: drag up = zoom in

    double zoomDelta = deltaY * m_sensitivity;
    double newZoom = m_currentZoom * (1.0 + zoomDelta);

    // Clamp to limits
    newZoom = std::clamp(newZoom, m_minZoom, m_maxZoom);

    if (std::abs(newZoom - m_currentZoom) > 0.001) {
        m_currentZoom = newZoom;
        emit zoomRequested(m_currentZoom, m_dragStartPos);
    }

    m_lastPos = pos;
}

void ZoomTool::onMouseRelease(const QPointF& pos, Qt::MouseButton button) {
    Q_UNUSED(pos)
    if (button == Qt::LeftButton) {
        m_dragging = false;
    }
}

void ZoomTool::onMouseWheel(int delta, const QPointF& pos) {
    if (!m_active) return;

    // Calculate zoom factor based on wheel delta
    double zoomFactor = delta > 0 ? 1.1 : 0.9;
    double newZoom = m_currentZoom * zoomFactor;

    // Clamp to limits
    newZoom = std::clamp(newZoom, m_minZoom, m_maxZoom);

    if (std::abs(newZoom - m_currentZoom) > 0.001) {
        m_currentZoom = newZoom;
        emit zoomRequested(m_currentZoom, pos);
    }
}

void ZoomTool::reset() {
    m_dragging = false;
    m_lastPos = QPointF();
    m_dragStartPos = QPointF();
}

void ZoomTool::setCurrentZoom(double zoom) {
    m_currentZoom = std::clamp(zoom, m_minZoom, m_maxZoom);
}

void ZoomTool::setZoomLimits(double minZoom, double maxZoom) {
    m_minZoom = minZoom;
    m_maxZoom = maxZoom;
    m_currentZoom = std::clamp(m_currentZoom, m_minZoom, m_maxZoom);
}

void ZoomTool::setSensitivity(double sensitivity) {
    m_sensitivity = sensitivity;
}

} // namespace Etrek::ImageViewer::Tool
