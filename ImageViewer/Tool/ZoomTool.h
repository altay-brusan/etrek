#ifndef ZOOMTOOL_H
#define ZOOMTOOL_H

#include "IImageTool.h"
#include "ImageViewerExport.h"

namespace Etrek::ImageViewer::Tool {

/**
 * @class ZoomTool
 * @brief Tool for zooming in/out on images.
 *
 * Interaction:
 * - Left mouse drag up: Zoom in
 * - Left mouse drag down: Zoom out
 * - Mouse wheel: Zoom in/out
 * - Double-click: Fit to window
 */
class IMAGEVIEWER_EXPORT ZoomTool : public IImageTool {
    Q_OBJECT

public:
    explicit ZoomTool(QObject* parent = nullptr);
    ~ZoomTool() override = default;

    // IImageTool interface
    ToolType type() const override { return ToolType::ZOOM; }
    QString name() const override { return tr("Zoom"); }
    QString tooltip() const override { return tr("Zoom in/out (Z)"); }
    QIcon icon() const override;
    bool isActive() const override { return m_active; }
    void activate() override;
    void deactivate() override;

    void onMousePress(const QPointF& pos, Qt::MouseButton button) override;
    void onMouseMove(const QPointF& pos) override;
    void onMouseRelease(const QPointF& pos, Qt::MouseButton button) override;
    void onMouseWheel(int delta, const QPointF& pos) override;
    void reset() override;

    /**
     * @brief Set the current zoom factor.
     */
    void setCurrentZoom(double zoom);
    double currentZoom() const { return m_currentZoom; }

    /**
     * @brief Set zoom limits.
     */
    void setZoomLimits(double minZoom, double maxZoom);

    /**
     * @brief Set zoom sensitivity for drag operations.
     */
    void setSensitivity(double sensitivity);

private:
    bool m_active = false;
    bool m_dragging = false;
    QPointF m_lastPos;
    QPointF m_dragStartPos;
    double m_currentZoom = 1.0;
    double m_minZoom = 0.1;
    double m_maxZoom = 20.0;
    double m_sensitivity = 0.01;
};

} // namespace Etrek::ImageViewer::Tool

#endif // ZOOMTOOL_H
