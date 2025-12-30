#include "WindowLevelTool.h"

#include <QIcon>
#include <cmath>

namespace Etrek::ImageViewer::Tool {

WindowLevelTool::WindowLevelTool(QObject* parent)
    : IImageTool(parent)
{
}

QIcon WindowLevelTool::icon() const {
    return QIcon(":/Images/Asset/Icon/windowlevel.png");
}

void WindowLevelTool::activate() {
    m_active = true;
    emit cursorChanged(Qt::CrossCursor);
}

void WindowLevelTool::deactivate() {
    m_active = false;
    m_dragging = false;
}

void WindowLevelTool::onMousePress(const QPointF& pos, Qt::MouseButton button) {
    if (!m_active) return;

    if (button == Qt::LeftButton) {
        m_dragging = true;
        m_lastPos = pos;
    }
}

void WindowLevelTool::onMouseMove(const QPointF& pos) {
    if (!m_active || !m_dragging) return;

    double deltaX = pos.x() - m_lastPos.x();
    double deltaY = pos.y() - m_lastPos.y();

    // Horizontal movement adjusts window (contrast)
    // Vertical movement adjusts level (brightness)
    double newWindow = m_currentWindow + deltaX * m_sensitivity;
    double newLevel = m_currentLevel - deltaY * m_sensitivity;  // Inverted for intuitive feel

    // Clamp window to minimum of 1
    newWindow = std::max(1.0, newWindow);

    m_currentWindow = newWindow;
    m_currentLevel = newLevel;

    emit windowLevelChanged(m_currentWindow, m_currentLevel);

    m_lastPos = pos;
}

void WindowLevelTool::onMouseRelease(const QPointF& pos, Qt::MouseButton button) {
    Q_UNUSED(pos)
    if (button == Qt::LeftButton) {
        m_dragging = false;
    }
}

void WindowLevelTool::onMouseWheel(int delta, const QPointF& pos) {
    Q_UNUSED(pos)
    if (!m_active) return;

    // Wheel adjusts window width
    double adjustment = delta > 0 ? 10.0 : -10.0;
    m_currentWindow = std::max(1.0, m_currentWindow + adjustment * m_sensitivity);

    emit windowLevelChanged(m_currentWindow, m_currentLevel);
}

void WindowLevelTool::reset() {
    m_dragging = false;
    m_lastPos = QPointF();
}

void WindowLevelTool::setCurrentWindowLevel(double window, double level) {
    m_currentWindow = window;
    m_currentLevel = level;
}

void WindowLevelTool::setSensitivity(double sensitivity) {
    m_sensitivity = sensitivity;
}

} // namespace Etrek::ImageViewer::Tool
