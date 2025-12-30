#include "VtkViewportRenderer.h"
#include "ImageViewerTypes.h"

#include <QVTKOpenGLNativeWidget.h>
#include <vtkGenericOpenGLRenderWindow.h>
#include <vtkRenderer.h>
#include <vtkImageActor.h>
#include <vtkImageData.h>
#include <vtkImageMapToWindowLevelColors.h>
#include <vtkInteractorStyleImage.h>
#include <vtkTextActor.h>
#include <vtkTextProperty.h>
#include <vtkActor2D.h>
#include <vtkLineSource.h>
#include <vtkPolyDataMapper2D.h>
#include <vtkProperty2D.h>
#include <vtkCamera.h>
#include <vtkImageProperty.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkNew.h>
#include <vtkPointData.h>
#include <vtkDataArray.h>
#include <vtkCoordinate.h>

#include <algorithm>
#include <cmath>

namespace Etrek::ImageViewer::Rendering {

VtkViewportRenderer::VtkViewportRenderer(QVTKOpenGLNativeWidget* widget, QObject* parent)
    : QObject(parent)
    , m_widget(widget)
{
    initializePipeline();
    initializeOverlayActors();
    initializeCrosshairs();
}

VtkViewportRenderer::~VtkViewportRenderer() {
    // VTK smart pointers handle cleanup
    if (m_renderer) {
        m_renderer->RemoveAllViewProps();
    }
}

void VtkViewportRenderer::initializePipeline() {
    // Create render window
    m_renderWindow = vtkSmartPointer<vtkGenericOpenGLRenderWindow>::New();
    m_widget->setRenderWindow(m_renderWindow);

    // Create renderer
    m_renderer = vtkSmartPointer<vtkRenderer>::New();
    m_renderer->SetBackground(0.0, 0.0, 0.0);  // Black background
    m_renderWindow->AddRenderer(m_renderer);

    // Create window/level filter
    m_windowLevelFilter = vtkSmartPointer<vtkImageMapToWindowLevelColors>::New();

    // Create image actor
    m_imageActor = vtkSmartPointer<vtkImageActor>::New();
    m_imageActor->GetProperty()->SetInterpolationTypeToLinear();

    // Create interactor style for 2D images
    m_interactorStyle = vtkSmartPointer<vtkInteractorStyleImage>::New();
    if (m_renderWindow->GetInteractor()) {
        m_renderWindow->GetInteractor()->SetInteractorStyle(m_interactorStyle);
    }
}

void VtkViewportRenderer::initializeOverlayActors() {
    // Create text actors for each corner
    for (int i = 0; i < 4; ++i) {
        m_overlayActors[i] = vtkSmartPointer<vtkTextActor>::New();

        vtkTextProperty* textProp = m_overlayActors[i]->GetTextProperty();
        textProp->SetFontFamilyToCourier();
        textProp->SetFontSize(m_overlayFontSize);
        textProp->SetColor(m_overlayColor.redF(), m_overlayColor.greenF(), m_overlayColor.blueF());
        textProp->SetShadow(1);
        textProp->SetShadowOffset(1, -1);

        m_overlayActors[i]->SetVisibility(m_overlayVisible);
        m_renderer->AddActor2D(m_overlayActors[i]);
    }

    // Set justification for each corner
    // Top-left
    m_overlayActors[0]->GetTextProperty()->SetJustificationToLeft();
    m_overlayActors[0]->GetTextProperty()->SetVerticalJustificationToTop();

    // Top-right
    m_overlayActors[1]->GetTextProperty()->SetJustificationToRight();
    m_overlayActors[1]->GetTextProperty()->SetVerticalJustificationToTop();

    // Bottom-left
    m_overlayActors[2]->GetTextProperty()->SetJustificationToLeft();
    m_overlayActors[2]->GetTextProperty()->SetVerticalJustificationToBottom();

    // Bottom-right
    m_overlayActors[3]->GetTextProperty()->SetJustificationToRight();
    m_overlayActors[3]->GetTextProperty()->SetVerticalJustificationToBottom();

    updateOverlayPositions();
}

void VtkViewportRenderer::initializeCrosshairs() {
    // Horizontal line
    m_crosshairLineH = vtkSmartPointer<vtkLineSource>::New();
    vtkNew<vtkPolyDataMapper2D> mapperH;
    mapperH->SetInputConnection(m_crosshairLineH->GetOutputPort());

    vtkNew<vtkCoordinate> coordH;
    coordH->SetCoordinateSystemToNormalizedDisplay();
    mapperH->SetTransformCoordinate(coordH);

    m_crosshairActorH = vtkSmartPointer<vtkActor2D>::New();
    m_crosshairActorH->SetMapper(mapperH);
    m_crosshairActorH->GetProperty()->SetColor(0.0, 1.0, 0.0);  // Green
    m_crosshairActorH->GetProperty()->SetLineWidth(1.0);
    m_crosshairActorH->SetVisibility(m_crosshairsVisible);
    m_renderer->AddActor2D(m_crosshairActorH);

    // Vertical line
    m_crosshairLineV = vtkSmartPointer<vtkLineSource>::New();
    vtkNew<vtkPolyDataMapper2D> mapperV;
    mapperV->SetInputConnection(m_crosshairLineV->GetOutputPort());

    vtkNew<vtkCoordinate> coordV;
    coordV->SetCoordinateSystemToNormalizedDisplay();
    mapperV->SetTransformCoordinate(coordV);

    m_crosshairActorV = vtkSmartPointer<vtkActor2D>::New();
    m_crosshairActorV->SetMapper(mapperV);
    m_crosshairActorV->GetProperty()->SetColor(0.0, 1.0, 0.0);  // Green
    m_crosshairActorV->GetProperty()->SetLineWidth(1.0);
    m_crosshairActorV->SetVisibility(m_crosshairsVisible);
    m_renderer->AddActor2D(m_crosshairActorV);

    // Default crosshair position at center
    setCrosshairPosition(0.5, 0.5);
}

void VtkViewportRenderer::updateOverlayPositions() {
    // Positions in normalized viewport coordinates (0-1)
    const double margin = 0.02;

    // Top-left
    m_overlayActors[0]->SetPosition(margin, 1.0 - margin);

    // Top-right
    m_overlayActors[1]->SetPosition(1.0 - margin, 1.0 - margin);

    // Bottom-left
    m_overlayActors[2]->SetPosition(margin, margin);

    // Bottom-right
    m_overlayActors[3]->SetPosition(1.0 - margin, margin);
}

void VtkViewportRenderer::setImageData(vtkImageData* imageData, bool autoWL) {
    if (!imageData) {
        clearImage();
        return;
    }

    // Get image dimensions
    int dims[3];
    imageData->GetDimensions(dims);
    m_imageWidth = dims[0];
    m_imageHeight = dims[1];

    // Get spacing
    double spacing[3];
    imageData->GetSpacing(spacing);
    m_spacingX = spacing[0];
    m_spacingY = spacing[1];

    // Set up window/level filter
    m_windowLevelFilter->SetInputData(imageData);

    if (autoWL) {
        autoWindowLevel();
    }

    m_windowLevelFilter->Update();

    // Connect to image actor
    m_imageActor->SetInputData(m_windowLevelFilter->GetOutput());

    // Add actor to renderer if not already added
    if (!m_renderer->HasViewProp(m_imageActor)) {
        m_renderer->AddActor(m_imageActor);
    }

    m_hasImage = true;

    // Reset camera to fit image
    fitToWindow();

    emit imageChanged();
}

void VtkViewportRenderer::clearImage() {
    if (m_renderer && m_imageActor) {
        m_renderer->RemoveActor(m_imageActor);
    }
    m_hasImage = false;
    m_imageWidth = 0;
    m_imageHeight = 0;
    render();
    emit imageChanged();
}

bool VtkViewportRenderer::hasImage() const {
    return m_hasImage;
}

void VtkViewportRenderer::setWindowLevel(double window, double level) {
    m_windowWidth = window;
    m_windowCenter = level;

    if (m_windowLevelFilter) {
        m_windowLevelFilter->SetWindow(m_inverted ? -window : window);
        m_windowLevelFilter->SetLevel(level);
        m_windowLevelFilter->Update();
    }

    render();
    emit windowLevelChanged(m_windowWidth, m_windowCenter);
}

void VtkViewportRenderer::getWindowLevel(double& window, double& level) const {
    window = m_windowWidth;
    level = m_windowCenter;
}

void VtkViewportRenderer::resetWindowLevel() {
    setWindowLevel(m_originalWindowWidth, m_originalWindowCenter);
}

void VtkViewportRenderer::autoWindowLevel() {
    if (!m_windowLevelFilter || !m_windowLevelFilter->GetInput()) {
        return;
    }

    vtkImageData* imageData = vtkImageData::SafeDownCast(m_windowLevelFilter->GetInput());
    calculateAutoWindowLevel(imageData, m_windowWidth, m_windowCenter);

    m_originalWindowWidth = m_windowWidth;
    m_originalWindowCenter = m_windowCenter;

    m_windowLevelFilter->SetWindow(m_inverted ? -m_windowWidth : m_windowWidth);
    m_windowLevelFilter->SetLevel(m_windowCenter);
    m_windowLevelFilter->Update();

    emit windowLevelChanged(m_windowWidth, m_windowCenter);
}

void VtkViewportRenderer::calculateAutoWindowLevel(vtkImageData* imageData,
                                                    double& window, double& level) {
    if (!imageData || !imageData->GetPointData() ||
        !imageData->GetPointData()->GetScalars()) {
        window = 256;
        level = 128;
        return;
    }

    // Get scalar range
    double range[2];
    imageData->GetScalarRange(range);

    double minVal = range[0];
    double maxVal = range[1];

    // Calculate window/level from range
    window = maxVal - minVal;
    level = (maxVal + minVal) / 2.0;

    // Ensure window is at least 1
    if (window < 1.0) {
        window = 1.0;
    }
}

void VtkViewportRenderer::setZoom(double factor) {
    if (!m_renderer) return;

    m_zoomFactor = std::clamp(factor, 0.1, 20.0);

    vtkCamera* camera = m_renderer->GetActiveCamera();
    if (camera) {
        // Calculate new parallel scale based on zoom
        double baseScale = std::max(m_imageWidth, m_imageHeight) / 2.0;
        camera->SetParallelScale(baseScale / m_zoomFactor);
    }

    render();
    emit zoomChanged(m_zoomFactor);
}

double VtkViewportRenderer::getZoom() const {
    return m_zoomFactor;
}

void VtkViewportRenderer::setPan(double deltaX, double deltaY) {
    if (!m_renderer) return;

    m_panX += deltaX;
    m_panY += deltaY;

    vtkCamera* camera = m_renderer->GetActiveCamera();
    if (camera) {
        double* focalPoint = camera->GetFocalPoint();
        double* position = camera->GetPosition();

        camera->SetFocalPoint(focalPoint[0] - deltaX, focalPoint[1] - deltaY, focalPoint[2]);
        camera->SetPosition(position[0] - deltaX, position[1] - deltaY, position[2]);
    }

    render();
}

void VtkViewportRenderer::getPan(double& panX, double& panY) const {
    panX = m_panX;
    panY = m_panY;
}

void VtkViewportRenderer::resetCamera() {
    m_zoomFactor = 1.0;
    m_panX = 0.0;
    m_panY = 0.0;

    fitToWindow();
    emit zoomChanged(m_zoomFactor);
}

void VtkViewportRenderer::fitToWindow() {
    if (!m_renderer) return;

    m_renderer->ResetCamera();

    vtkCamera* camera = m_renderer->GetActiveCamera();
    if (camera) {
        camera->ParallelProjectionOn();
    }

    render();
}

void VtkViewportRenderer::setInverted(bool inverted) {
    if (m_inverted == inverted) return;

    m_inverted = inverted;

    if (m_windowLevelFilter) {
        m_windowLevelFilter->SetWindow(m_inverted ? -m_windowWidth : m_windowWidth);
        m_windowLevelFilter->Update();
    }

    render();
}

bool VtkViewportRenderer::isInverted() const {
    return m_inverted;
}

void VtkViewportRenderer::setOverlayText(OverlayCorner corner, const QString& text) {
    int index = cornerToIndex(corner);
    if (index >= 0 && index < 4 && m_overlayActors[index]) {
        m_overlayActors[index]->SetInput(text.toStdString().c_str());
        render();
    }
}

void VtkViewportRenderer::clearOverlayText(OverlayCorner corner) {
    int index = cornerToIndex(corner);
    if (index >= 0 && index < 4 && m_overlayActors[index]) {
        m_overlayActors[index]->SetInput("");
        render();
    }
}

void VtkViewportRenderer::clearAllOverlayText() {
    for (auto& actor : m_overlayActors) {
        if (actor) {
            actor->SetInput("");
        }
    }
    render();
}

void VtkViewportRenderer::setOverlayVisible(bool visible) {
    m_overlayVisible = visible;
    for (auto& actor : m_overlayActors) {
        if (actor) {
            actor->SetVisibility(visible);
        }
    }
    render();
}

bool VtkViewportRenderer::isOverlayVisible() const {
    return m_overlayVisible;
}

void VtkViewportRenderer::setOverlayColor(const QColor& color) {
    m_overlayColor = color;
    for (auto& actor : m_overlayActors) {
        if (actor) {
            actor->GetTextProperty()->SetColor(color.redF(), color.greenF(), color.blueF());
        }
    }
    render();
}

void VtkViewportRenderer::setOverlayFontSize(int size) {
    m_overlayFontSize = size;
    for (auto& actor : m_overlayActors) {
        if (actor) {
            actor->GetTextProperty()->SetFontSize(size);
        }
    }
    render();
}

void VtkViewportRenderer::setCrosshairsVisible(bool visible) {
    m_crosshairsVisible = visible;
    if (m_crosshairActorH) m_crosshairActorH->SetVisibility(visible);
    if (m_crosshairActorV) m_crosshairActorV->SetVisibility(visible);
    render();
}

bool VtkViewportRenderer::areCrosshairsVisible() const {
    return m_crosshairsVisible;
}

void VtkViewportRenderer::setCrosshairPosition(double x, double y) {
    // x, y in normalized coordinates (0-1)
    if (m_crosshairLineH) {
        m_crosshairLineH->SetPoint1(0.0, y, 0.0);
        m_crosshairLineH->SetPoint2(1.0, y, 0.0);
        m_crosshairLineH->Update();
    }
    if (m_crosshairLineV) {
        m_crosshairLineV->SetPoint1(x, 0.0, 0.0);
        m_crosshairLineV->SetPoint2(x, 1.0, 0.0);
        m_crosshairLineV->Update();
    }
    render();
}

void VtkViewportRenderer::render() {
    if (m_renderWindow) {
        m_renderWindow->Render();
    }
}

void VtkViewportRenderer::setBackgroundColor(const QColor& color) {
    if (m_renderer) {
        m_renderer->SetBackground(color.redF(), color.greenF(), color.blueF());
        render();
    }
}

vtkImageData* VtkViewportRenderer::getImageData() const {
    if (m_windowLevelFilter) {
        return vtkImageData::SafeDownCast(m_windowLevelFilter->GetInput());
    }
    return nullptr;
}

void VtkViewportRenderer::applyState(const ViewportState& state) {
    setWindowLevel(state.windowWidth, state.windowCenter);
    setZoom(state.zoomFactor);
    // Pan is relative, so we need to calculate delta
    double deltaX = state.panX - m_panX;
    double deltaY = state.panY - m_panY;
    setPan(deltaX, deltaY);
    setInverted(state.isInverted);
}

ViewportState VtkViewportRenderer::getState() const {
    ViewportState state;
    state.zoomFactor = m_zoomFactor;
    state.panX = m_panX;
    state.panY = m_panY;
    state.windowWidth = m_windowWidth;
    state.windowCenter = m_windowCenter;
    state.isWindowLevelSet = m_hasImage;
    state.originalWindowWidth = m_originalWindowWidth;
    state.originalWindowCenter = m_originalWindowCenter;
    state.isInverted = m_inverted;
    return state;
}

void VtkViewportRenderer::getImageDimensions(int& width, int& height) const {
    width = m_imageWidth;
    height = m_imageHeight;
}

void VtkViewportRenderer::getImageSpacing(double& spacingX, double& spacingY) const {
    spacingX = m_spacingX;
    spacingY = m_spacingY;
}

int VtkViewportRenderer::cornerToIndex(OverlayCorner corner) const {
    switch (corner) {
        case OverlayCorner::TOP_LEFT:     return 0;
        case OverlayCorner::TOP_RIGHT:    return 1;
        case OverlayCorner::BOTTOM_LEFT:  return 2;
        case OverlayCorner::BOTTOM_RIGHT: return 3;
        default: return -1;
    }
}

} // namespace Etrek::ImageViewer::Rendering
