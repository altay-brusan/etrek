#include "RulerTool.h"

#include <QIcon>

namespace Etrek::ImageViewer::Tool {

RulerTool::RulerTool(QObject* parent)
    : IImageTool(parent)
{
}

QIcon RulerTool::icon() const {
    return QIcon(":/Images/Asset/Icon/ruler.png");
}

void RulerTool::activate() {
    m_active = true;
    emit cursorChanged(Qt::CrossCursor);
}

void RulerTool::deactivate() {
    m_active = false;
    m_drawing = false;
    m_currentLine = MeasurementLine();
}

void RulerTool::onMousePress(const QPointF& pos, Qt::MouseButton button) {
    if (!m_active) return;

    if (button == Qt::LeftButton) {
        m_drawing = true;
        m_currentLine = MeasurementLine();
        m_currentLine.startPoint = pos;
        m_currentLine.endPoint = pos;
        m_currentLine.isComplete = false;

        emit measurementLineUpdated(m_currentLine);
    }
}

void RulerTool::onMouseMove(const QPointF& pos) {
    if (!m_active || !m_drawing) return;

    m_currentLine.endPoint = pos;
    m_currentLine.calculateDistance(m_pixelSpacingX, m_pixelSpacingY);

    emit measurementLineUpdated(m_currentLine);
}

void RulerTool::onMouseRelease(const QPointF& pos, Qt::MouseButton button) {
    if (!m_active) return;

    if (button == Qt::LeftButton && m_drawing) {
        m_drawing = false;
        m_currentLine.endPoint = pos;
        m_currentLine.calculateDistance(m_pixelSpacingX, m_pixelSpacingY);
        m_currentLine.isComplete = true;

        // Only save measurements with some length
        if (m_currentLine.distancePixels > 2.0) {
            m_measurements.append(m_currentLine);
            emit measurementLineCompleted(m_currentLine);
        }

        m_currentLine = MeasurementLine();
    }
}

void RulerTool::onMouseWheel(int delta, const QPointF& pos) {
    Q_UNUSED(delta)
    Q_UNUSED(pos)
    // Ruler tool doesn't use wheel
}

void RulerTool::reset() {
    m_drawing = false;
    m_currentLine = MeasurementLine();
}

void RulerTool::setPixelSpacing(double spacingX, double spacingY) {
    m_pixelSpacingX = spacingX;
    m_pixelSpacingY = spacingY;
}

void RulerTool::clearMeasurements() {
    m_measurements.clear();
    m_currentLine = MeasurementLine();
    m_drawing = false;
}

} // namespace Etrek::ImageViewer::Tool
