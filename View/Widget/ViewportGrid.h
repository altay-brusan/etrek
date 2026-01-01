#ifndef VIEWPORTGRID_H
#define VIEWPORTGRID_H

#include <QWidget>
#include <QPointF>
#include <array>

#include "ViewportLayout.h"

// Forward declarations
class QGridLayout;
class ImageViewport;
class QVTKOpenGLNativeWidget;

namespace Ui {
class ViewportGrid;
}

/**
 * @class ViewportGrid
 * @brief Container widget managing multiple ImageViewport widgets in a grid.
 *
 * Supports layouts:
 * - SINGLE: 1x1 (one viewport)
 * - ONE_BY_TWO: 1x2 (two viewports side by side)
 * - TWO_BY_ONE: 2x1 (two viewports stacked)
 * - TWO_BY_TWO: 2x2 (four viewports)
 */
class ViewportGrid : public QWidget
{
    Q_OBJECT

public:
    static constexpr int MAX_VIEWPORTS = 4;

    explicit ViewportGrid(QWidget* parent = nullptr);
    ~ViewportGrid() override;

    /**
     * @brief Set the viewport layout.
     */
    void setViewportLayout(Etrek::ImageViewer::ViewportLayout layout);
    Etrek::ImageViewer::ViewportLayout currentLayout() const;

    /**
     * @brief Get a viewport by index.
     */
    ImageViewport* getViewport(int index);
    const ImageViewport* getViewport(int index) const;

    /**
     * @brief Get the VTK widget for a viewport.
     */
    QVTKOpenGLNativeWidget* getVtkWidget(int index);

    /**
     * @brief Get all VTK widgets.
     */
    std::array<QVTKOpenGLNativeWidget*, MAX_VIEWPORTS> getAllVtkWidgets();

    /**
     * @brief Get the active viewport index.
     */
    int activeViewportIndex() const;

    /**
     * @brief Set the active viewport.
     */
    void setActiveViewport(int index);

    /**
     * @brief Get number of visible viewports.
     */
    int visibleViewportCount() const;

signals:
    /**
     * @brief Emitted when active viewport changes.
     */
    void activeViewportChanged(int index);

    /**
     * @brief Emitted when layout changes.
     */
    void layoutChanged(Etrek::ImageViewer::ViewportLayout layout);

    /**
     * @brief Forwarded from viewports - mouse press.
     */
    void viewportMousePressed(int viewportIndex, const QPointF& pos, Qt::MouseButton button, Qt::KeyboardModifiers modifiers);

    /**
     * @brief Forwarded from viewports - mouse move.
     */
    void viewportMouseMoved(int viewportIndex, const QPointF& pos, Qt::MouseButtons buttons, Qt::KeyboardModifiers modifiers);

    /**
     * @brief Forwarded from viewports - mouse release.
     */
    void viewportMouseReleased(int viewportIndex, const QPointF& pos, Qt::MouseButton button, Qt::KeyboardModifiers modifiers);

    /**
     * @brief Forwarded from viewports - mouse wheel.
     */
    void viewportMouseWheel(int viewportIndex, int delta, const QPointF& pos, Qt::KeyboardModifiers modifiers);

    /**
     * @brief Forwarded from viewports - double click.
     */
    void viewportDoubleClicked(int viewportIndex, const QPointF& pos, Qt::MouseButton button);

protected:
    void resizeEvent(QResizeEvent* event) override;

private slots:
    void onViewportActivated(int index);

private:
    void createViewports();
    void updateGridLayout();
    void connectViewportSignals(ImageViewport* viewport);

    Ui::ViewportGrid* ui;
    QGridLayout* m_gridLayout;
    std::array<ImageViewport*, MAX_VIEWPORTS> m_viewports;
    Etrek::ImageViewer::ViewportLayout m_currentLayout = Etrek::ImageViewer::ViewportLayout::SINGLE;
    int m_activeIndex = 0;
};

#endif // VIEWPORTGRID_H
