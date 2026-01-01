#ifndef RESETTOOL_H
#define RESETTOOL_H

#include "IImageTool.h"
#include "ImageViewerExport.h"

namespace Etrek::ImageViewer::Tool {

/**
 * @class ResetTool
 * @brief Tool for resetting the viewport to its original state.
 *
 * This is an "instant action" tool - clicking triggers the reset.
 */
class IMAGEVIEWER_EXPORT ResetTool : public IImageTool {
    Q_OBJECT

public:
    explicit ResetTool(QObject* parent = nullptr);
    ~ResetTool() override = default;

    // IImageTool interface
    ToolType type() const override { return ToolType::RESET; }
    QString name() const override { return tr("Reset"); }
    QString tooltip() const override { return tr("Reset view to original state"); }
    QIcon icon() const override;
    bool isActive() const override { return m_active; }
    void activate() override;
    void deactivate() override;

    void onMousePress(const QPointF& pos, Qt::MouseButton button) override;
    void onMouseMove(const QPointF& pos) override;
    void onMouseRelease(const QPointF& pos, Qt::MouseButton button) override;
    void onMouseWheel(int delta, const QPointF& pos) override;
    void reset() override;

signals:
    /**
     * @brief Emitted when reset is triggered.
     */
    void resetRequested();

private:
    bool m_active = false;
};

} // namespace Etrek::ImageViewer::Tool

#endif // RESETTOOL_H
