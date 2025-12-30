#ifndef IMAGEVIEWPORT_H
#define IMAGEVIEWPORT_H

#include <QWidget>
#include <QPointF>

// Forward declarations
class QVTKOpenGLNativeWidget;

namespace Etrek::ImageViewer {
enum class InteractionType;
}

namespace Ui {
class ImageViewport;
}

/**
 * @class ImageViewport
 * @brief A single viewport widget containing a VTK render widget.
 *
 * This widget wraps QVTKOpenGLNativeWidget and adds:
 * - Mouse event forwarding to delegates
 * - Active/inactive border styling
 * - Focus handling for multi-viewport layouts
 */
class ImageViewport : public QWidget
{
    Q_OBJECT

public:
    explicit ImageViewport(QWidget* parent = nullptr);
    ~ImageViewport() override;

    /**
     * @brief Get the underlying VTK widget.
     */
    QVTKOpenGLNativeWidget* vtkWidget() const;

    /**
     * @brief Set whether this viewport is active (has focus).
     */
    void setActive(bool active);
    bool isActive() const;

    /**
     * @brief Set the viewport index (0-3).
     */
    void setIndex(int index);
    int index() const;

    /**
     * @brief Enable/disable mouse interaction.
     */
    void setInteractionEnabled(bool enabled);
    bool isInteractionEnabled() const;

signals:
    /**
     * @brief Emitted when the viewport is clicked (gains focus).
     */
    void activated(int index);

    /**
     * @brief Emitted on mouse press.
     */
    void mousePressed(int index, const QPointF& pos, Qt::MouseButton button, Qt::KeyboardModifiers modifiers);

    /**
     * @brief Emitted on mouse move.
     */
    void mouseMoved(int index, const QPointF& pos, Qt::MouseButtons buttons, Qt::KeyboardModifiers modifiers);

    /**
     * @brief Emitted on mouse release.
     */
    void mouseReleased(int index, const QPointF& pos, Qt::MouseButton button, Qt::KeyboardModifiers modifiers);

    /**
     * @brief Emitted on mouse wheel.
     */
    void mouseWheelScrolled(int index, int delta, const QPointF& pos, Qt::KeyboardModifiers modifiers);

    /**
     * @brief Emitted on mouse double-click.
     */
    void mouseDoubleClicked(int index, const QPointF& pos, Qt::MouseButton button);

protected:
    void mousePressEvent(QMouseEvent* event) override;
    void mouseMoveEvent(QMouseEvent* event) override;
    void mouseReleaseEvent(QMouseEvent* event) override;
    void wheelEvent(QWheelEvent* event) override;
    void mouseDoubleClickEvent(QMouseEvent* event) override;
    void enterEvent(QEnterEvent* event) override;
    void leaveEvent(QEvent* event) override;
    bool eventFilter(QObject* watched, QEvent* event) override;

private:
    void updateBorderStyle();
    QPointF mapToImage(const QPoint& widgetPos) const;

    Ui::ImageViewport* ui;
    QVTKOpenGLNativeWidget* m_vtkWidget;
    int m_index = 0;
    bool m_active = false;
    bool m_interactionEnabled = true;
    bool m_mousePressed = false;
};

#endif // IMAGEVIEWPORT_H
