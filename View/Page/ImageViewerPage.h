#ifndef IMAGEVIEWERPAGE_H
#define IMAGEVIEWERPAGE_H

#include <QWidget>
#include <QPointF>

#include "ViewportLayout.h"
#include "ImageViewerTypes.h"

// Forward declarations
class ViewportGrid;
class ImageToolPanel;
class SeriesThumbnailPanel;
class QVTKOpenGLNativeWidget;

namespace Ui {
class ImageViewerPage;
}

/**
 * @class ImageViewerPage
 * @brief Main page for DICOM/medical image viewing.
 *
 * Layout:
 * - Left: ImageToolPanel (tool selection, layout buttons)
 * - Center: ViewportGrid (1-4 image viewports)
 * - Right: SeriesThumbnailPanel (optional, collapsible)
 *
 * Features:
 * - Multi-viewport layout (1x1, 1x2, 2x2)
 * - Drag and drop support for loading files
 * - Tool selection (zoom, pan, W/L, measurements)
 */
class ImageViewerPage : public QWidget
{
    Q_OBJECT

public:
    explicit ImageViewerPage(QWidget* parent = nullptr);
    ~ImageViewerPage() override;

    // Widget accessors for delegate
    ViewportGrid* getViewportGrid() const;
    ImageToolPanel* getToolPanel() const;
    SeriesThumbnailPanel* getThumbnailPanel() const;

    /**
     * @brief Get VTK widget for a specific viewport.
     */
    QVTKOpenGLNativeWidget* getVtkWidget(int viewportIndex);

    /**
     * @brief Get all VTK widgets.
     */
    std::array<QVTKOpenGLNativeWidget*, 4> getAllVtkWidgets();

    /**
     * @brief Show/hide the thumbnail panel.
     */
    void setThumbnailPanelVisible(bool visible);
    bool isThumbnailPanelVisible() const;

signals:
    // Forwarded from tool panel
    void toolSelected(Etrek::ImageViewer::ToolType tool);
    void layoutSelected(Etrek::ImageViewer::ViewportLayout layout);
    void openFileRequested();
    void invertRequested();
    void resetViewRequested();
    void fitToWindowRequested();

    // Forwarded from viewport grid
    void activeViewportChanged(int index);
    void viewportMousePressed(int viewportIndex, const QPointF& pos, Qt::MouseButton button, Qt::KeyboardModifiers modifiers);
    void viewportMouseMoved(int viewportIndex, const QPointF& pos, Qt::MouseButtons buttons, Qt::KeyboardModifiers modifiers);
    void viewportMouseReleased(int viewportIndex, const QPointF& pos, Qt::MouseButton button, Qt::KeyboardModifiers modifiers);
    void viewportMouseWheel(int viewportIndex, int delta, const QPointF& pos, Qt::KeyboardModifiers modifiers);
    void viewportDoubleClicked(int viewportIndex, const QPointF& pos, Qt::MouseButton button);

    // Forwarded from thumbnail panel
    void thumbnailClicked(int id, bool isSeries);
    void thumbnailDoubleClicked(int id, bool isSeries);

    // Drag and drop
    void filesDropped(const QStringList& filePaths);

    // Close request - emitted when user wants to close ImageViewer
    void closeRequested();

protected:
    void dragEnterEvent(QDragEnterEvent* event) override;
    void dragMoveEvent(QDragMoveEvent* event) override;
    void dropEvent(QDropEvent* event) override;
    void keyPressEvent(QKeyEvent* event) override;

private:
    void setupConnections();

    Ui::ImageViewerPage* ui;
    ViewportGrid* m_viewportGrid;
    ImageToolPanel* m_toolPanel;
    SeriesThumbnailPanel* m_thumbnailPanel;
};

#endif // IMAGEVIEWERPAGE_H
