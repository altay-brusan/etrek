#include "RulerOverlayWidget.h"

#include <QPainter>
#include <QPainterPath>
#include <QPen>
#include <QFont>
#include <cmath>

namespace Etrek::ImageViewer::Widget {

RulerOverlayWidget::RulerOverlayWidget(QWidget* parent)
    : QWidget(parent)
{
    // Set up the widget as a transparent overlay
    setAttribute(Qt::WA_TranslucentBackground);
    setAttribute(Qt::WA_TransparentForMouseEvents);  // Let mouse events pass through
    setMouseTracking(false);
}

void RulerOverlayWidget::setImageToWidgetTransform(CoordinateTransform transform) {
    m_imageToWidget = std::move(transform);
}

void RulerOverlayWidget::setMeasurements(const QVector<MeasurementLine>& measurements) {
    m_measurements = measurements;
    update();
}

void RulerOverlayWidget::setCurrentMeasurement(const MeasurementLine& line) {
    m_currentLine = line;
    m_hasCurrentLine = true;
    update();
}

void RulerOverlayWidget::clearCurrentMeasurement() {
    m_hasCurrentLine = false;
    update();
}

void RulerOverlayWidget::setSelectedRuler(int rulerId) {
    if (m_selectedRulerId != rulerId) {
        m_selectedRulerId = rulerId;
        update();
    }
}

void RulerOverlayWidget::setHoveredHandle(int rulerId, bool isStartHandle) {
    if (m_hoveredRulerId != rulerId || m_hoveredIsStart != isStartHandle) {
        m_hoveredRulerId = rulerId;
        m_hoveredIsStart = isStartHandle;
        update();
    }
}

void RulerOverlayWidget::clearHoveredHandle() {
    if (m_hoveredRulerId != -1) {
        m_hoveredRulerId = -1;
        update();
    }
}

void RulerOverlayWidget::refresh() {
    update();
}

QPointF RulerOverlayWidget::transformToWidget(const QPointF& imagePos) const {
    if (m_imageToWidget) {
        return m_imageToWidget(imagePos);
    }
    return imagePos;  // Fallback: no transform
}

void RulerOverlayWidget::paintEvent(QPaintEvent* event) {
    Q_UNUSED(event)

    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    // Draw completed measurements
    for (const auto& line : m_measurements) {
        bool isSelected = (line.id == m_selectedRulerId);
        drawRulerLine(painter, line, isSelected, false);
    }

    // Draw current in-progress measurement
    if (m_hasCurrentLine) {
        drawRulerLine(painter, m_currentLine, false, true);
    }
}

void RulerOverlayWidget::drawRulerLine(QPainter& painter, const MeasurementLine& line,
                                        bool isSelected, bool isCurrent) {
    // Transform image coordinates to widget coordinates
    QPointF startWidget = transformToWidget(line.startPoint);
    QPointF endWidget = transformToWidget(line.endPoint);

    // Check if both points are valid (visible)
    if (startWidget.x() < -1000 || endWidget.x() < -1000) {
        return;  // Points are not visible
    }

    // Choose colors based on state
    QColor lineColor;
    if (isCurrent) {
        lineColor = QColor(0, 191, 255);  // Cyan for drawing
    } else if (isSelected) {
        lineColor = QColor(255, 215, 0);  // Gold for selected
    } else {
        lineColor = QColor(0, 191, 255);  // Cyan for normal
    }

    // Draw the line
    QPen linePen(lineColor, LINE_WIDTH);
    linePen.setCapStyle(Qt::RoundCap);
    painter.setPen(linePen);
    painter.drawLine(startWidget, endWidget);

    // Draw handles at endpoints
    bool startHovered = (line.id == m_hoveredRulerId && m_hoveredIsStart);
    bool endHovered = (line.id == m_hoveredRulerId && !m_hoveredIsStart);

    drawHandle(painter, startWidget, startHovered, isSelected);
    drawHandle(painter, endWidget, endHovered, isSelected);

    // Draw distance label
    if (line.distancePixels > 5.0) {  // Only show label if line has some length
        drawDistanceLabel(painter, line, startWidget, endWidget);
    }
}

void RulerOverlayWidget::drawHandle(QPainter& painter, const QPointF& widgetPos,
                                     bool isHovered, bool isSelected) {
    // Handle fill color
    QColor fillColor;
    if (isHovered) {
        fillColor = QColor(255, 255, 0);  // Yellow when hovered
    } else if (isSelected) {
        fillColor = QColor(255, 215, 0);  // Gold when selected
    } else {
        fillColor = QColor(255, 255, 255);  // White normally
    }

    // Handle border color
    QColor borderColor = QColor(0, 191, 255);  // Cyan border

    // Draw filled square with border
    painter.setPen(QPen(borderColor, 2));
    painter.setBrush(fillColor);
    QRectF handleRect(widgetPos.x() - HANDLE_RADIUS, widgetPos.y() - HANDLE_RADIUS,
                      HANDLE_RADIUS * 2, HANDLE_RADIUS * 2);
    painter.drawRect(handleRect);
}

void RulerOverlayWidget::drawDistanceLabel(QPainter& painter, const MeasurementLine& line,
                                            const QPointF& startWidget, const QPointF& endWidget) {
    // Calculate midpoint
    QPointF midpoint((startWidget.x() + endWidget.x()) / 2.0,
                     (startWidget.y() + endWidget.y()) / 2.0);

    // Format the distance string: "25.4 (mm)"
    QString distanceText = QString("%1 (mm)").arg(line.distanceMm, 0, 'f', 1);

    // Set up font
    QFont font("Arial", LABEL_FONT_SIZE, QFont::Bold);
    painter.setFont(font);

    // Calculate text rectangle
    QFontMetrics fm(font);
    QRect textRect = fm.boundingRect(distanceText);

    // Position label slightly above the line midpoint
    // Calculate perpendicular offset
    double dx = endWidget.x() - startWidget.x();
    double dy = endWidget.y() - startWidget.y();
    double length = std::sqrt(dx * dx + dy * dy);

    if (length > 0.001) {
        // Perpendicular unit vector (rotated 90 degrees)
        double perpX = -dy / length;
        double perpY = dx / length;

        // Offset the label perpendicular to the line
        double offset = 15.0;  // pixels
        midpoint.setX(midpoint.x() + perpX * offset);
        midpoint.setY(midpoint.y() + perpY * offset);
    }

    // Center the text on the midpoint
    QPointF textPos(midpoint.x() - textRect.width() / 2.0,
                    midpoint.y() + textRect.height() / 4.0);

    // Draw text outline (black) for visibility
    QPainterPath path;
    path.addText(textPos, font, distanceText);

    painter.setPen(QPen(Qt::black, 3, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));
    painter.setBrush(Qt::NoBrush);
    painter.drawPath(path);

    // Draw text fill (white)
    painter.setPen(Qt::NoPen);
    painter.setBrush(Qt::white);
    painter.drawPath(path);
}

} // namespace Etrek::ImageViewer::Widget
