#include "ResetTool.h"

#include <QIcon>

namespace Etrek::ImageViewer::Tool {

ResetTool::ResetTool(QObject* parent)
    : IImageTool(parent)
{
}

QIcon ResetTool::icon() const {
    return QIcon(":/Images/Asset/Icon/reset.png");
}

void ResetTool::activate() {
    m_active = true;
    // Reset is triggered immediately on activation
    emit resetRequested();
    emit cursorChanged(Qt::ArrowCursor);
}

void ResetTool::deactivate() {
    m_active = false;
}

void ResetTool::onMousePress(const QPointF& pos, Qt::MouseButton button) {
    Q_UNUSED(pos)
    if (m_active && button == Qt::LeftButton) {
        emit resetRequested();
    }
}

void ResetTool::onMouseMove(const QPointF& pos) {
    Q_UNUSED(pos)
    // Reset tool doesn't track mouse movement
}

void ResetTool::onMouseRelease(const QPointF& pos, Qt::MouseButton button) {
    Q_UNUSED(pos)
    Q_UNUSED(button)
}

void ResetTool::onMouseWheel(int delta, const QPointF& pos) {
    Q_UNUSED(delta)
    Q_UNUSED(pos)
}

void ResetTool::reset() {
    // Nothing to reset for this tool
}

} // namespace Etrek::ImageViewer::Tool
