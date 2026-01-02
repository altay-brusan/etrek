#ifndef ZOOMTOOL_H
#define ZOOMTOOL_H

#include "IImageTool.h"
#include "ImageViewerExport.h"

namespace Etrek::ImageViewer::Tool {

/**
 * @class ZoomTool
 * @brief Magnifier tool for examining image details.
 *
 * When active, displays a magnifying glass overlay that shows
 * a zoomed portion of the image centered on the cursor position.
 *
 * Interaction:
 * - Move mouse: Update magnifier position
 * - Left click: Toggle magnification level (2x/3x)
 * - Mouse wheel: Also toggles magnification level
 */
class IMAGEVIEWER_EXPORT ZoomTool : public IImageTool {
    Q_OBJECT

public:
    explicit ZoomTool(QObject* parent = nullptr);
    ~ZoomTool() override = default;

    // IImageTool interface
    ToolType type() const override { return ToolType::ZOOM; }
    QString name() const override { return tr("Magnifier"); }
    QString tooltip() const override { return tr("Magnifier - examine image details (Z)"); }
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
     * @brief Set the current magnification factor (for state sync).
     */
    void setCurrentZoom(double zoom);
    double currentZoom() const { return m_currentMagnification; }

    /**
     * @brief Set zoom/magnification limits.
     */
    void setZoomLimits(double minZoom, double maxZoom);

    /**
     * @brief Set sensitivity (unused in magnifier mode, kept for API compatibility).
     */
    void setSensitivity(double sensitivity);

private:
    bool m_active = false;
    QPointF m_lastPos;
    double m_currentMagnification = 2.0;
    double m_minZoom = 2.0;
    double m_maxZoom = 3.0;
    double m_sensitivity = 0.01;  // Unused in magnifier mode
};

} // namespace Etrek::ImageViewer::Tool

#endif // ZOOMTOOL_H
