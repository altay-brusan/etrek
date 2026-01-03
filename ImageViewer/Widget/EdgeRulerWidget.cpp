#include "EdgeRulerWidget.h"

#include <QPainter>
#include <QPen>
#include <QFont>
#include <cmath>
#include <algorithm>

namespace Etrek::ImageViewer::Widget {

EdgeRulerWidget::EdgeRulerWidget(QWidget* parent)
    : QWidget(parent)
{
    // Set up the widget as a transparent overlay
    setAttribute(Qt::WA_TranslucentBackground);
    setAttribute(Qt::WA_TransparentForMouseEvents);
    setMouseTracking(false);
}

void EdgeRulerWidget::setPixelSpacing(double spacingX, double spacingY) {
    m_pixelSpacingX = spacingX;
    m_pixelSpacingY = spacingY;
    update();
}

void EdgeRulerWidget::setImageDimensions(int width, int height) {
    m_imageWidth = width;
    m_imageHeight = height;
    update();
}

void EdgeRulerWidget::setZoomLevel(double zoom) {
    m_zoomLevel = zoom;
    update();
}

void EdgeRulerWidget::setImageToWidgetTransform(CoordinateTransform transform) {
    m_imageToWidget = std::move(transform);
    update();
}

void EdgeRulerWidget::setShowHorizontalRuler(bool show) {
    m_showHorizontal = show;
    update();
}

void EdgeRulerWidget::setShowVerticalRuler(bool show) {
    m_showVertical = show;
    update();
}

void EdgeRulerWidget::setRulerColor(const QColor& color) {
    m_rulerColor = color;
    update();
}

void EdgeRulerWidget::setRulerWidth(int rulerWidth) {
    m_rulerWidth = rulerWidth;
    update();
}

QPointF EdgeRulerWidget::transformToWidget(const QPointF& imagePos) const {
    if (m_imageToWidget) {
        return m_imageToWidget(imagePos);
    }
    return imagePos;
}

double EdgeRulerWidget::calculateTickInterval() const {
    // Calculate the display size of 1mm at current zoom
    double mmPerDisplayPixel = m_pixelSpacingY / m_zoomLevel;
    double pixelsPerMm = 1.0 / mmPerDisplayPixel;

    // Choose tick interval so ticks are at least 3 pixels apart
    if (pixelsPerMm >= 3.0) {
        return 1.0;   // 1mm ticks
    } else if (pixelsPerMm >= 0.6) {
        return 5.0;   // 5mm ticks
    } else if (pixelsPerMm >= 0.3) {
        return 10.0;  // 1cm ticks
    } else {
        return 50.0;  // 5cm ticks
    }
}

void EdgeRulerWidget::paintEvent(QPaintEvent* event) {
    Q_UNUSED(event)

    if (!m_imageToWidget || m_imageWidth <= 0 || m_imageHeight <= 0) {
        return;
    }

    if (m_pixelSpacingX <= 0 || m_pixelSpacingY <= 0) {
        return;
    }

    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    if (m_showVertical) {
        drawVerticalRuler(painter);
    }

    if (m_showHorizontal) {
        drawHorizontalRuler(painter);
    }
}

void EdgeRulerWidget::drawVerticalRuler(QPainter& painter) {
    // Get the image corners in widget coordinates
    QPointF imageTopRight = transformToWidget(QPointF(m_imageWidth, 0));
    QPointF imageBottomRight = transformToWidget(QPointF(m_imageWidth, m_imageHeight));
    QPointF imageTopLeft = transformToWidget(QPointF(0, 0));

    // Check if the right edge of the image is visible
    if (imageTopRight.x() < 0 || imageTopRight.x() > width()) {
        return;  // Image right edge not visible in widget
    }

    // Calculate the visible portion of the image (clamp to widget bounds)
    int imageTop = static_cast<int>(std::max(0.0, imageTopRight.y()));
    int imageBottom = static_cast<int>(std::min(static_cast<double>(height()), imageBottomRight.y()));

    if (imageTop >= imageBottom) {
        return;  // No visible vertical range
    }

    // Ruler X position - at the right edge of the image
    int rulerX = static_cast<int>(imageTopRight.x());

    // Draw ruler background - only within image bounds
    QRect rulerRect(rulerX, imageTop, m_rulerWidth, imageBottom - imageTop);
    painter.fillRect(rulerRect, QColor(0, 0, 0, 180));

    // Draw ruler left border (the line along image edge)
    painter.setPen(QPen(m_rulerColor, 1));
    painter.drawLine(rulerX, imageTop, rulerX, imageBottom);

    // Calculate physical dimensions
    double imageSizeMm = m_imageHeight * m_pixelSpacingY;

    // Determine tick interval based on zoom
    double tickInterval = calculateTickInterval();
    double majorInterval = (tickInterval <= 5.0) ? 10.0 : 50.0;  // Major ticks at cm or 5cm

    // Set up font for labels
    QFont font("Arial", FONT_SIZE);
    painter.setFont(font);
    QFontMetrics fm(font);

    // Draw "cm" label at top of ruler (if space available)
    if (imageTop < height() - 20) {
        painter.setPen(m_rulerColor);
        QString unitLabel = "cm";
        int unitX = rulerX + (m_rulerWidth - fm.horizontalAdvance(unitLabel)) / 2;
        int unitY = imageTop + fm.height() + 2;
        painter.drawText(unitX, unitY, unitLabel);
    }

    // Calculate total number of ticks (use integer math to avoid floating point issues)
    int tickIntervalUm = static_cast<int>(tickInterval * 1000);  // Convert to micrometers
    int majorIntervalUm = static_cast<int>(majorInterval * 1000);
    int totalUm = static_cast<int>(imageSizeMm * 1000);

    // Draw ticks from 0 to image height
    for (int um = 0; um <= totalUm; um += tickIntervalUm) {
        double mm = um / 1000.0;

        // Convert mm position to image pixel position
        double imageY = mm / m_pixelSpacingY;

        // Transform to widget coordinates
        QPointF widgetPos = transformToWidget(QPointF(m_imageWidth, imageY));

        // Only draw ticks within the image bounds (in widget space)
        if (widgetPos.y() < imageTop || widgetPos.y() > imageBottom) {
            continue;
        }

        int y = static_cast<int>(widgetPos.y());
        int tickLength;
        bool drawLabel = false;

        // Determine tick type using integer modulo (no floating point errors)
        if (um % majorIntervalUm == 0) {
            // Major tick (cm)
            tickLength = MAJOR_TICK_LENGTH;
            drawLabel = (um > 0);  // Don't label 0
        } else if (um % 5000 == 0) {
            // Medium tick (5mm = 5000um)
            tickLength = MEDIUM_TICK_LENGTH;
        } else {
            // Minor tick (1mm)
            tickLength = MINOR_TICK_LENGTH;
        }

        // Draw tick mark
        painter.setPen(QPen(m_rulerColor, 1));
        painter.drawLine(rulerX, y, rulerX + tickLength, y);

        // Draw label at major ticks
        if (drawLabel) {
            int cm = um / 10000;  // 10000um = 1cm
            QString label = QString::number(cm);

            int textY = y + fm.ascent() / 2 - 1;
            int textX = rulerX + tickLength + LABEL_MARGIN;

            // Draw text with shadow for visibility
            painter.setPen(QColor(0, 0, 0));
            painter.drawText(textX + 1, textY + 1, label);
            painter.setPen(m_rulerColor);
            painter.drawText(textX, textY, label);
        }
    }
}

void EdgeRulerWidget::drawHorizontalRuler(QPainter& painter) {
    // Get the image corners in widget coordinates
    QPointF imageBottomLeft = transformToWidget(QPointF(0, m_imageHeight));
    QPointF imageBottomRight = transformToWidget(QPointF(m_imageWidth, m_imageHeight));

    // Check if the bottom edge of the image is visible
    if (imageBottomLeft.y() < 0 || imageBottomLeft.y() > height()) {
        return;  // Image bottom edge not visible in widget
    }

    // Calculate the visible portion of the image (clamp to widget bounds)
    int imageLeft = static_cast<int>(std::max(0.0, imageBottomLeft.x()));
    int imageRight = static_cast<int>(std::min(static_cast<double>(width()), imageBottomRight.x()));

    if (imageLeft >= imageRight) {
        return;  // No visible horizontal range
    }

    // Ruler Y position - at the bottom edge of the image
    int rulerY = static_cast<int>(imageBottomLeft.y());

    // Ensure ruler doesn't go beyond widget
    if (rulerY + m_rulerWidth > height()) {
        return;  // Not enough space for ruler
    }

    // Draw ruler background - only within image bounds
    QRect rulerRect(imageLeft, rulerY, imageRight - imageLeft, m_rulerWidth);
    painter.fillRect(rulerRect, QColor(0, 0, 0, 180));

    // Draw ruler top border (the line along image edge)
    painter.setPen(QPen(m_rulerColor, 1));
    painter.drawLine(imageLeft, rulerY, imageRight, rulerY);

    // Calculate physical dimensions
    double imageSizeMm = m_imageWidth * m_pixelSpacingX;

    // Determine tick interval based on zoom
    double tickInterval = calculateTickInterval();
    double majorInterval = (tickInterval <= 5.0) ? 10.0 : 50.0;

    // Set up font for labels
    QFont font("Arial", FONT_SIZE);
    painter.setFont(font);
    QFontMetrics fm(font);

    // Draw "cm" label at left of ruler (if space available)
    if (imageLeft < width() - 30) {
        painter.setPen(m_rulerColor);
        QString unitLabel = "cm";
        int unitX = imageLeft + 4;
        int unitY = rulerY + (m_rulerWidth + fm.ascent()) / 2;
        painter.drawText(unitX, unitY, unitLabel);
    }

    // Calculate total number of ticks (use integer math to avoid floating point issues)
    int tickIntervalUm = static_cast<int>(tickInterval * 1000);  // Convert to micrometers
    int majorIntervalUm = static_cast<int>(majorInterval * 1000);
    int totalUm = static_cast<int>(imageSizeMm * 1000);

    // Draw ticks from 0 to image width
    for (int um = 0; um <= totalUm; um += tickIntervalUm) {
        double mm = um / 1000.0;

        // Convert mm position to image pixel position
        double imageX = mm / m_pixelSpacingX;

        // Transform to widget coordinates
        QPointF widgetPos = transformToWidget(QPointF(imageX, m_imageHeight));

        // Only draw ticks within the image bounds (in widget space)
        if (widgetPos.x() < imageLeft || widgetPos.x() > imageRight) {
            continue;
        }

        int x = static_cast<int>(widgetPos.x());
        int tickLength;
        bool drawLabel = false;

        // Determine tick type using integer modulo (no floating point errors)
        if (um % majorIntervalUm == 0) {
            tickLength = MAJOR_TICK_LENGTH;
            drawLabel = (um > 0);  // Don't label 0
        } else if (um % 5000 == 0) {
            // Medium tick (5mm = 5000um)
            tickLength = MEDIUM_TICK_LENGTH;
        } else {
            tickLength = MINOR_TICK_LENGTH;
        }

        // Draw tick mark
        painter.setPen(QPen(m_rulerColor, 1));
        painter.drawLine(x, rulerY, x, rulerY + tickLength);

        // Draw label at major ticks
        if (drawLabel) {
            int cm = um / 10000;  // 10000um = 1cm
            QString label = QString::number(cm);

            int textWidth = fm.horizontalAdvance(label);
            int textX = x - textWidth / 2;
            int textY = rulerY + tickLength + LABEL_MARGIN + fm.ascent();

            // Draw text with shadow
            painter.setPen(QColor(0, 0, 0));
            painter.drawText(textX + 1, textY + 1, label);
            painter.setPen(m_rulerColor);
            painter.drawText(textX, textY, label);
        }
    }
}

} // namespace Etrek::ImageViewer::Widget
