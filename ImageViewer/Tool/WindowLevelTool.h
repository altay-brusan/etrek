#ifndef WINDOWLEVELTOOL_H
#define WINDOWLEVELTOOL_H

#include "IImageTool.h"
#include "ImageViewerExport.h"

namespace Etrek::ImageViewer::Tool {

/**
 * @class WindowLevelTool
 * @brief Tool for adjusting window/level (contrast/brightness).
 *
 * Interaction:
 * - Left mouse drag horizontal: Adjust window width
 * - Left mouse drag vertical: Adjust window center (level)
 * - Mouse wheel: Quick adjustment
 */
class IMAGEVIEWER_EXPORT WindowLevelTool : public IImageTool {
    Q_OBJECT

public:
    explicit WindowLevelTool(QObject* parent = nullptr);
    ~WindowLevelTool() override = default;

    // IImageTool interface
    ToolType type() const override { return ToolType::WINDOW_LEVEL; }
    QString name() const override { return tr("Window/Level"); }
    QString tooltip() const override { return tr("Adjust contrast/brightness (W)"); }
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
     * @brief Set the current window/level values.
     */
    void setCurrentWindowLevel(double window, double level);

    /**
     * @brief Set sensitivity for drag operations.
     */
    void setSensitivity(double sensitivity);

private:
    bool m_active = false;
    bool m_dragging = false;
    QPointF m_lastPos;
    double m_currentWindow = 256.0;
    double m_currentLevel = 128.0;
    double m_sensitivity = 1.0;
};

} // namespace Etrek::ImageViewer::Tool

#endif // WINDOWLEVELTOOL_H
