#ifndef VTKVIEWPORTRENDERER_H
#define VTKVIEWPORTRENDERER_H

#include <QObject>
#include <QString>
#include <QColor>
#include <QImage>
#include <array>
#include <memory>

#include <vtkSmartPointer.h>
#include <vtkRenderer.h>

// Forward declarations
class QVTKOpenGLNativeWidget;
class vtkGenericOpenGLRenderWindow;
class vtkImageActor;
class vtkImageData;
class vtkImageMapToWindowLevelColors;
class vtkInteractorStyleImage;
class vtkTextActor;
class vtkActor2D;
class vtkLineSource;
class vtkPolyDataMapper2D;
class vtkCamera;

namespace Etrek::ImageViewer {
struct ViewportState;
struct DicomMetadata;
enum class OverlayCorner;
}

namespace Etrek::ImageViewer::Rendering {

/**
 * @class VtkViewportRenderer
 * @brief Manages VTK rendering pipeline for a single viewport.
 *
 * Handles:
 * - Image display with window/level adjustment
 * - DICOM overlay text (4 corners)
 * - Zoom and pan operations
 * - Crosshair display
 * - Measurement annotations
 */
class VtkViewportRenderer : public QObject {
    Q_OBJECT

public:
    /**
     * @brief Construct renderer for given VTK widget.
     * @param widget The QVTKOpenGLNativeWidget to render to
     * @param parent Parent QObject
     */
    explicit VtkViewportRenderer(QVTKOpenGLNativeWidget* widget, QObject* parent = nullptr);
    ~VtkViewportRenderer() override;

    // Prevent copying
    VtkViewportRenderer(const VtkViewportRenderer&) = delete;
    VtkViewportRenderer& operator=(const VtkViewportRenderer&) = delete;

    /**
     * @brief Set the image data to display.
     * @param imageData VTK image data
     * @param autoWindowLevel If true, calculate W/L from image data
     */
    void setImageData(vtkImageData* imageData, bool autoWindowLevel = true);

    /**
     * @brief Clear the current image.
     */
    void clearImage();

    /**
     * @brief Check if an image is currently loaded.
     */
    bool hasImage() const;

    // Window/Level operations
    void setWindowLevel(double window, double level);
    void getWindowLevel(double& window, double& level) const;
    void resetWindowLevel();
    void autoWindowLevel();

    // Camera/View operations
    void setZoom(double factor);
    double getZoom() const;
    void setPan(double deltaX, double deltaY);
    void getPan(double& panX, double& panY) const;
    void resetCamera();
    void fitToWindow();

    // Inversion
    void setInverted(bool inverted);
    bool isInverted() const;

    // Overlay text (4 corners)
    void setOverlayText(OverlayCorner corner, const QString& text);
    void clearOverlayText(OverlayCorner corner);
    void clearAllOverlayText();
    void setOverlayVisible(bool visible);
    bool isOverlayVisible() const;
    void setOverlayColor(const QColor& color);
    void setOverlayFontSize(int size);

    // Crosshairs
    void setCrosshairsVisible(bool visible);
    bool areCrosshairsVisible() const;
    void setCrosshairPosition(double x, double y);

    // Rendering
    void render();
    void setBackgroundColor(const QColor& color);

    // Get components for advanced operations
    vtkRenderer* getRenderer() const { return m_renderer.Get(); }
    vtkImageData* getImageData() const;
    QVTKOpenGLNativeWidget* getWidget() const { return m_widget; }

    // State management
    void applyState(const ViewportState& state);
    ViewportState getState() const;

    // Image info
    void getImageDimensions(int& width, int& height) const;
    void getImageSpacing(double& spacingX, double& spacingY) const;

    /**
     * @brief Get the current image as a QImage for magnifier/screenshot.
     * @return QImage of the current image data, or null image if no image loaded
     */
    QImage getImageAsQImage() const;

    /**
     * @brief Convert widget coordinates to image pixel coordinates.
     * @param widgetPos Position in widget coordinates (pixels from top-left)
     * @return Position in image coordinates (pixels), or (-1,-1) if no image
     */
    QPointF widgetToImageCoords(const QPointF& widgetPos) const;

signals:
    /**
     * @brief Emitted when window/level changes.
     */
    void windowLevelChanged(double window, double level);

    /**
     * @brief Emitted when zoom changes.
     */
    void zoomChanged(double factor);

    /**
     * @brief Emitted when image data changes.
     */
    void imageChanged();

private:
    void initializePipeline();
    void initializeOverlayActors();
    void initializeCrosshairs();
    void updateOverlayPositions();
    void calculateAutoWindowLevel(vtkImageData* imageData, double& window, double& level);
    int cornerToIndex(OverlayCorner corner) const;

    QVTKOpenGLNativeWidget* m_widget;
    vtkSmartPointer<vtkGenericOpenGLRenderWindow> m_renderWindow;
    vtkSmartPointer<vtkRenderer> m_renderer;
    vtkSmartPointer<vtkImageActor> m_imageActor;
    vtkSmartPointer<vtkImageMapToWindowLevelColors> m_windowLevelFilter;
    vtkSmartPointer<vtkInteractorStyleImage> m_interactorStyle;

    // Overlay text actors (4 corners)
    std::array<vtkSmartPointer<vtkTextActor>, 4> m_overlayActors;
    bool m_overlayVisible = true;
    QColor m_overlayColor{0, 255, 0};  // Green
    int m_overlayFontSize = 12;

    // Crosshairs
    vtkSmartPointer<vtkActor2D> m_crosshairActorH;
    vtkSmartPointer<vtkActor2D> m_crosshairActorV;
    vtkSmartPointer<vtkLineSource> m_crosshairLineH;
    vtkSmartPointer<vtkLineSource> m_crosshairLineV;
    bool m_crosshairsVisible = false;

    // State
    double m_windowWidth = 0.0;
    double m_windowCenter = 0.0;
    double m_originalWindowWidth = 0.0;
    double m_originalWindowCenter = 0.0;
    double m_zoomFactor = 1.0;
    double m_panX = 0.0;
    double m_panY = 0.0;
    bool m_inverted = false;
    bool m_hasImage = false;

    // Image dimensions cache
    int m_imageWidth = 0;
    int m_imageHeight = 0;
    double m_spacingX = 1.0;
    double m_spacingY = 1.0;

    // Resize handling
    bool m_needsFitOnResize = false;
    int m_lastViewportWidth = 0;
    int m_lastViewportHeight = 0;

protected:
    bool eventFilter(QObject* watched, QEvent* event) override;
};

} // namespace Etrek::ImageViewer::Rendering

#endif // VTKVIEWPORTRENDERER_H
