#ifndef ANGLETOOL_H
#define ANGLETOOL_H

#include "IImageTool.h"
#include "ImageViewerExport.h"
#include <QVector>

namespace Etrek::ImageViewer::Tool {

/**
 * @class AngleTool
 * @brief Tool for measuring angles on images.
 *
 * Interaction:
 * - Click to place first point
 * - Click to place vertex (middle point)
 * - Click to place third point and complete angle
 */
class IMAGEVIEWER_EXPORT AngleTool : public IImageTool {
    Q_OBJECT

public:
    explicit AngleTool(QObject* parent = nullptr);
    ~AngleTool() override = default;

    // IImageTool interface
    ToolType type() const override { return ToolType::ANGLE; }
    QString name() const override { return tr("Angle"); }
    QString tooltip() const override { return tr("Measure angle (A)"); }
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
     * @brief Get all completed angle measurements.
     */
    const QVector<MeasurementAngle>& getMeasurements() const { return m_measurements; }

    /**
     * @brief Clear all measurements.
     */
    void clearMeasurements();

    /**
     * @brief Get the current (in-progress) measurement.
     */
    const MeasurementAngle& currentMeasurement() const { return m_currentAngle; }
    int pointsPlaced() const { return m_pointsPlaced; }

private:
    bool m_active = false;
    int m_pointsPlaced = 0;  // 0, 1, 2, or 3
    MeasurementAngle m_currentAngle;
    QVector<MeasurementAngle> m_measurements;
};

} // namespace Etrek::ImageViewer::Tool

#endif // ANGLETOOL_H
