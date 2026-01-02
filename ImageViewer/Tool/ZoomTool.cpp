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
    // Use a magnifying glass cursor
    emit cursorChanged(Qt::CrossCursor);
}

void ZoomTool::deactivate() {
    m_active = false;
    // Hide magnifier when tool is deactivated
    emit magnifierHideRequested();
}

void ZoomTool::onMousePress(const QPointF& pos, Qt::MouseButton button) {
    if (!m_active) return;

    if (button == Qt::LeftButton) {
        // Toggle magnification level on click
        emit magnifierToggleRequested();
        // Update magnifier position
        emit magnifierRequested(pos);
    }
}

void ZoomTool::onMouseMove(const QPointF& pos) {
    if (!m_active) return;

    // Update magnifier position as mouse moves
    m_lastPos = pos;
    emit magnifierRequested(pos);
}

void ZoomTool::onMouseRelease(const QPointF& pos, Qt::MouseButton button) {
    Q_UNUSED(pos)
    Q_UNUSED(button)
    // Nothing special on release
}

void ZoomTool::onMouseWheel(int delta, const QPointF& pos) {
    if (!m_active) return;

    Q_UNUSED(pos)
    // Toggle magnification on wheel scroll
    emit magnifierToggleRequested();
}

void ZoomTool::reset() {
    m_lastPos = QPointF();
}

void ZoomTool::setCurrentZoom(double zoom) {
    m_currentMagnification = std::clamp(zoom, m_minZoom, m_maxZoom);
}

void ZoomTool::setZoomLimits(double minZoom, double maxZoom) {
    m_minZoom = minZoom;
    m_maxZoom = maxZoom;
    m_currentMagnification = std::clamp(m_currentMagnification, m_minZoom, m_maxZoom);
}

void ZoomTool::setSensitivity(double sensitivity) {
    m_sensitivity = sensitivity;
}

} // namespace Etrek::ImageViewer::Tool
