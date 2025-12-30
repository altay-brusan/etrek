#include "AngleTool.h"

#include <QIcon>

namespace Etrek::ImageViewer::Tool {

AngleTool::AngleTool(QObject* parent)
    : IImageTool(parent)
{
}

QIcon AngleTool::icon() const {
    return QIcon(":/Images/Asset/Icon/angle.png");
}

void AngleTool::activate() {
    m_active = true;
    emit cursorChanged(Qt::CrossCursor);
}

void AngleTool::deactivate() {
    m_active = false;
    m_pointsPlaced = 0;
    m_currentAngle = MeasurementAngle();
}

void AngleTool::onMousePress(const QPointF& pos, Qt::MouseButton button) {
    if (!m_active) return;

    if (button == Qt::LeftButton) {
        switch (m_pointsPlaced) {
            case 0:
                // First point
                m_currentAngle = MeasurementAngle();
                m_currentAngle.point1 = pos;
                m_currentAngle.vertex = pos;
                m_currentAngle.point2 = pos;
                m_currentAngle.isComplete = false;
                m_pointsPlaced = 1;
                break;

            case 1:
                // Vertex (middle point)
                m_currentAngle.vertex = pos;
                m_currentAngle.point2 = pos;
                m_pointsPlaced = 2;
                break;

            case 2:
                // Third point - complete the angle
                m_currentAngle.point2 = pos;
                m_currentAngle.calculateAngle();
                m_currentAngle.isComplete = true;
                m_measurements.append(m_currentAngle);
                emit measurementAngleCompleted(m_currentAngle);

                // Reset for next measurement
                m_currentAngle = MeasurementAngle();
                m_pointsPlaced = 0;
                break;
        }

        emit measurementAngleUpdated(m_currentAngle);
    } else if (button == Qt::RightButton) {
        // Cancel current measurement
        m_currentAngle = MeasurementAngle();
        m_pointsPlaced = 0;
        emit measurementAngleUpdated(m_currentAngle);
    }
}

void AngleTool::onMouseMove(const QPointF& pos) {
    if (!m_active || m_pointsPlaced == 0) return;

    // Update the current point being placed
    if (m_pointsPlaced == 1) {
        m_currentAngle.vertex = pos;
        m_currentAngle.point2 = pos;
    } else if (m_pointsPlaced == 2) {
        m_currentAngle.point2 = pos;
        m_currentAngle.calculateAngle();
    }

    emit measurementAngleUpdated(m_currentAngle);
}

void AngleTool::onMouseRelease(const QPointF& pos, Qt::MouseButton button) {
    Q_UNUSED(pos)
    Q_UNUSED(button)
    // Angle tool uses click-to-place, not drag
}

void AngleTool::onMouseWheel(int delta, const QPointF& pos) {
    Q_UNUSED(delta)
    Q_UNUSED(pos)
    // Angle tool doesn't use wheel
}

void AngleTool::reset() {
    m_pointsPlaced = 0;
    m_currentAngle = MeasurementAngle();
}

void AngleTool::clearMeasurements() {
    m_measurements.clear();
    m_currentAngle = MeasurementAngle();
    m_pointsPlaced = 0;
}

} // namespace Etrek::ImageViewer::Tool
