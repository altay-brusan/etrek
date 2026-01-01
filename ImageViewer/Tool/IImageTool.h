#ifndef IIMAGETOOL_H
#define IIMAGETOOL_H

#include <QObject>
#include <QPointF>
#include <QIcon>
#include <QString>
#include "ImageViewerTypes.h"
#include "ImageViewerExport.h"

namespace Etrek::ImageViewer {

/**
 * @class IImageTool
 * @brief Interface for imaging tools (zoom, pan, window/level, measurements).
 *
 * Tools handle mouse interaction events and emit signals when
 * the viewport state should be updated.
 */
class IMAGEVIEWER_EXPORT IImageTool : public QObject {
    Q_OBJECT

public:
    explicit IImageTool(QObject* parent = nullptr) : QObject(parent) {}
    virtual ~IImageTool() = default;

    /**
     * @brief Get the tool type.
     */
    virtual ToolType type() const = 0;

    /**
     * @brief Get display name for the tool.
     */
    virtual QString name() const = 0;

    /**
     * @brief Get tooltip description.
     */
    virtual QString tooltip() const = 0;

    /**
     * @brief Get the tool icon.
     */
    virtual QIcon icon() const = 0;

    /**
     * @brief Check if the tool is currently active.
     */
    virtual bool isActive() const = 0;

    /**
     * @brief Activate the tool.
     */
    virtual void activate() = 0;

    /**
     * @brief Deactivate the tool.
     */
    virtual void deactivate() = 0;

    /**
     * @brief Handle mouse press event.
     * @param pos Position in image coordinates
     * @param button The mouse button pressed
     */
    virtual void onMousePress(const QPointF& pos, Qt::MouseButton button) = 0;

    /**
     * @brief Handle mouse move event.
     * @param pos Position in image coordinates
     */
    virtual void onMouseMove(const QPointF& pos) = 0;

    /**
     * @brief Handle mouse release event.
     * @param pos Position in image coordinates
     * @param button The mouse button released
     */
    virtual void onMouseRelease(const QPointF& pos, Qt::MouseButton button) = 0;

    /**
     * @brief Handle mouse wheel event.
     * @param delta Wheel delta (positive = scroll up)
     * @param pos Position in image coordinates
     */
    virtual void onMouseWheel(int delta, const QPointF& pos) = 0;

    /**
     * @brief Reset tool state.
     */
    virtual void reset() = 0;

signals:
    /**
     * @brief Emitted when zoom should be applied.
     * @param factor Zoom factor (> 1 = zoom in, < 1 = zoom out)
     * @param centerPos Center position for zoom
     */
    void zoomRequested(double factor, const QPointF& centerPos);

    /**
     * @brief Emitted when pan should be applied.
     * @param deltaX Horizontal pan amount in pixels
     * @param deltaY Vertical pan amount in pixels
     */
    void panRequested(double deltaX, double deltaY);

    /**
     * @brief Emitted when window/level should be changed.
     * @param window New window width
     * @param level New window center
     */
    void windowLevelChanged(double window, double level);

    /**
     * @brief Emitted when a measurement line is being drawn.
     * @param line The measurement line data
     */
    void measurementLineUpdated(const MeasurementLine& line);

    /**
     * @brief Emitted when a measurement line is completed.
     * @param line The completed measurement line
     */
    void measurementLineCompleted(const MeasurementLine& line);

    /**
     * @brief Emitted when an angle measurement is being drawn.
     * @param angle The angle measurement data
     */
    void measurementAngleUpdated(const MeasurementAngle& angle);

    /**
     * @brief Emitted when an angle measurement is completed.
     * @param angle The completed angle measurement
     */
    void measurementAngleCompleted(const MeasurementAngle& angle);

    /**
     * @brief Emitted when reset is requested.
     */
    void resetRequested();

    /**
     * @brief Emitted when the cursor should change.
     * @param cursor The new cursor shape
     */
    void cursorChanged(Qt::CursorShape cursor);
};

} // namespace Etrek::ImageViewer

#endif // IIMAGETOOL_H
