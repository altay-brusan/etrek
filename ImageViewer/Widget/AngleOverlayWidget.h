#ifndef ANGLEOVERLAYWIDGET_H
#define ANGLEOVERLAYWIDGET_H

#include <QWidget>
#include <QVector>
#include <functional>
#include "ImageViewerTypes.h"
#include "ImageViewerExport.h"

namespace Etrek::ImageViewer::Widget {

/**
 * @class AngleOverlayWidget
 * @brief Transparent overlay widget that renders angle measurements.
 *
 * This widget is placed on top of a viewport and renders:
 * - Two arm lines from vertex to point1 and point2
 * - An arc at the vertex showing the measured angle
 * - Square handles at all three points for dragging
 * - Angle value label in degrees positioned on the arc
 *
 * The widget uses a coordinate transform function to convert
 * image coordinates to widget coordinates, allowing angles to
 * stay aligned with the image during zoom/pan operations.
 */
class IMAGEVIEWER_EXPORT AngleOverlayWidget : public QWidget {
    Q_OBJECT

public:
    using CoordinateTransform = std::function<QPointF(const QPointF&)>;

    explicit AngleOverlayWidget(QWidget* parent = nullptr);
    ~AngleOverlayWidget() override = default;

    /**
     * @brief Set the function to convert image coords to widget coords.
     * Must be called before rendering.
     */
    void setImageToWidgetTransform(CoordinateTransform transform);

    /**
     * @brief Update the list of completed angles to display.
     */
    void setMeasurements(const QVector<MeasurementAngle>& measurements);

    /**
     * @brief Get the current measurements.
     */
    const QVector<MeasurementAngle>& measurements() const { return m_measurements; }

    /**
     * @brief Set the current in-progress measurement (during drawing).
     */
    void setCurrentMeasurement(const MeasurementAngle& angle);

    /**
     * @brief Clear the in-progress measurement.
     */
    void clearCurrentMeasurement();

    /**
     * @brief Set the currently selected angle for editing.
     */
    void setSelectedAngle(int angleId);

    /**
     * @brief Get the currently selected angle ID.
     */
    int selectedAngle() const { return m_selectedAngleId; }

    /**
     * @brief Set which handle is being hovered.
     */
    void setHoveredHandle(int angleId, AngleHandleType handleType);

    /**
     * @brief Clear handle hover state.
     */
    void clearHoveredHandle();

    /**
     * @brief Trigger redraw.
     */
    void refresh();

protected:
    void paintEvent(QPaintEvent* event) override;

private:
    void drawAngle(QPainter& painter, const MeasurementAngle& angle, bool isSelected, bool isCurrent);
    void drawHandle(QPainter& painter, const QPointF& widgetPos, bool isHovered, bool isSelected);
    void drawArc(QPainter& painter, const MeasurementAngle& angle,
                 const QPointF& vertexWidget, const QPointF& point1Widget, const QPointF& point2Widget);
    void drawAngleLabel(QPainter& painter, const MeasurementAngle& angle, const QPointF& vertexWidget);
    QPointF transformToWidget(const QPointF& imagePos) const;

    CoordinateTransform m_imageToWidget;
    QVector<MeasurementAngle> m_measurements;
    MeasurementAngle m_currentAngle;
    bool m_hasCurrentAngle = false;
    int m_selectedAngleId = -1;
    int m_hoveredAngleId = -1;
    AngleHandleType m_hoveredHandleType = AngleHandleType::NONE;

    // Visual constants
    static constexpr int HANDLE_RADIUS = 5;
    static constexpr int LINE_WIDTH = 2;
    static constexpr int ARC_RADIUS = 30;      // Radius of the angle arc
    static constexpr int LABEL_FONT_SIZE = 10;
};

} // namespace Etrek::ImageViewer::Widget

#endif // ANGLEOVERLAYWIDGET_H
