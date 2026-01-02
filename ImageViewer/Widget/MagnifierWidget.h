#ifndef MAGNIFIERWIDGET_H
#define MAGNIFIERWIDGET_H

#include <QWidget>
#include <QImage>
#include <QPointF>
#include "ImageViewerExport.h"

namespace Etrek::ImageViewer::Widget {

/**
 * @class MagnifierWidget
 * @brief A floating magnifier overlay that shows a zoomed portion of an image.
 *
 * The magnifier displays a circular or rectangular region around the cursor
 * position, magnified by a configurable factor (2x or 3x).
 */
class IMAGEVIEWER_EXPORT MagnifierWidget : public QWidget {
    Q_OBJECT

public:
    explicit MagnifierWidget(QWidget* parent = nullptr);
    ~MagnifierWidget() override = default;

    /**
     * @brief Set the magnification factor (2.0 or 3.0 typically).
     */
    void setMagnification(double factor);
    double magnification() const { return m_magnification; }

    /**
     * @brief Toggle between 2x and 3x magnification.
     */
    void toggleMagnification();

    /**
     * @brief Set the size of the magnifier window.
     */
    void setMagnifierSize(int size);
    int magnifierSize() const { return m_size; }

    /**
     * @brief Update the magnifier with a new source image and cursor position.
     * @param sourceImage The full image to magnify from
     * @param cursorPos The cursor position in image coordinates
     * @param widgetPos The cursor position in widget/screen coordinates for positioning
     */
    void updateMagnifier(const QImage& sourceImage, const QPointF& cursorPos, const QPoint& widgetPos);

    /**
     * @brief Show the magnifier at the specified position.
     */
    void showAt(const QPoint& pos);

    /**
     * @brief Hide the magnifier.
     */
    void hideMagnifier();

signals:
    void magnificationChanged(double factor);

protected:
    void paintEvent(QPaintEvent* event) override;

private:
    void updateMagnifiedRegion();

    QImage m_sourceImage;
    QImage m_magnifiedImage;
    QPointF m_cursorPos;
    double m_magnification = 2.0;
    int m_size = 280;  // Diameter of magnifier (1.4x of 200)
    bool m_circular = true;  // Use circular magnifier shape
};

} // namespace Etrek::ImageViewer::Widget

#endif // MAGNIFIERWIDGET_H
