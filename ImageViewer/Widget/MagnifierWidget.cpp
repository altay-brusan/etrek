#include "MagnifierWidget.h"

#include <QPainter>
#include <QPainterPath>
#include <QPen>

namespace Etrek::ImageViewer::Widget {

MagnifierWidget::MagnifierWidget(QWidget* parent)
    : QWidget(parent)
{
    // Set up the widget as a transparent overlay that doesn't intercept mouse events
    setAttribute(Qt::WA_TranslucentBackground);
    setAttribute(Qt::WA_TransparentForMouseEvents);  // Let mouse events pass through
    setMouseTracking(false);

    // Set fixed size
    setFixedSize(m_size, m_size);

    hide();
}

void MagnifierWidget::setMagnification(double factor) {
    if (factor != m_magnification) {
        m_magnification = qBound(1.5, factor, 5.0);
        updateMagnifiedRegion();
        update();
        emit magnificationChanged(m_magnification);
    }
}

void MagnifierWidget::toggleMagnification() {
    // Toggle between 2x and 3x
    if (m_magnification < 2.5) {
        setMagnification(3.0);
    } else {
        setMagnification(2.0);
    }
}

void MagnifierWidget::setMagnifierSize(int size) {
    m_size = qBound(100, size, 300);
    setFixedSize(m_size, m_size);
    updateMagnifiedRegion();
    update();
}

void MagnifierWidget::updateMagnifier(const QImage& sourceImage, const QPointF& cursorPos, const QPoint& widgetPos) {
    m_sourceImage = sourceImage;
    m_cursorPos = cursorPos;

    updateMagnifiedRegion();

    // Center the magnifier on the cursor position
    QPoint newPos = widgetPos - QPoint(m_size / 2, m_size / 2);

    move(newPos);
    update();
}

void MagnifierWidget::showAt(const QPoint& pos) {
    move(pos);
    show();
    raise();
}

void MagnifierWidget::hideMagnifier() {
    hide();
}

void MagnifierWidget::updateMagnifiedRegion() {
    if (m_sourceImage.isNull()) {
        m_magnifiedImage = QImage();
        return;
    }

    // Calculate the source region to capture
    // The region size is magnifier size / magnification factor
    int sourceSize = static_cast<int>(m_size / m_magnification);

    // Calculate source rectangle centered on cursor position
    int sourceX = static_cast<int>(m_cursorPos.x()) - sourceSize / 2;
    int sourceY = static_cast<int>(m_cursorPos.y()) - sourceSize / 2;

    // Clamp to image bounds
    sourceX = qBound(0, sourceX, m_sourceImage.width() - sourceSize);
    sourceY = qBound(0, sourceY, m_sourceImage.height() - sourceSize);

    // Handle edge cases where source size is larger than image
    if (sourceSize > m_sourceImage.width()) {
        sourceSize = m_sourceImage.width();
        sourceX = 0;
    }
    if (sourceSize > m_sourceImage.height()) {
        sourceSize = m_sourceImage.height();
        sourceY = 0;
    }

    if (sourceSize <= 0) {
        m_magnifiedImage = QImage();
        return;
    }

    // Extract and scale the region
    QRect sourceRect(sourceX, sourceY, sourceSize, sourceSize);
    QImage region = m_sourceImage.copy(sourceRect);

    // Scale up to magnifier size
    m_magnifiedImage = region.scaled(m_size, m_size, Qt::KeepAspectRatio, Qt::SmoothTransformation);
}

void MagnifierWidget::paintEvent(QPaintEvent* event) {
    Q_UNUSED(event)

    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    // Create circular clip path if using circular magnifier
    if (m_circular) {
        QPainterPath clipPath;
        clipPath.addEllipse(rect().adjusted(2, 2, -2, -2));
        painter.setClipPath(clipPath);
    }

    // Draw the magnified image
    if (!m_magnifiedImage.isNull()) {
        // Center the image in the widget
        int x = (width() - m_magnifiedImage.width()) / 2;
        int y = (height() - m_magnifiedImage.height()) / 2;
        painter.drawImage(x, y, m_magnifiedImage);
    } else {
        // Draw placeholder background
        painter.fillRect(rect(), QColor(40, 40, 40));
    }

    // Reset clip for border drawing
    painter.setClipping(false);

    // Draw border
    QPen borderPen(QColor(0, 150, 255), 3);
    painter.setPen(borderPen);
    painter.setBrush(Qt::NoBrush);

    if (m_circular) {
        painter.drawEllipse(rect().adjusted(1, 1, -1, -1));
    } else {
        painter.drawRect(rect().adjusted(1, 1, -1, -1));
    }

    // Note: No internal crosshair - the cursor itself is the crosshair

    // Draw magnification label
    painter.setPen(Qt::white);
    painter.setFont(QFont("Arial", 10, QFont::Bold));
    QString label = QString("%1x").arg(m_magnification, 0, 'f', 1);
    painter.drawText(rect().adjusted(5, 5, -5, -5), Qt::AlignTop | Qt::AlignRight, label);
}

} // namespace Etrek::ImageViewer::Widget
