#ifndef RULEROVERLAYWIDGET_H
#define RULEROVERLAYWIDGET_H

#include <QWidget>
#include <QVector>
#include <functional>
#include "ImageViewerTypes.h"
#include "ImageViewerExport.h"

namespace Etrek::ImageViewer::Widget {

/**
 * @class RulerOverlayWidget
 * @brief Transparent overlay widget that renders measurement rulers.
 *
 * This widget is placed on top of a viewport and renders:
 * - Ruler lines between start and end points
 * - Circular handles at endpoints for dragging
 * - Distance labels positioned near the line midpoint
 *
 * The widget uses a coordinate transform function to convert
 * image coordinates to widget coordinates, allowing rulers to
 * stay aligned with the image during zoom/pan operations.
 */
class IMAGEVIEWER_EXPORT RulerOverlayWidget : public QWidget {
    Q_OBJECT

public:
    using CoordinateTransform = std::function<QPointF(const QPointF&)>;

    explicit RulerOverlayWidget(QWidget* parent = nullptr);
    ~RulerOverlayWidget() override = default;

    /**
     * @brief Set the function to convert image coords to widget coords.
     * Must be called before rendering.
     */
    void setImageToWidgetTransform(CoordinateTransform transform);

    /**
     * @brief Update the list of completed rulers to display.
     */
    void setMeasurements(const QVector<MeasurementLine>& measurements);

    /**
     * @brief Get the current measurements.
     */
    const QVector<MeasurementLine>& measurements() const { return m_measurements; }

    /**
     * @brief Set the current in-progress measurement (during drawing).
     */
    void setCurrentMeasurement(const MeasurementLine& line);

    /**
     * @brief Clear the in-progress measurement.
     */
    void clearCurrentMeasurement();

    /**
     * @brief Set the currently selected ruler for editing.
     */
    void setSelectedRuler(int rulerId);

    /**
     * @brief Get the currently selected ruler ID.
     */
    int selectedRuler() const { return m_selectedRulerId; }

    /**
     * @brief Set which handle is being hovered.
     */
    void setHoveredHandle(int rulerId, bool isStartHandle);

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
    void drawRulerLine(QPainter& painter, const MeasurementLine& line, bool isSelected, bool isCurrent);
    void drawHandle(QPainter& painter, const QPointF& widgetPos, bool isHovered, bool isSelected);
    void drawDistanceLabel(QPainter& painter, const MeasurementLine& line, const QPointF& startWidget, const QPointF& endWidget);
    QPointF transformToWidget(const QPointF& imagePos) const;

    CoordinateTransform m_imageToWidget;
    QVector<MeasurementLine> m_measurements;
    MeasurementLine m_currentLine;
    bool m_hasCurrentLine = false;
    int m_selectedRulerId = -1;
    int m_hoveredRulerId = -1;
    bool m_hoveredIsStart = false;

    // Visual constants
    static constexpr int HANDLE_RADIUS = 5;
    static constexpr int LINE_WIDTH = 2;
    static constexpr int LABEL_FONT_SIZE = 10;
};

} // namespace Etrek::ImageViewer::Widget

#endif // RULEROVERLAYWIDGET_H
