#ifndef EDGERULERWIDGET_H
#define EDGERULERWIDGET_H

#include <QWidget>
#include <functional>
#include "ImageViewerExport.h"

namespace Etrek::ImageViewer::Widget {

/**
 * @class EdgeRulerWidget
 * @brief Transparent overlay widget that renders measurement rulers on image edges.
 *
 * This widget displays calibrated rulers on the right and bottom edges of a
 * medical image viewport. The rulers show physical measurements (mm/cm) based
 * on the DICOM pixel spacing and update with zoom/pan operations.
 *
 * Features:
 * - Right edge: Vertical ruler showing height measurements
 * - Bottom edge: Horizontal ruler showing width measurements (optional)
 * - Tick marks at 1mm intervals with major ticks at 1cm
 * - Labels at centimeter intervals
 * - Adapts to zoom and pan operations
 */
class IMAGEVIEWER_EXPORT EdgeRulerWidget : public QWidget {
    Q_OBJECT

public:
    using CoordinateTransform = std::function<QPointF(const QPointF&)>;

    explicit EdgeRulerWidget(QWidget* parent = nullptr);
    ~EdgeRulerWidget() override = default;

    /**
     * @brief Set the pixel spacing (mm per pixel).
     * @param spacingX Horizontal spacing in mm/pixel
     * @param spacingY Vertical spacing in mm/pixel
     */
    void setPixelSpacing(double spacingX, double spacingY);

    /**
     * @brief Set the image dimensions in pixels.
     */
    void setImageDimensions(int width, int height);

    /**
     * @brief Set the current zoom level.
     * @param zoom Zoom factor (1.0 = 100%)
     */
    void setZoomLevel(double zoom);

    /**
     * @brief Set the function to convert image coords to widget coords.
     */
    void setImageToWidgetTransform(CoordinateTransform transform);

    /**
     * @brief Show/hide the horizontal (bottom) ruler.
     */
    void setShowHorizontalRuler(bool show);
    bool showHorizontalRuler() const { return m_showHorizontal; }

    /**
     * @brief Show/hide the vertical (right) ruler.
     */
    void setShowVerticalRuler(bool show);
    bool showVerticalRuler() const { return m_showVertical; }

    /**
     * @brief Set the ruler color.
     */
    void setRulerColor(const QColor& color);

    /**
     * @brief Set the ruler width in pixels.
     */
    void setRulerWidth(int width);

protected:
    void paintEvent(QPaintEvent* event) override;

private:
    void drawVerticalRuler(QPainter& painter);
    void drawHorizontalRuler(QPainter& painter);
    QPointF transformToWidget(const QPointF& imagePos) const;

    /**
     * @brief Calculate tick interval based on zoom level.
     * Returns the interval in mm that gives reasonable tick spacing.
     */
    double calculateTickInterval() const;

    CoordinateTransform m_imageToWidget;

    double m_pixelSpacingX = 1.0;  // mm per pixel
    double m_pixelSpacingY = 1.0;
    int m_imageWidth = 0;
    int m_imageHeight = 0;
    double m_zoomLevel = 1.0;

    bool m_showHorizontal = true;   // Bottom ruler
    bool m_showVertical = true;     // Right ruler

    QColor m_rulerColor{255, 255, 255};  // White
    int m_rulerWidth = 25;  // Width of ruler area in pixels

    // Visual constants
    static constexpr int MINOR_TICK_LENGTH = 3;   // 1mm ticks
    static constexpr int MEDIUM_TICK_LENGTH = 6;  // 5mm ticks
    static constexpr int MAJOR_TICK_LENGTH = 10;  // 1cm ticks
    static constexpr int FONT_SIZE = 9;
    static constexpr int LABEL_MARGIN = 2;
};

} // namespace Etrek::ImageViewer::Widget

#endif // EDGERULERWIDGET_H
