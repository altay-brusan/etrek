#include "ImageViewport.h"
#include "ui_ImageViewport.h"

#include <QVTKOpenGLNativeWidget.h>
#include <QVBoxLayout>
#include <QMouseEvent>
#include <QWheelEvent>
#include <QFrame>

ImageViewport::ImageViewport(QWidget* parent)
    : QWidget(parent)
    , ui(new Ui::ImageViewport)
{
    ui->setupUi(this);

    // Create VTK widget
    m_vtkWidget = new QVTKOpenGLNativeWidget(this);
    m_vtkWidget->setMouseTracking(true);

    // Add to layout
    QVBoxLayout* layout = new QVBoxLayout(this);
    layout->setContentsMargins(2, 2, 2, 2);
    layout->setSpacing(0);
    layout->addWidget(m_vtkWidget);
    setLayout(layout);

    // Install event filter to capture VTK widget events
    m_vtkWidget->installEventFilter(this);

    // Set default style
    setAutoFillBackground(true);
    updateBorderStyle();

    // Enable mouse tracking
    setMouseTracking(true);
}

ImageViewport::~ImageViewport() {
    delete ui;
}

QVTKOpenGLNativeWidget* ImageViewport::vtkWidget() const {
    return m_vtkWidget;
}

void ImageViewport::setActive(bool active) {
    if (m_active != active) {
        m_active = active;
        updateBorderStyle();
    }
}

bool ImageViewport::isActive() const {
    return m_active;
}

void ImageViewport::setIndex(int index) {
    m_index = index;
}

int ImageViewport::index() const {
    return m_index;
}

void ImageViewport::setInteractionEnabled(bool enabled) {
    m_interactionEnabled = enabled;
}

bool ImageViewport::isInteractionEnabled() const {
    return m_interactionEnabled;
}

void ImageViewport::updateBorderStyle() {
    if (m_active) {
        setStyleSheet("ImageViewport { border: 2px solid #00FF00; background-color: #000000; }");
    } else {
        setStyleSheet("ImageViewport { border: 1px solid #404040; background-color: #000000; }");
    }
}

QPointF ImageViewport::mapToImage(const QPoint& widgetPos) const {
    // Map widget coordinates to VTK widget coordinates
    QPoint vtkPos = m_vtkWidget->mapFromParent(widgetPos);
    return QPointF(vtkPos.x(), vtkPos.y());
}

void ImageViewport::mousePressEvent(QMouseEvent* event) {
    if (!m_interactionEnabled) {
        QWidget::mousePressEvent(event);
        return;
    }

    m_mousePressed = true;

    // Activate this viewport
    if (!m_active) {
        emit activated(m_index);
    }

    QPointF imagePos = mapToImage(event->pos());
    emit mousePressed(m_index, imagePos, event->button(), event->modifiers());

    // Don't call base - we handle the event
}

void ImageViewport::mouseMoveEvent(QMouseEvent* event) {
    if (!m_interactionEnabled) {
        QWidget::mouseMoveEvent(event);
        return;
    }

    QPointF imagePos = mapToImage(event->pos());
    emit mouseMoved(m_index, imagePos, event->buttons(), event->modifiers());
}

void ImageViewport::mouseReleaseEvent(QMouseEvent* event) {
    if (!m_interactionEnabled) {
        QWidget::mouseReleaseEvent(event);
        return;
    }

    m_mousePressed = false;

    QPointF imagePos = mapToImage(event->pos());
    emit mouseReleased(m_index, imagePos, event->button(), event->modifiers());
}

void ImageViewport::wheelEvent(QWheelEvent* event) {
    if (!m_interactionEnabled) {
        QWidget::wheelEvent(event);
        return;
    }

    // Activate on wheel if not active
    if (!m_active) {
        emit activated(m_index);
    }

    QPointF imagePos = QPointF(event->position().x(), event->position().y());
    int delta = event->angleDelta().y();
    emit mouseWheelScrolled(m_index, delta, imagePos, event->modifiers());

    event->accept();
}

void ImageViewport::mouseDoubleClickEvent(QMouseEvent* event) {
    if (!m_interactionEnabled) {
        QWidget::mouseDoubleClickEvent(event);
        return;
    }

    QPointF imagePos = mapToImage(event->pos());
    emit mouseDoubleClicked(m_index, imagePos, event->button());
}

void ImageViewport::enterEvent(QEnterEvent* event) {
    // Could add hover effect here
    QWidget::enterEvent(event);
}

void ImageViewport::leaveEvent(QEvent* event) {
    // Could remove hover effect here
    QWidget::leaveEvent(event);
}

bool ImageViewport::eventFilter(QObject* watched, QEvent* event) {
    // Forward VTK widget events to this widget
    if (watched == m_vtkWidget && m_interactionEnabled) {
        switch (event->type()) {
            case QEvent::MouseButtonPress: {
                QMouseEvent* mouseEvent = static_cast<QMouseEvent*>(event);
                mousePressEvent(mouseEvent);
                return true;  // Consume the event
            }
            case QEvent::MouseMove: {
                QMouseEvent* mouseEvent = static_cast<QMouseEvent*>(event);
                mouseMoveEvent(mouseEvent);
                return true;
            }
            case QEvent::MouseButtonRelease: {
                QMouseEvent* mouseEvent = static_cast<QMouseEvent*>(event);
                mouseReleaseEvent(mouseEvent);
                return true;
            }
            case QEvent::Wheel: {
                QWheelEvent* we = static_cast<QWheelEvent*>(event);
                wheelEvent(we);
                return true;
            }
            case QEvent::MouseButtonDblClick: {
                QMouseEvent* mouseEvent = static_cast<QMouseEvent*>(event);
                mouseDoubleClickEvent(mouseEvent);
                return true;
            }
            default:
                break;
        }
    }

    return QWidget::eventFilter(watched, event);
}
