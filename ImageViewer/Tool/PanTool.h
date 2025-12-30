#ifndef PANTOOL_H
#define PANTOOL_H

#include "IImageTool.h"
#include "ImageViewerExport.h"

namespace Etrek::ImageViewer::Tool {

/**
 * @class PanTool
 * @brief Tool for panning/scrolling the image view.
 *
 * Interaction:
 * - Left mouse drag: Pan the image
 * - Middle mouse drag: Also pans (always active)
 */
class IMAGEVIEWER_EXPORT PanTool : public IImageTool {
    Q_OBJECT

public:
    explicit PanTool(QObject* parent = nullptr);
    ~PanTool() override = default;

    // IImageTool interface
    ToolType type() const override { return ToolType::PAN; }
    QString name() const override { return tr("Pan"); }
    QString tooltip() const override { return tr("Pan/scroll image (P)"); }
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
     * @brief Set pan sensitivity.
     */
    void setSensitivity(double sensitivity);

private:
    bool m_active = false;
    bool m_dragging = false;
    QPointF m_lastPos;
    double m_sensitivity = 1.0;
};

} // namespace Etrek::ImageViewer::Tool

#endif // PANTOOL_H
