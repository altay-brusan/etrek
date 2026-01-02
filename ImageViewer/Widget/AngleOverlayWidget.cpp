#include "AngleOverlayWidget.h"

#include <QPainter>
#include <QPainterPath>
#include <QPen>
#include <QFont>
#include <cmath>

namespace Etrek::ImageViewer::Widget {

namespace {
    constexpr double PI = 3.14159265358979323846;
}

AngleOverlayWidget::AngleOverlayWidget(QWidget* parent)
    : QWidget(parent)
{
    // Set up the widget as a transparent overlay
    setAttribute(Qt::WA_TranslucentBackground);
    setAttribute(Qt::WA_TransparentForMouseEvents);  // Let mouse events pass through
    setMouseTracking(false);
}

void AngleOverlayWidget::setImageToWidgetTransform(CoordinateTransform transform) {
    m_imageToWidget = std::move(transform);
}

void AngleOverlayWidget::setMeasurements(const QVector<MeasurementAngle>& measurements) {
    m_measurements = measurements;
    update();
}

void AngleOverlayWidget::setCurrentMeasurement(const MeasurementAngle& angle) {
    m_currentAngle = angle;
    m_hasCurrentAngle = true;
    update();
}

void AngleOverlayWidget::clearCurrentMeasurement() {
    m_hasCurrentAngle = false;
    update();
}

void AngleOverlayWidget::setSelectedAngle(int angleId) {
    if (m_selectedAngleId != angleId) {
        m_selectedAngleId = angleId;
        update();
    }
}

void AngleOverlayWidget::setHoveredHandle(int angleId, AngleHandleType handleType) {
    if (m_hoveredAngleId != angleId || m_hoveredHandleType != handleType) {
        m_hoveredAngleId = angleId;
        m_hoveredHandleType = handleType;
        update();
    }
}

void AngleOverlayWidget::clearHoveredHandle() {
    if (m_hoveredAngleId != -1) {
        m_hoveredAngleId = -1;
        m_hoveredHandleType = AngleHandleType::NONE;
        update();
    }
}

void AngleOverlayWidget::refresh() {
    update();
}

QPointF AngleOverlayWidget::transformToWidget(const QPointF& imagePos) const {
    if (m_imageToWidget) {
        return m_imageToWidget(imagePos);
    }
    return imagePos;  // Fallback: no transform
}

void AngleOverlayWidget::paintEvent(QPaintEvent* event) {
    Q_UNUSED(event)

    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    // Draw completed measurements
    for (const auto& angle : m_measurements) {
        bool isSelected = (angle.id == m_selectedAngleId);
        drawAngle(painter, angle, isSelected, false);
    }

    // Draw current in-progress measurement
    if (m_hasCurrentAngle) {
        drawAngle(painter, m_currentAngle, false, true);
    }
}

void AngleOverlayWidget::drawAngle(QPainter& painter, const MeasurementAngle& angle,
                                    bool isSelected, bool isCurrent) {
    // Transform image coordinates to widget coordinates
    QPointF point1Widget = transformToWidget(angle.point1);
    QPointF vertexWidget = transformToWidget(angle.vertex);
    QPointF point2Widget = transformToWidget(angle.point2);

    // Check if points are valid (visible)
    if (vertexWidget.x() < -1000 || point1Widget.x() < -1000 || point2Widget.x() < -1000) {
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

    // Draw the two arm lines
    QPen linePen(lineColor, LINE_WIDTH);
    linePen.setCapStyle(Qt::RoundCap);
    painter.setPen(linePen);

    // Arm 1: vertex to point1
    painter.drawLine(vertexWidget, point1Widget);
    // Arm 2: vertex to point2
    painter.drawLine(vertexWidget, point2Widget);

    // Draw arc if angle is being formed (at least vertex placed)
    if (angle.isComplete ||
        (isCurrent && (point1Widget != vertexWidget && point2Widget != vertexWidget))) {
        drawArc(painter, angle, vertexWidget, point1Widget, point2Widget);
    }

    // Draw handles at all three points
    bool point1Hovered = (angle.id == m_hoveredAngleId && m_hoveredHandleType == AngleHandleType::POINT1);
    bool vertexHovered = (angle.id == m_hoveredAngleId && m_hoveredHandleType == AngleHandleType::VERTEX);
    bool point2Hovered = (angle.id == m_hoveredAngleId && m_hoveredHandleType == AngleHandleType::POINT2);

    drawHandle(painter, point1Widget, point1Hovered, isSelected);
    drawHandle(painter, vertexWidget, vertexHovered, isSelected);
    drawHandle(painter, point2Widget, point2Hovered, isSelected);

    // Draw angle label if complete or being formed
    if (angle.angleDegrees > 0.1) {
        drawAngleLabel(painter, angle, vertexWidget);
    }
}

void AngleOverlayWidget::drawHandle(QPainter& painter, const QPointF& widgetPos,
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

void AngleOverlayWidget::drawArc(QPainter& painter, const MeasurementAngle& angle,
                                  const QPointF& vertexWidget, const QPointF& point1Widget,
                                  const QPointF& point2Widget) {
    // Calculate angles for both arms relative to positive X axis
    double angle1 = std::atan2(point1Widget.y() - vertexWidget.y(),
                               point1Widget.x() - vertexWidget.x());
    double angle2 = std::atan2(point2Widget.y() - vertexWidget.y(),
                               point2Widget.x() - vertexWidget.x());

    // Convert to degrees (Qt uses degrees for arc drawing)
    double angle1Deg = angle1 * 180.0 / PI;
    double angle2Deg = angle2 * 180.0 / PI;

    // Calculate sweep angle (the angle between the two arms)
    double sweepAngle = angle2Deg - angle1Deg;

    // Normalize sweep angle to be between -180 and 180
    while (sweepAngle > 180.0) sweepAngle -= 360.0;
    while (sweepAngle < -180.0) sweepAngle += 360.0;

    // Qt's arc drawing uses 1/16th degree units and starts from 3 o'clock position
    // going counter-clockwise for positive angles
    // We need to negate start angle because Qt's Y-axis is inverted from standard math
    int startAngle = static_cast<int>(-angle1Deg * 16);
    int spanAngle = static_cast<int>(-sweepAngle * 16);

    // Draw the arc
    QRectF arcRect(vertexWidget.x() - ARC_RADIUS, vertexWidget.y() - ARC_RADIUS,
                   ARC_RADIUS * 2, ARC_RADIUS * 2);

    QPen arcPen(QColor(0, 191, 255), 2);  // Cyan arc
    painter.setPen(arcPen);
    painter.setBrush(Qt::NoBrush);
    painter.drawArc(arcRect, startAngle, spanAngle);
}

void AngleOverlayWidget::drawAngleLabel(QPainter& painter, const MeasurementAngle& angle,
                                         const QPointF& vertexWidget) {
    // Format the angle string: "45.2 deg"
    QString angleText = QString::fromUtf8("%1\u00B0").arg(angle.angleDegrees, 0, 'f', 1);

    // Set up font
    QFont font("Arial", LABEL_FONT_SIZE, QFont::Bold);
    painter.setFont(font);

    // Calculate text rectangle
    QFontMetrics fm(font);
    QRect textRect = fm.boundingRect(angleText);

    // Position label near the arc (offset from vertex)
    // Use the bisector of the angle to place the label
    QPointF point1Widget = transformToWidget(angle.point1);
    QPointF point2Widget = transformToWidget(angle.point2);

    double angle1 = std::atan2(point1Widget.y() - vertexWidget.y(),
                               point1Widget.x() - vertexWidget.x());
    double angle2 = std::atan2(point2Widget.y() - vertexWidget.y(),
                               point2Widget.x() - vertexWidget.x());

    // Calculate bisector angle
    double bisector = (angle1 + angle2) / 2.0;

    // Check if we need to flip the bisector (when angles are on opposite sides of 0)
    double angleDiff = angle2 - angle1;
    while (angleDiff > PI) angleDiff -= 2 * PI;
    while (angleDiff < -PI) angleDiff += 2 * PI;

    if (std::abs(angleDiff) > PI) {
        bisector += PI;
    }

    // Position label along the bisector, outside the arc
    double labelDistance = ARC_RADIUS + 20;  // Place label outside the arc
    QPointF labelPos(vertexWidget.x() + labelDistance * std::cos(bisector),
                     vertexWidget.y() + labelDistance * std::sin(bisector));

    // Center the text on the label position
    QPointF textPos(labelPos.x() - textRect.width() / 2.0,
                    labelPos.y() + textRect.height() / 4.0);

    // Draw text outline (black) for visibility
    QPainterPath path;
    path.addText(textPos, font, angleText);

    painter.setPen(QPen(Qt::black, 3, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));
    painter.setBrush(Qt::NoBrush);
    painter.drawPath(path);

    // Draw text fill (white)
    painter.setPen(Qt::NoPen);
    painter.setBrush(Qt::white);
    painter.drawPath(path);
}

} // namespace Etrek::ImageViewer::Widget
