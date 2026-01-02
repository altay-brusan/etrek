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
 * - Click to place first point (arm 1 endpoint)
 * - Click to place vertex (middle point where angle is measured)
 * - Click to place third point (arm 2 endpoint) to complete
 * - Click on an angle to select it
 * - Drag handles to resize existing angles
 * - Press Delete to remove selected angle
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

    /**
     * @brief Get the current edit state.
     */
    AngleEditState editState() const { return m_editState; }

    /**
     * @brief Check if currently placing points for a new angle.
     */
    bool isPlacing() const {
        return m_editState == AngleEditState::PLACING_POINT1 ||
               m_editState == AngleEditState::PLACING_VERTEX ||
               m_editState == AngleEditState::PLACING_POINT2;
    }

    /**
     * @brief Check if a position hits any angle handle.
     * @param pos Position in image coordinates
     * @param outAngleId Output: ID of the angle with hit handle
     * @param outHandleType Output: Which handle was hit
     * @return true if a handle was hit
     */
    bool hitTestHandle(const QPointF& pos, int& outAngleId, AngleHandleType& outHandleType) const;

    /**
     * @brief Check if a position hits any angle arm.
     * @param pos Position in image coordinates
     * @return ID of the hit angle, or -1 if none
     */
    int hitTestAngle(const QPointF& pos) const;

    /**
     * @brief Select an angle for editing.
     */
    void selectAngle(int angleId);

    /**
     * @brief Get the currently selected angle ID.
     */
    int selectedAngle() const { return m_selectedAngleId; }

    /**
     * @brief Delete an angle by ID.
     */
    void deleteAngle(int angleId);

    /**
     * @brief Delete the currently selected angle.
     */
    void deleteSelectedAngle();

signals:
    /**
     * @brief Emitted as angle is being placed/drawn.
     */
    void measurementAngleUpdated(const MeasurementAngle& angle);

    /**
     * @brief Emitted when angle measurement is completed.
     */
    void measurementAngleCompleted(const MeasurementAngle& angle);

    /**
     * @brief Emitted when angle selection changes.
     */
    void angleSelected(int angleId);

    /**
     * @brief Emitted when an angle is modified (moved/resized).
     */
    void angleModified(int angleId, const MeasurementAngle& angle);

    /**
     * @brief Emitted when an angle is deleted.
     */
    void angleDeleted(int angleId);

    /**
     * @brief Emitted when measurements list changes (for overlay refresh).
     */
    void measurementsChanged();

    /**
     * @brief Emitted when handle hover state changes.
     */
    void handleHovered(int angleId, AngleHandleType handleType);

    /**
     * @brief Emitted when no handle is being hovered.
     */
    void handleHoverCleared();

private:
    int findAngleIndex(int angleId) const;

    bool m_active = false;
    AngleEditState m_editState = AngleEditState::NONE;
    MeasurementAngle m_currentAngle;
    QVector<MeasurementAngle> m_measurements;

    // Edit mode state
    int m_selectedAngleId = -1;
    int m_dragAngleId = -1;
    AngleHandleType m_dragHandleType = AngleHandleType::NONE;
    int m_nextAngleId = 0;

    // Hover state tracking
    int m_lastHoveredAngleId = -1;
    AngleHandleType m_lastHoveredHandleType = AngleHandleType::NONE;

    // Hit testing threshold (pixels)
    static constexpr double HANDLE_HIT_THRESHOLD = 15.0;
    static constexpr double ARM_HIT_THRESHOLD = 8.0;
};

} // namespace Etrek::ImageViewer::Tool

#endif // ANGLETOOL_H
