#ifndef RULERTOOL_H
#define RULERTOOL_H

#include "IImageTool.h"
#include "ImageViewerExport.h"
#include <QVector>

namespace Etrek::ImageViewer::Tool {

/**
 * @class RulerTool
 * @brief Tool for measuring distances on images.
 *
 * Interaction:
 * - Click and drag to draw a measurement line
 * - Release to complete the measurement
 * - Distance is calculated using pixel spacing
 */
class IMAGEVIEWER_EXPORT RulerTool : public IImageTool {
    Q_OBJECT

public:
    explicit RulerTool(QObject* parent = nullptr);
    ~RulerTool() override = default;

    // IImageTool interface
    ToolType type() const override { return ToolType::RULER; }
    QString name() const override { return tr("Ruler"); }
    QString tooltip() const override { return tr("Measure distance (R)"); }
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
     * @brief Set pixel spacing for accurate measurements.
     */
    void setPixelSpacing(double spacingX, double spacingY);

    /**
     * @brief Get all completed measurements.
     */
    const QVector<MeasurementLine>& getMeasurements() const { return m_measurements; }

    /**
     * @brief Clear all measurements.
     */
    void clearMeasurements();

    /**
     * @brief Get the current (in-progress) measurement.
     */
    const MeasurementLine& currentMeasurement() const { return m_currentLine; }
    bool isDrawing() const { return m_drawing; }

signals:
    /**
     * @brief Emitted as measurement line is being drawn.
     */
    void measurementLineUpdated(const MeasurementLine& line);

    /**
     * @brief Emitted when measurement line is completed.
     */
    void measurementLineCompleted(const MeasurementLine& line);

private:
    bool m_active = false;
    bool m_drawing = false;
    MeasurementLine m_currentLine;
    QVector<MeasurementLine> m_measurements;
    double m_pixelSpacingX = 1.0;
    double m_pixelSpacingY = 1.0;
};

} // namespace Etrek::ImageViewer::Tool

#endif // RULERTOOL_H
