# ImageViewer Technical Documentation

This document provides comprehensive technical details about the ImageViewer component in the Etrek medical imaging application. It is intended for future development reference and code review.

---

## Table of Contents

1. [Architecture Overview](#1-architecture-overview)
2. [Module Structure](#2-module-structure)
3. [Integration with Application](#3-integration-with-application)
4. [View Layer Components](#4-view-layer-components)
5. [Tool System](#5-tool-system)
6. [Rendering Layer (VTK)](#6-rendering-layer-vtk)
7. [Service Layer](#7-service-layer)
8. [Layout System](#8-layout-system)
9. [User Interactions](#9-user-interactions)
10. [Signal/Slot Data Flow](#10-signalslot-data-flow)
11. [Design Patterns](#11-design-patterns)
12. [File Reference](#12-file-reference)
13. [Build Configuration](#13-build-configuration)
14. [Extension Points](#14-extension-points)

---

## 1. Architecture Overview

The ImageViewer is a **modular medical image viewing system** built on:
- **Qt 6.5.3** - UI framework
- **VTK 9.5** - 3D/2D visualization and rendering
- **DCMTK 3.6.9** - DICOM protocol and parsing

### Design Philosophy

The component follows the **MVD (Model-View-Delegate)** pattern with clear separation of concerns:
- **View** (`ImageViewerPage`) - Pure UI presentation
- **Delegate** (`ImageViewerPageDelegate`) - Business logic orchestration
- **Model** - Data entities and state management

### High-Level Architecture

```
┌─────────────────────────────────────────────────────────────────────┐
│                         Application Layer                           │
│  ┌─────────────────────────┐    ┌─────────────────────────────────┐ │
│  │ ImageViewerPageBuilder  │───▶│   ImageViewerPageDelegate       │ │
│  └─────────────────────────┘    │   (Business Logic)              │ │
│                                 └────────────┬────────────────────┘ │
└──────────────────────────────────────────────┼──────────────────────┘
                                               │
┌──────────────────────────────────────────────┼──────────────────────┐
│                          View Layer          │                       │
│  ┌───────────────────────────────────────────┼────────────────────┐ │
│  │                    ImageViewerPage        │                    │ │
│  │  ┌──────────────┐  ┌──────────────────────┼──┐  ┌────────────┐ │ │
│  │  │ImageToolPanel│  │    ViewportGrid      │  │  │SeriesThumb-│ │ │
│  │  │  (Toolbar)   │  │  ┌────────┬────────┐ │  │  │nailPanel   │ │ │
│  │  └──────────────┘  │  │Viewport│Viewport│ │  │  └────────────┘ │ │
│  │                    │  │   0    │   1    │ │  │                 │ │
│  │                    │  ├────────┼────────┤ │  │                 │ │
│  │                    │  │Viewport│Viewport│ │  │                 │ │
│  │                    │  │   2    │   3    │ │  │                 │ │
│  │                    │  └────────┴────────┘ │  │                 │ │
│  │                    └─────────────────────────┘                 │ │
│  └────────────────────────────────────────────────────────────────┘ │
└─────────────────────────────────────────────────────────────────────┘
                                               │
┌──────────────────────────────────────────────┼──────────────────────┐
│                      ImageViewer Module      │                       │
│  ┌────────────────┐  ┌───────────────────────┼─┐  ┌───────────────┐ │
│  │     Tools      │  │    Rendering          │ │  │   Services    │ │
│  │  ┌──────────┐  │  │  ┌──────────────────┐ │ │  │ ┌───────────┐ │ │
│  │  │ ZoomTool │  │  │  │VtkViewportRender │ │ │  │ │ImageLoader│ │ │
│  │  │ PanTool  │  │  │  │      [0..3]      │ │ │  │ │ Service   │ │ │
│  │  │ W/L Tool │  │  │  └──────────────────┘ │ │  │ └───────────┘ │ │
│  │  │RulerTool │  │  │  ┌──────────────────┐ │ │  │ ┌───────────┐ │ │
│  │  │AngleTool │  │  │  │MultiViewportMgr  │ │ │  │ │DicomParser│ │ │
│  │  │ResetTool │  │  │  └──────────────────┘ │ │  │ │ Service   │ │ │
│  │  └──────────┘  │  └─────────────────────────┘  │ └───────────┘ │ │
│  └────────────────┘                               └───────────────┘ │
└─────────────────────────────────────────────────────────────────────┘
```

---

## 2. Module Structure

### Directory Layout

```
ImageViewer/
├── CMakeLists.txt              # Module build configuration
├── Tool/                       # Image manipulation tools
│   ├── IImageTool.h/cpp        # Base tool interface
│   ├── ZoomTool.h/cpp          # Zoom functionality
│   ├── PanTool.h/cpp           # Pan functionality
│   ├── WindowLevelTool.h/cpp   # Contrast/brightness
│   ├── RulerTool.h/cpp         # Distance measurement
│   ├── AngleTool.h/cpp         # Angle measurement
│   └── ResetTool.h/cpp         # View reset
├── Rendering/                  # VTK rendering pipeline
│   ├── VtkViewportRenderer.h/cpp     # Single viewport renderer
│   └── MultiViewportManager.h/cpp    # Multi-viewport coordinator
├── Service/                    # Image loading services
│   ├── ImageLoaderService.h/cpp      # Unified image loader
│   └── DicomParserService.h/cpp      # DCMTK-based DICOM parser
└── Data/                       # Data entities
    └── Entity/
        └── ImageDisplayParams.h      # Display parameters

View/
├── Page/
│   └── ImageViewerPage.h/cpp/ui      # Main page widget
├── Widget/
│   ├── ViewportGrid.h/cpp/ui         # Viewport grid container
│   ├── ImageViewport.h/cpp/ui        # Single viewport wrapper
│   ├── ImageToolPanel.h/cpp/ui       # Left toolbar panel
│   └── SeriesThumbnailPanel.h/cpp/ui # Right thumbnail panel
└── Asset/Icon/                       # Tool and layout icons

Application/
├── Builder/
│   └── ImageViewerPageBuilder.h/cpp  # Widget+Delegate builder
└── Delegate/
    └── ImageViewerPageDelegate.h/cpp # Business logic

Common/Include/ImageViewer/
├── IImageViewerContext.h       # Inter-page communication interface
├── ImageViewerTypes.h          # Enums and data structures
└── ViewportLayout.h            # Layout configuration
```

---

## 3. Integration with Application

### Context Interface

The `IImageViewerContext` interface enables inter-page communication for passing image data to ImageViewer:

```cpp
enum class ImageSourceType {
    FILE,       // Load from file path
    STUDY,      // Load from database study ID
    SERIES,     // Load from database series ID
    RAW_DATA    // Direct image data from ExamPage
};

class IImageViewerContext {
public:
    virtual ImageSourceType sourceType() const = 0;
    virtual QString filePath() const = 0;
    virtual int studyId() const = 0;
    virtual int seriesId() const = 0;
    virtual QByteArray rawData() const = 0;
    virtual DicomMetadata metadata() const = 0;
};
```

### Builder Pattern Integration

The `ImageViewerPageBuilder` creates the widget and delegate together with dependency injection:

```cpp
class ImageViewerPageBuilder : public IWidgetDelegateBuilder<ImageViewerPage, ImageViewerPageDelegate> {
public:
    std::pair<ImageViewerPage*, ImageViewerPageDelegate*>
        build(const DelegateParameter& params,
              QWidget* parentWidget = nullptr,
              QObject* parentDelegate = nullptr) override;
};
```

**Construction Steps:**
1. Create `ImageViewerPage` widget
2. Create `DicomRepository` from database connection
3. Create `ImageViewerPageDelegate` with injected dependencies:
   - UI page reference
   - DicomRepository
   - DatabaseConnectionSetting
   - IContextManager (weak_ptr)
4. Return widget-delegate pair

### Delegate Dependencies

```cpp
class ImageViewerPageDelegate : public QObject, public IDelegate, public IPageAction {
private:
    // UI
    ImageViewerPage* m_ui;

    // Repositories
    std::shared_ptr<DicomRepository> m_dicomRepo;
    std::shared_ptr<DatabaseConnectionSetting> m_dbConnection;
    std::weak_ptr<IContextManager> m_contextManager;

    // Services
    std::unique_ptr<ImageLoaderService> m_imageLoader;
    std::unique_ptr<MultiViewportManager> m_viewportManager;

    // Tools
    std::unique_ptr<WindowLevelTool> m_windowLevelTool;
    std::unique_ptr<ZoomTool> m_zoomTool;
    std::unique_ptr<PanTool> m_panTool;
    std::unique_ptr<RulerTool> m_rulerTool;
    std::unique_ptr<AngleTool> m_angleTool;
    std::unique_ptr<ResetTool> m_resetTool;

    // State
    ToolType m_currentTool;
    ViewportLayout m_currentLayout;
    int m_activeViewportIndex;
    std::array<DicomMetadata, 4> m_viewportMetadata;
    std::array<ViewportState, 4> m_viewportStates;
};
```

### IPageAction Interface

The delegate implements `IPageAction` for page lifecycle management:

```cpp
// IPageAction methods
void apply();   // Save intermediate state
void accept();  // Complete viewing session, emit closeRequested()
void reject();  // Cancel session, emit closeRequested()
```

---

## 4. View Layer Components

### 4.1 ImageViewerPage

**Location:** `View/Page/ImageViewerPage.h/cpp/ui`

The main page widget containing the entire image viewer interface.

**Layout Structure:**
```
┌─────────────────────────────────────────────────────────┐
│                   ImageViewerPage                       │
│  ┌─────────┬───────────────────────────┬─────────────┐ │
│  │  Tool   │                           │  Thumbnail  │ │
│  │  Panel  │      ViewportGrid         │    Panel    │ │
│  │  (~60px)│       (expandable)        │  (~150px)   │ │
│  │         │                           │ (collapsible)│ │
│  └─────────┴───────────────────────────┴─────────────┘ │
│            └─────── QSplitter (resizable) ──────┘      │
└─────────────────────────────────────────────────────────┘
```

**Styling:**
- Dark theme: `background-color: rgb(30, 30, 30);`
- Splitter initial sizes: `{60, 700, 150}`

**Signals:**
```cpp
// Tool selection
void toolSelected(ToolType type);
void layoutSelected(ViewportLayout layout);

// Action buttons
void openFileRequested();
void invertRequested();
void resetViewRequested();
void fitToWindowRequested();

// Viewport interactions
void activeViewportChanged(int index);
void viewportMousePressed(int index, QPointF pos, Qt::MouseButton, Qt::KeyboardModifiers);
void viewportMouseMoved(int index, QPointF pos, Qt::MouseButton, Qt::KeyboardModifiers);
void viewportMouseReleased(int index, QPointF pos, Qt::MouseButton, Qt::KeyboardModifiers);
void viewportMouseWheelScrolled(int index, int delta, QPointF pos, Qt::KeyboardModifiers);
void viewportMouseDoubleClicked(int index, QPointF pos, Qt::MouseButton);

// Thumbnails
void thumbnailClicked(int id, bool isSeries);
void thumbnailDoubleClicked(int id, bool isSeries);

// Drag & drop
void filesDropped(QStringList paths);
```

**Keyboard Shortcuts:**
| Key | Action |
|-----|--------|
| `Z` | Zoom tool |
| `P` | Pan tool |
| `W` | Window/Level tool |
| `R` | Ruler tool |
| `A` | Angle tool |
| `I` | Invert image |
| `F` | Fit to window |
| `1` | Single (1x1) layout |
| `2` | 1x2 layout |
| `4` | 2x2 layout |
| `Ctrl+O` | Open file |
| `Delete` / `Backspace` | Delete selected ruler/angle |
| `Ctrl+Delete` | Clear all rulers/angles |
| `Escape` | Close ImageViewer |

**Drag & Drop:** Accepts local file URLs, emits `filesDropped()` signal.

### 4.2 ViewportGrid

**Location:** `View/Widget/ViewportGrid.h/cpp/ui`

Container managing 4 `ImageViewport` widgets in configurable layouts.

**Key Methods:**
```cpp
ImageViewport* getViewport(int index);           // Access viewport (0-3)
QVTKOpenGLNativeWidget* getVtkWidget(int index); // Get VTK render widget
std::array<QVTKOpenGLNativeWidget*, 4> getAllVtkWidgets();

void setViewportLayout(ViewportLayout layout);   // Switch layout
int activeViewportIndex() const;
void setActiveViewport(int index);
int visibleViewportCount() const;                // Returns 1, 2, or 4
```

**Grid Configuration:**
- Container margins: 0
- Spacing: 2px between viewports
- Stretch factors: Viewports expand to fill space

### 4.3 ImageViewport

**Location:** `View/Widget/ImageViewport.h/cpp/ui`

Single viewport wrapper containing a `QVTKOpenGLNativeWidget`.

**Styling:**
- Active: Green border `2px solid #00FF00`
- Inactive: Gray border `1px solid #404040`

**Signals:**
```cpp
void activated(int index);
void mousePressed(int index, QPointF pos, Qt::MouseButton, Qt::KeyboardModifiers);
void mouseMoved(int index, QPointF pos, Qt::MouseButton, Qt::KeyboardModifiers);
void mouseReleased(int index, QPointF pos, Qt::MouseButton, Qt::KeyboardModifiers);
void mouseWheelScrolled(int index, int delta, QPointF pos, Qt::KeyboardModifiers);
void mouseDoubleClicked(int index, QPointF pos, Qt::MouseButton);
```

### 4.4 ImageToolPanel

**Location:** `View/Widget/ImageToolPanel.h/cpp/ui`

Left toolbar panel with tool and layout buttons.

**Tool Buttons (6 total):**
| Tool | Icon | Description |
|------|------|-------------|
| Zoom | `zoom.png` | Zoom in/out |
| Pan | `pan.png` | Move image |
| Window/Level | `windowlevel.png` | Adjust contrast/brightness |
| Ruler | `ruler.png` | Measure distance |
| Angle | `angle.png` | Measure angle |
| Reset | `reset.png` | Reset view |

**Layout Buttons (3 total):**
| Layout | Icon | Description |
|--------|------|-------------|
| 1x1 | `layout1x1.png` | Single viewport |
| 1x2 | `layout1x2.png` | Two viewports horizontal |
| 2x2 | `layout2x2.png` | Four viewports grid |

**Action Buttons:**
- Open File
- Invert
- Fit to Window

**Button Groups:**
- `m_toolButtonGroup` (exclusive) - Only one tool active at a time
- `m_layoutButtonGroup` (exclusive) - Only one layout active at a time

### 4.5 SeriesThumbnailPanel

**Location:** `View/Widget/SeriesThumbnailPanel.h/cpp/ui`

Right collapsible panel showing series/image thumbnails.

**ThumbnailInfo Structure:**
```cpp
struct ThumbnailInfo {
    int id;                 // Series or Instance ID
    QString label;          // Display text
    QString description;    // Optional description
    QPixmap thumbnail;      // Thumbnail image
    int imageCount;         // Images in series
    bool isSeries;          // Boolean flag
};
```

**Key Methods:**
```cpp
void clear();
void addThumbnail(const ThumbnailInfo& info);
void setThumbnails(const QVector<ThumbnailInfo>& thumbnails);
int selectedId() const;
void setSelectedId(int id);
void setExpanded(bool expanded);
bool isExpanded() const;
```

---

## 5. Tool System

### 5.1 IImageTool Base Interface

**Location:** `ImageViewer/Tool/IImageTool.h`

```cpp
class IImageTool : public QObject {
public:
    // Properties
    virtual ToolType type() const = 0;
    virtual QString name() const = 0;
    virtual QString tooltip() const = 0;
    virtual QIcon icon() const = 0;
    virtual bool isActive() const = 0;

    // Lifecycle
    virtual void activate() = 0;
    virtual void deactivate() = 0;
    virtual void reset() = 0;

    // Event handlers
    virtual void onMousePress(QPointF pos, Qt::MouseButton button) = 0;
    virtual void onMouseMove(QPointF pos) = 0;
    virtual void onMouseRelease(QPointF pos, Qt::MouseButton button) = 0;
    virtual void onMouseWheel(int delta, QPointF pos) = 0;

signals:
    void zoomRequested(double factor, QPointF centerPos);
    void panRequested(double deltaX, double deltaY);
    void windowLevelChanged(double window, double level);
    void measurementLineUpdated(const MeasurementLine& line);
    void measurementLineCompleted(const MeasurementLine& line);
    void measurementAngleUpdated(const MeasurementAngle& angle);
    void measurementAngleCompleted(const MeasurementAngle& angle);
    void resetRequested();
    void cursorChanged(Qt::CursorShape cursor);
};
```

### 5.2 ZoomTool

**Location:** `ImageViewer/Tool/ZoomTool.h/cpp`

**Interaction:**
- Left drag up → Zoom in
- Left drag down → Zoom out
- Mouse wheel → Zoom in/out
- Double-click → Fit to window

**Configuration:**
- Zoom limits: 0.1x to 20x (default)
- Sensitivity: 0.01 (default)

**Signal:** `zoomRequested(double factor, QPointF centerPos)`

### 5.3 PanTool

**Location:** `ImageViewer/Tool/PanTool.h/cpp`

**Interaction:**
- Left drag → Pan image
- Middle drag → Always pans (independent of tool selection)

**Configuration:**
- Sensitivity: 1.0 (default)

**Signal:** `panRequested(double deltaX, double deltaY)`

### 5.4 WindowLevelTool

**Location:** `ImageViewer/Tool/WindowLevelTool.h/cpp`

**Interaction:**
- Horizontal drag → Adjust window width (contrast)
- Vertical drag → Adjust window center/level (brightness)
- Mouse wheel → Quick adjustment

**Default Values:**
- Window: 256
- Level: 128

**Signal:** `windowLevelChanged(double window, double level)`

**Purpose:** Critical for medical imaging - adjusts contrast/brightness for different tissue visualization.

### 5.5 RulerTool

**Location:** `ImageViewer/Tool/RulerTool.h/cpp`

**Interaction:**
- Click and drag → Draw measurement line
- Release → Complete measurement
- Click on handle → Start dragging handle
- Drag handle → Resize ruler
- Click on line → Select ruler
- Delete key → Delete selected ruler
- Ctrl+Delete → Clear all rulers

**Features:**
- Calculates distance in mm AND pixels
- Uses pixel spacing from DICOM metadata
- Maintains vector of completed measurements
- Interactive editing with draggable handles
- Visual feedback (hover highlighting, selection)

**Key Methods:**
```cpp
void setPixelSpacing(double spacingX, double spacingY);
QVector<MeasurementLine> getMeasurements() const;
void clearMeasurements();
MeasurementLine currentMeasurement() const;
bool isDrawing() const;

// Interactive editing
bool hitTestHandle(const QPointF& pos, int& rulerId, bool& isStartHandle) const;
int hitTestRuler(const QPointF& pos) const;
void selectRuler(int rulerId);
int selectedRuler() const;
void deleteRuler(int rulerId);
void deleteSelectedRuler();
```

**Signals:**
- `measurementLineUpdated(MeasurementLine)` - Live updates while drawing
- `measurementLineCompleted(MeasurementLine)` - When released
- `rulerSelected(int rulerId)` - When ruler is selected
- `rulerModified(int rulerId, MeasurementLine)` - When ruler is resized
- `rulerDeleted(int rulerId)` - When ruler is deleted
- `measurementsChanged()` - General refresh signal
- `handleHovered(int rulerId, bool isStartHandle)` - Handle hover state
- `handleHoverCleared()` - No handle hovered

**Edit States:**
```cpp
enum class RulerEditState {
    NONE,           // Not editing
    DRAWING_NEW,    // Drawing new ruler
    DRAGGING_START, // Dragging start handle
    DRAGGING_END    // Dragging end handle
};
```

### 5.6 AngleTool

**Location:** `ImageViewer/Tool/AngleTool.h/cpp`

**Interaction:**
1. Click → Place first point (arm 1 endpoint)
2. Click → Place vertex (middle point where angle is measured)
3. Click → Place third point (arm 2 endpoint) and complete angle
4. Click on handle → Start dragging handle
5. Drag handle → Resize angle
6. Click on arm → Select angle
7. Delete key → Delete selected angle
8. Ctrl+Delete → Clear all angles
9. Right-click → Cancel current placement

**Features:**
- Three-point angle measurement
- Visible arc at vertex showing measured angle in degrees
- Interactive editing with draggable handles at all three points
- Visual feedback (hover highlighting, selection)
- Dynamic angle recalculation during drag

**Key Methods:**
```cpp
QVector<MeasurementAngle> getMeasurements() const;
void clearMeasurements();
MeasurementAngle currentMeasurement() const;
AngleEditState editState() const;
bool isPlacing() const;

// Interactive editing
bool hitTestHandle(const QPointF& pos, int& angleId, AngleHandleType& handleType) const;
int hitTestAngle(const QPointF& pos) const;
void selectAngle(int angleId);
int selectedAngle() const;
void deleteAngle(int angleId);
void deleteSelectedAngle();
```

**Signals:**
- `measurementAngleUpdated(MeasurementAngle)` - Live updates during placement
- `measurementAngleCompleted(MeasurementAngle)` - When third point placed
- `angleSelected(int angleId)` - When angle is selected
- `angleModified(int angleId, MeasurementAngle)` - When angle is resized
- `angleDeleted(int angleId)` - When angle is deleted
- `measurementsChanged()` - General refresh signal
- `handleHovered(int angleId, AngleHandleType)` - Handle hover state
- `handleHoverCleared()` - No handle hovered

**Edit States:**
```cpp
enum class AngleEditState {
    NONE,             // Not editing
    PLACING_POINT1,   // Placing first point
    PLACING_VERTEX,   // Placing vertex
    PLACING_POINT2,   // Placing third point
    DRAGGING_POINT1,  // Dragging first point handle
    DRAGGING_VERTEX,  // Dragging vertex handle
    DRAGGING_POINT2   // Dragging third point handle
};

enum class AngleHandleType {
    NONE,
    POINT1,
    VERTEX,
    POINT2
};
```

### 5.7 ResetTool

**Location:** `ImageViewer/Tool/ResetTool.h/cpp`

**Purpose:** "Instant action" tool - clicking triggers reset.

**Signal:** `resetRequested()`

---

## 6. Rendering Layer (VTK)

### 6.1 VtkViewportRenderer

**Location:** `ImageViewer/Rendering/VtkViewportRenderer.h/cpp`

Manages VTK rendering pipeline for a single viewport.

**VTK Pipeline Components:**
```
┌─────────────────────────────────────────────────────────┐
│                 VTK Rendering Pipeline                  │
│                                                         │
│  vtkImageData                                           │
│       │                                                 │
│       ▼                                                 │
│  vtkImageMapToWindowLevelColors (window/level filter)   │
│       │                                                 │
│       ▼                                                 │
│  vtkImageActor (image display)                          │
│       │                                                 │
│       ▼                                                 │
│  vtkRenderer (scene)                                    │
│       │                                                 │
│       ▼                                                 │
│  vtkGenericOpenGLRenderWindow                           │
│       │                                                 │
│       ▼                                                 │
│  QVTKOpenGLNativeWidget (Qt widget)                     │
└─────────────────────────────────────────────────────────┘
```

**Core Functionality:**

**Image Display:**
```cpp
void setImageData(vtkImageData* data, bool autoWindowLevel = true);
void clearImage();
bool hasImage() const;
vtkImageData* getImageData() const;
```

**Window/Level Operations:**
```cpp
void setWindowLevel(double window, double level);
void getWindowLevel(double& window, double& level) const;
void resetWindowLevel();
void autoWindowLevel();  // Calculate from image histogram
```

**Camera/View Operations:**
```cpp
void setZoom(double factor);  // 1.0 = 100%
double getZoom() const;
void setPan(double deltaX, double deltaY);
void getPan(double& panX, double& panY) const;
void resetCamera();
void fitToWindow();
```

**Image Manipulation:**
```cpp
void setInverted(bool inverted);
bool isInverted() const;
```

**Overlay Text (4 corners):**
```cpp
void setOverlayText(OverlayCorner corner, const QString& text);
void clearOverlayText(OverlayCorner corner);
void clearAllOverlayText();
void setOverlayVisible(bool visible);
void setOverlayColor(const QColor& color);  // Default: green #00FF00
void setOverlayFontSize(int size);
```

**Crosshairs:**
```cpp
void setCrosshairsVisible(bool visible);
void setCrosshairPosition(double x, double y);
```

**State Management:**
```cpp
void applyState(const ViewportState& state);
ViewportState getState() const;
void getImageDimensions(int& width, int& height) const;
void getImageSpacing(double& spacingX, double& spacingY) const;
```

**Signals:**
```cpp
void windowLevelChanged(double window, double level);
void zoomChanged(double factor);
void imageChanged();
```

### 6.2 MultiViewportManager

**Location:** `ImageViewer/Rendering/MultiViewportManager.h/cpp`

Manages up to 4 `VtkViewportRenderer` instances with synchronized operations.

**Initialization:**
```cpp
void initialize(std::array<QVTKOpenGLNativeWidget*, 4> widgets);
bool isInitialized() const;
```

**Layout Management:**
```cpp
void setLayout(ViewportLayout layout);
ViewportLayout currentLayout() const;
int visibleViewportCount() const;  // Returns 1, 2, or 4
```

**Viewport Access:**
```cpp
VtkViewportRenderer* getRenderer(int index);
int activeViewportIndex() const;
void setActiveViewport(int index);
VtkViewportRenderer* activeRenderer();
```

**Image Operations:**
```cpp
void setImageData(int viewportIndex, vtkImageData* data, bool autoWindowLevel = true);
void clearViewport(int viewportIndex);
void clearAllViewports();
```

**Linked Mode (synchronized viewports):**
```cpp
void setLinkedMode(bool linked);
bool isLinkedMode() const;
void synchronizeWindowLevel();  // Apply active viewport's W/L to all
void synchronizeZoom();         // Apply active viewport's zoom to all
void synchronizePan();          // Apply active viewport's pan to all
```

**Batch Operations:**
```cpp
void forEachVisibleViewport(std::function<void(VtkViewportRenderer*, int)> func);
void forEachViewport(std::function<void(VtkViewportRenderer*, int)> func);
void renderAll();
void renderViewport(int index);
```

**Signals:**
```cpp
void layoutChanged(ViewportLayout layout);
void activeViewportChanged(int index);
void viewportWindowLevelChanged(int viewportIndex, double window, double level);
void viewportZoomChanged(int viewportIndex, double factor);
void viewportImageChanged(int viewportIndex);
```

---

## 7. Service Layer

### 7.1 ImageLoaderService

**Location:** `ImageViewer/Service/ImageLoaderService.h/cpp`

Unified image loading service supporting multiple formats.

**Supported Formats:**
- DICOM (`.dcm`, `.dicom`, files without extension)
- PNG
- JPEG
- JPEG2000 (via DCMTK if available)
- BMP
- TIFF

**FullImageLoadResult Structure:**
```cpp
struct FullImageLoadResult {
    bool success;
    QString errorMessage;
    DicomMetadata metadata;
    vtkSmartPointer<vtkImageData> imageData;
};
```

**Public Methods:**
```cpp
// File-based loading
FullImageLoadResult loadFromFile(const QString& filePath);
FullImageLoadResult loadFromBytes(const QByteArray& data, ImageFormat format = UNKNOWN);

// Format-specific methods
FullImageLoadResult loadDicom(const QString& filePath);
FullImageLoadResult loadDicomFromBytes(const QByteArray& data);
FullImageLoadResult loadPng(const QString& filePath);
FullImageLoadResult loadJpeg(const QString& filePath);
FullImageLoadResult loadBmp(const QString& filePath);
FullImageLoadResult loadTiff(const QString& filePath);

// Utility methods
bool isSupportedFormat(const QString& filePath) const;
ImageFormat detectFormatFromContent(const QByteArray& data) const;
vtkSmartPointer<vtkImageData> convertToVtkImageData(...);
DicomMetadata createDefaultMetadata(int width, int height, int bitsAllocated);
```

### 7.2 DicomParserService

**Location:** `ImageViewer/Service/DicomParserService.h/cpp`

DCMTK-based DICOM file parser.

**Features:**
- Parse DICOM from file or memory
- Extract comprehensive metadata
- Extract raw pixel data
- Auto-detect bit depth and signed/unsigned
- Handle various transfer syntaxes

**Public Methods:**
```cpp
DicomMetadata parseMetadata(const QString& filePath);
DicomMetadata parseMetadata(const QByteArray& data);
QByteArray extractPixelData(const QString& filePath);
QByteArray extractPixelData(const QByteArray& data);
bool loadDicomFile(const QString& filePath, DicomMetadata& metadata, QByteArray& pixelData);
bool loadDicomData(const QByteArray& data, DicomMetadata& metadata, QByteArray& pixelData);
bool isDicomFile(const QString& filePath) const;
QString lastError() const;
```

---

## 8. Layout System

### ViewportLayout Enum

```cpp
enum class ViewportLayout {
    SINGLE,      // 1x1 - Single viewport
    ONE_BY_TWO,  // 1x2 - Two viewports side by side
    TWO_BY_ONE,  // 2x1 - Two viewports stacked
    TWO_BY_TWO   // 2x2 - Four viewports in grid
};
```

### Helper Functions

```cpp
int viewportCount(ViewportLayout layout);  // Returns 1, 2, or 4
void layoutDimensions(ViewportLayout layout, int& rows, int& cols);
```

### Layout Configurations

```
SINGLE (1x1):          ONE_BY_TWO (1x2):      TWO_BY_TWO (2x2):
┌───────────┐          ┌─────┬─────┐          ┌─────┬─────┐
│           │          │     │     │          │  0  │  1  │
│     0     │          │  0  │  1  │          ├─────┼─────┤
│           │          │     │     │          │  2  │  3  │
└───────────┘          └─────┴─────┘          └─────┴─────┘
```

### Implementation in ViewportGrid

- Maintains array of 4 `ImageViewport` objects
- Uses `QGridLayout` with dynamic repositioning
- `updateGridLayout()` reconfigures grid based on layout
- Shows/hides viewports based on current layout
- Maintains active viewport validity (resets if hidden)

---

## 9. User Interactions

### 9.1 Tool Interactions

| Tool | Mouse Action | Result |
|------|--------------|--------|
| **Zoom** | Left drag up | Zoom in |
| | Left drag down | Zoom out |
| | Mouse wheel | Zoom in/out |
| | Double-click | Fit to window |
| **Pan** | Left drag | Move image |
| | Middle drag | Always pans |
| **Window/Level** | Horizontal drag | Adjust contrast (window) |
| | Vertical drag | Adjust brightness (level) |
| | Mouse wheel | Quick adjustment |
| **Ruler** | Click + drag | Draw measurement line |
| | Release | Complete measurement |
| | Click on handle | Start dragging handle |
| | Drag handle | Resize ruler |
| | Click on line | Select ruler |
| | Delete key | Delete selected ruler |
| **Angle** | Click (1st) | Place first point |
| | Click (2nd) | Place vertex |
| | Click (3rd) | Complete angle |
| | Click on handle | Start dragging handle |
| | Drag handle | Resize angle |
| | Click on arm | Select angle |
| | Right-click | Cancel placement |
| | Delete key | Delete selected angle |
| **Reset** | Click | Reset to original state |

### 9.2 Viewport Interactions

- **Single-click** on viewport → Activates that viewport
- **Active viewport** → Green border (`#00FF00`)
- **Inactive viewport** → Gray border (`#404040`)

### 9.3 Thumbnail Panel Interactions

- **Single-click** → Select thumbnail
- **Double-click** → Load into active viewport

### 9.4 Drag & Drop

- Drop image files onto `ImageViewerPage`
- Supported: DICOM, PNG, JPEG, BMP, TIFF
- Emits `filesDropped(QStringList)` signal
- Files distributed to visible viewports

### 9.5 Keyboard Navigation

See keyboard shortcuts in [Section 4.1](#41-imageviewerpage).

---

## 10. Signal/Slot Data Flow

### Connection Hierarchy

```
ImageViewerPage (UI)
       │ signals
       ▼
ImageViewerPageDelegate (Business Logic)
       │ commands
       ▼
MultiViewportManager
       │ commands
       ▼
VtkViewportRenderer[0..3]
       │ renders to
       ▼
QVTKOpenGLNativeWidget[0..3]
```

### Interaction Flow Example (Zoom)

```
1. User drags in viewport
   → ImageViewport.mouseMoveEvent()

2. → emit mouseMoved(int index, QPointF pos, ...)

3. → ViewportGrid forwards signal
   → ImageViewerPage.viewportMouseMoved()

4. → ImageViewerPage emits
   → viewportMouseMoved(index, pos, ...)

5. → ImageViewerPageDelegate.onViewportMouseMoved() [slot]

6. → Delegate calls active tool
   → m_zoomTool->onMouseMove(pos)

7. → Tool calculates zoom factor
   → emit zoomRequested(factor, centerPos)

8. → Delegate slot
   → onZoomRequested(factor, centerPos)

9. → Delegate gets active renderer
   → m_viewportManager->activeRenderer()

10. → renderer->setZoom(factor)

11. → Renderer updates camera/view

12. → renderer->render()

13. → VTK widget updates display
```

### Signal Forwarding Chain

```
ImageViewport → ViewportGrid → ImageViewerPage → ImageViewerPageDelegate
ImageToolPanel → ImageViewerPage → ImageViewerPageDelegate
SeriesThumbnailPanel → ImageViewerPage → ImageViewerPageDelegate
Tool signals → ImageViewerPageDelegate
```

---

## 11. Design Patterns

### Builder Pattern
- `ImageViewerPageBuilder` creates widget and delegate together
- Handles dependency injection
- Returns pair `(ImageViewerPage*, ImageViewerPageDelegate*)`

### Delegate Pattern (MVD)
- `ImageViewerPageDelegate` encapsulates all business logic
- `ImageViewerPage` is pure UI (data display only)
- Clear separation: UI presentation vs. application logic

### Repository Pattern
- `DicomRepository` abstracts database access
- Delegate uses repository for database operations
- SOLID principle: Depend on abstractions

### Strategy Pattern (Implicit)
- Different tools implement `IImageTool` interface
- Delegate switches strategies via `activateTool()`
- Each tool encapsulates specific interaction behavior

### Observer Pattern
- Extensive Qt signal/slot connections
- Loose coupling between components
- Event-driven architecture

### Adapter Pattern
- `VtkViewportRenderer` adapts VTK pipeline to application needs
- Hides VTK complexity from higher layers
- Provides simple API (`setWindowLevel`, `setZoom`, etc.)

---

## 12. File Reference

| Component | Location |
|-----------|----------|
| **Page** | `View/Page/ImageViewerPage.h/cpp/ui` |
| **ViewportGrid** | `View/Widget/ViewportGrid.h/cpp/ui` |
| **ImageViewport** | `View/Widget/ImageViewport.h/cpp/ui` |
| **ImageToolPanel** | `View/Widget/ImageToolPanel.h/cpp/ui` |
| **SeriesThumbnailPanel** | `View/Widget/SeriesThumbnailPanel.h/cpp/ui` |
| **IImageTool** | `ImageViewer/Tool/IImageTool.h/cpp` |
| **ZoomTool** | `ImageViewer/Tool/ZoomTool.h/cpp` |
| **PanTool** | `ImageViewer/Tool/PanTool.h/cpp` |
| **WindowLevelTool** | `ImageViewer/Tool/WindowLevelTool.h/cpp` |
| **RulerTool** | `ImageViewer/Tool/RulerTool.h/cpp` |
| **AngleTool** | `ImageViewer/Tool/AngleTool.h/cpp` |
| **ResetTool** | `ImageViewer/Tool/ResetTool.h/cpp` |
| **VtkViewportRenderer** | `ImageViewer/Rendering/VtkViewportRenderer.h/cpp` |
| **MultiViewportManager** | `ImageViewer/Rendering/MultiViewportManager.h/cpp` |
| **ImageLoaderService** | `ImageViewer/Service/ImageLoaderService.h/cpp` |
| **DicomParserService** | `ImageViewer/Service/DicomParserService.h/cpp` |
| **RulerOverlayWidget** | `ImageViewer/Widget/RulerOverlayWidget.h/cpp` |
| **AngleOverlayWidget** | `ImageViewer/Widget/AngleOverlayWidget.h/cpp` |
| **MagnifierWidget** | `ImageViewer/Widget/MagnifierWidget.h/cpp` |
| **ImageDisplayParams** | `ImageViewer/Data/Entity/ImageDisplayParams.h` |
| **ImageViewerPageDelegate** | `Application/Delegate/ImageViewerPageDelegate.h/cpp` |
| **ImageViewerPageBuilder** | `Application/Builder/ImageViewerPageBuilder.h/cpp` |
| **IImageViewerContext** | `Common/Include/ImageViewer/IImageViewerContext.h` |
| **ImageViewerTypes** | `Common/Include/ImageViewer/ImageViewerTypes.h` |
| **ViewportLayout** | `Common/Include/ImageViewer/ViewportLayout.h` |
| **Icons** | `View/Asset/Icon/*.png` |

---

## 13. Build Configuration

**ImageViewer Module** (`ImageViewer/CMakeLists.txt`):

```cmake
# Library type
SHARED library (ImageViewer.dll)

# Export configuration
WINDOWS_EXPORT_ALL_SYMBOLS = TRUE

# Qt automation
CMAKE_AUTOMOC = ON
CMAKE_AUTOUIC = ON
CMAKE_AUTORCC = ON

# Dependencies
Qt6::Core, Qt6::Widgets, Qt6::Gui
VTK::CommonCore, VTK::RenderingCore, VTK::InteractionStyle,
VTK::RenderingOpenGL2, VTK::GUISupportQt
DCMTK::dcmdata, DCMTK::dcmimage, DCMTK::ofstd
Core, Common, View, Dicom

# Output
out/build/[debug|release]/ImageViewer.dll
```

---

## 14. Extension Points

### Adding New Tools

1. Create new class inheriting from `IImageTool`
2. Implement all virtual methods
3. Add new `ToolType` enum value to `ImageViewerTypes.h`
4. Add tool icon to `View/Asset/Icon/`
5. Register tool in `ImageViewerPageDelegate::initializeTools()`
6. Add button to `ImageToolPanel`
7. Connect signals in delegate

### Adding New Image Formats

1. Add format to `ImageFormat` enum in `ImageViewerTypes.h`
2. Implement format-specific loader in `ImageLoaderService`
3. Update `isSupportedFormat()` method
4. Update `detectFormatFromContent()` for magic byte detection

### Adding New Layouts

1. Add layout to `ViewportLayout` enum
2. Update `viewportCount()` helper function
3. Update `layoutDimensions()` helper function
4. Implement grid configuration in `ViewportGrid::updateGridLayout()`
5. Add layout icon and button to `ImageToolPanel`

### TODO Items (In Code)

1. `loadFromStudy()` - Database study loading (not yet implemented)
2. `loadFromSeries()` - Database series loading (not yet implemented)
3. Context-based image loading in `onPageLoaded()`
4. Linked mode synchronization enhancements
5. Measurement overlay rendering

---

## Appendix: Data Structures

### DicomMetadata

```cpp
struct DicomMetadata {
    // Patient Module
    QString patientName;
    QString patientId;
    QString patientBirthDate;
    QString patientSex;

    // Study Module
    QString studyInstanceUid;
    QString studyDate;
    QString studyTime;
    QString studyDescription;
    QString accessionNumber;
    QString referringPhysicianName;

    // Series Module
    QString seriesInstanceUid;
    QString modality;
    QString seriesDescription;
    QString bodyPartExamined;
    int seriesNumber;

    // Image Module
    QString sopInstanceUid;
    QString sopClassUid;
    int instanceNumber;
    int imageNumber;
    int totalImages;
    int totalSlices;

    // Image Pixel Module
    int rows;
    int columns;
    int bitsAllocated;
    int bitsStored;
    int highBit;
    bool pixelRepresentationSigned;
    QString photometricInterpretation;

    // VOI LUT Module
    double windowCenter;
    double windowWidth;

    // Modality LUT Module
    double rescaleIntercept;
    double rescaleSlope;

    // Pixel Spacing
    double pixelSpacingX;  // mm
    double pixelSpacingY;  // mm
};
```

### ViewportState

```cpp
struct ViewportState {
    double zoomFactor = 1.0;    // 1.0 = 100%
    double panX = 0.0;
    double panY = 0.0;
    double windowWidth = 256.0;
    double windowCenter = 128.0;
    bool inverted = false;
    int flipHorizontal = 0;
    int flipVertical = 0;
    int rotation = 0;          // degrees

    void reset();
    void initializeFromMetadata(const DicomMetadata& metadata);
};
```

### MeasurementLine

```cpp
struct MeasurementLine {
    QPointF startPoint;
    QPointF endPoint;
    double distanceMm = 0.0;      // Distance in millimeters
    double distancePixels = 0.0;  // Distance in pixels
    bool isComplete = false;
    int id = -1;                  // Unique identifier
    bool isSelected = false;      // Currently selected for editing

    void calculateDistance(double spacingX, double spacingY);
    bool isNearStartHandle(const QPointF& pos, double threshold = 10.0) const;
    bool isNearEndHandle(const QPointF& pos, double threshold = 10.0) const;
    bool isNearLine(const QPointF& pos, double threshold = 5.0) const;
};
```

### MeasurementAngle

```cpp
struct MeasurementAngle {
    QPointF point1;           // First endpoint (arm 1)
    QPointF vertex;           // Vertex (middle point where angle is measured)
    QPointF point2;           // Second endpoint (arm 2)
    double angleDegrees = 0.0;
    bool isComplete = false;
    int id = -1;              // Unique identifier
    bool isSelected = false;  // Currently selected for editing

    void calculateAngle();
    double getArm1Angle() const;  // Angle of arm 1 relative to X-axis (radians)
    double getArm2Angle() const;  // Angle of arm 2 relative to X-axis (radians)
    bool isNearPoint1Handle(const QPointF& pos, double threshold = 10.0) const;
    bool isNearVertexHandle(const QPointF& pos, double threshold = 10.0) const;
    bool isNearPoint2Handle(const QPointF& pos, double threshold = 10.0) const;
    bool isNearArm(const QPointF& pos, double threshold = 5.0) const;
};
```

### RulerOverlayWidget

Transparent overlay widget that renders measurement rulers using QPainter.

**Visual Elements:**
- Ruler lines: 2px width, cyan (#00BFFF)
- Handles: Square (10x10px), white fill with cyan border
- Hovered handle: Yellow fill
- Selected ruler: Gold color
- Distance label: White text with black outline, format: `25.4 (mm)`

**Key Methods:**
```cpp
void setImageToWidgetTransform(CoordinateTransform transform);
void setMeasurements(const QVector<MeasurementLine>& measurements);
void setCurrentMeasurement(const MeasurementLine& line);
void setSelectedRuler(int rulerId);
void setHoveredHandle(int rulerId, bool isStartHandle);
void clearHoveredHandle();
void refresh();
```

### AngleOverlayWidget

Transparent overlay widget that renders angle measurements using QPainter.

**Visual Elements:**
- Arm lines: 2px width, cyan (#00BFFF)
- Arc: 30px radius at vertex, cyan
- Handles: Square (10x10px) at all three points
- Hovered handle: Yellow fill
- Selected angle: Gold color
- Angle label: White text with black outline, format: `45.2°`

**Key Methods:**
```cpp
void setImageToWidgetTransform(CoordinateTransform transform);
void setMeasurements(const QVector<MeasurementAngle>& measurements);
void setCurrentMeasurement(const MeasurementAngle& angle);
void setSelectedAngle(int angleId);
void setHoveredHandle(int angleId, AngleHandleType handleType);
void clearHoveredHandle();
void refresh();
```

---

*Document Version: 1.1*
*Last Updated: January 2026*
*Author: Generated for Etrek Medical Imaging Application*
*Changes: Added interactive editing documentation for Ruler and Angle tools*
