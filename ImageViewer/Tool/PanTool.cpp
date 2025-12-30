#include "PanTool.h"

#include <QIcon>

namespace Etrek::ImageViewer::Tool {

PanTool::PanTool(QObject* parent)
    : IImageTool(parent)
{
}

QIcon PanTool::icon() const {
    return QIcon(":/Images/Asset/Icon/pan.png");
}

void PanTool::activate() {
    m_active = true;
    emit cursorChanged(Qt::OpenHandCursor);
}

void PanTool::deactivate() {
    m_active = false;
    m_dragging = false;
}

void PanTool::onMousePress(const QPointF& pos, Qt::MouseButton button) {
    if (!m_active) return;

    if (button == Qt::LeftButton || button == Qt::MiddleButton) {
        m_dragging = true;
        m_lastPos = pos;
        emit cursorChanged(Qt::ClosedHandCursor);
    }
}

void PanTool::onMouseMove(const QPointF& pos) {
    if (!m_active || !m_dragging) return;

    double deltaX = (pos.x() - m_lastPos.x()) * m_sensitivity;
    double deltaY = (pos.y() - m_lastPos.y()) * m_sensitivity;

    emit panRequested(deltaX, deltaY);

    m_lastPos = pos;
}

void PanTool::onMouseRelease(const QPointF& pos, Qt::MouseButton button) {
    Q_UNUSED(pos)
    if (button == Qt::LeftButton || button == Qt::MiddleButton) {
        m_dragging = false;
        emit cursorChanged(Qt::OpenHandCursor);
    }
}

void PanTool::onMouseWheel(int delta, const QPointF& pos) {
    Q_UNUSED(delta)
    Q_UNUSED(pos)
    // Pan tool doesn't use wheel - could be used for scroll
}

void PanTool::reset() {
    m_dragging = false;
    m_lastPos = QPointF();
}

void PanTool::setSensitivity(double sensitivity) {
    m_sensitivity = sensitivity;
}

} // namespace Etrek::ImageViewer::Tool
