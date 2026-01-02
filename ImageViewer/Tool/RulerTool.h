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
 * - Click on a ruler to select it
 * - Drag handles to resize existing rulers
 * - Press Delete to remove selected ruler
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
    bool isDrawing() const { return m_editState == RulerEditState::DRAWING_NEW; }

    /**
     * @brief Get the current edit state.
     */
    RulerEditState editState() const { return m_editState; }

    /**
     * @brief Check if a position hits any ruler handle.
     * @param pos Position in image coordinates
     * @param outRulerId Output: ID of the ruler with hit handle
     * @param outIsStartHandle Output: true if start handle, false if end handle
     * @return true if a handle was hit
     */
    bool hitTestHandle(const QPointF& pos, int& outRulerId, bool& outIsStartHandle) const;

    /**
     * @brief Check if a position hits any ruler line.
     * @param pos Position in image coordinates
     * @return ID of the hit ruler, or -1 if none
     */
    int hitTestRuler(const QPointF& pos) const;

    /**
     * @brief Select a ruler for editing.
     */
    void selectRuler(int rulerId);

    /**
     * @brief Get the currently selected ruler ID.
     */
    int selectedRuler() const { return m_selectedRulerId; }

    /**
     * @brief Delete a ruler by ID.
     */
    void deleteRuler(int rulerId);

    /**
     * @brief Delete the currently selected ruler.
     */
    void deleteSelectedRuler();

signals:
    /**
     * @brief Emitted as measurement line is being drawn.
     */
    void measurementLineUpdated(const MeasurementLine& line);

    /**
     * @brief Emitted when measurement line is completed.
     */
    void measurementLineCompleted(const MeasurementLine& line);

    /**
     * @brief Emitted when ruler selection changes.
     */
    void rulerSelected(int rulerId);

    /**
     * @brief Emitted when a ruler is modified (moved/resized).
     */
    void rulerModified(int rulerId, const MeasurementLine& line);

    /**
     * @brief Emitted when a ruler is deleted.
     */
    void rulerDeleted(int rulerId);

    /**
     * @brief Emitted when measurements list changes (for overlay refresh).
     */
    void measurementsChanged();

    /**
     * @brief Emitted when handle hover state changes.
     * @param rulerId ID of ruler being hovered, or -1 if none
     * @param isStartHandle true if hovering start handle, false if end
     */
    void handleHovered(int rulerId, bool isStartHandle);

    /**
     * @brief Emitted when no handle is being hovered.
     */
    void handleHoverCleared();

private:
    int findRulerIndex(int rulerId) const;

    bool m_active = false;
    RulerEditState m_editState = RulerEditState::NONE;
    MeasurementLine m_currentLine;
    QVector<MeasurementLine> m_measurements;
    double m_pixelSpacingX = 1.0;
    double m_pixelSpacingY = 1.0;

    // Edit mode state
    int m_selectedRulerId = -1;
    int m_dragRulerId = -1;
    bool m_dragIsStart = false;
    int m_nextRulerId = 0;

    // Hover state tracking
    int m_lastHoveredRulerId = -1;
    bool m_lastHoveredIsStart = false;

    // Hit testing threshold (pixels)
    static constexpr double HANDLE_HIT_THRESHOLD = 15.0;
    static constexpr double LINE_HIT_THRESHOLD = 8.0;
};

} // namespace Etrek::ImageViewer::Tool

#endif // RULERTOOL_H
