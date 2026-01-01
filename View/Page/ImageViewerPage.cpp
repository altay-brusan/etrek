#include "ImageViewerPage.h"
#include "ui_ImageViewerPage.h"

#include "ViewportGrid.h"
#include "ImageToolPanel.h"
#include "SeriesThumbnailPanel.h"

#include <QVTKOpenGLNativeWidget.h>
#include <QHBoxLayout>
#include <QSplitter>
#include <QDragEnterEvent>
#include <QDropEvent>
#include <QMimeData>
#include <QKeyEvent>
#include <QUrl>
#include <QFileInfo>

using namespace Etrek::ImageViewer;

ImageViewerPage::ImageViewerPage(QWidget* parent)
    : QWidget(parent)
    , ui(new Ui::ImageViewerPage)
{
    ui->setupUi(this);

    // Create widgets
    m_toolPanel = new ImageToolPanel(this);
    m_viewportGrid = new ViewportGrid(this);
    m_thumbnailPanel = new SeriesThumbnailPanel(this);

    // Create splitter for resizable panels
    QSplitter* mainSplitter = new QSplitter(Qt::Horizontal, this);
    mainSplitter->addWidget(m_toolPanel);
    mainSplitter->addWidget(m_viewportGrid);
    mainSplitter->addWidget(m_thumbnailPanel);

    // Set stretch factors - viewports get most space
    mainSplitter->setStretchFactor(0, 0);  // Tool panel - fixed
    mainSplitter->setStretchFactor(1, 1);  // Viewport grid - expandable
    mainSplitter->setStretchFactor(2, 0);  // Thumbnail panel - fixed

    // Set initial sizes
    mainSplitter->setSizes({60, 700, 150});

    // Add splitter to main layout
    QHBoxLayout* mainLayout = new QHBoxLayout(this);
    mainLayout->setContentsMargins(0, 0, 0, 0);
    mainLayout->setSpacing(0);
    mainLayout->addWidget(mainSplitter);
    setLayout(mainLayout);

    // Enable drag and drop
    setAcceptDrops(true);

    // Setup signal connections
    setupConnections();

    // Apply dark theme
    setStyleSheet("background-color: rgb(30, 30, 30);");
}

ImageViewerPage::~ImageViewerPage() {
    delete ui;
}

void ImageViewerPage::setupConnections() {
    // Tool panel signals
    connect(m_toolPanel, &ImageToolPanel::toolSelected,
            this, &ImageViewerPage::toolSelected);
    connect(m_toolPanel, &ImageToolPanel::layoutSelected,
            this, &ImageViewerPage::layoutSelected);
    connect(m_toolPanel, &ImageToolPanel::openFileRequested,
            this, &ImageViewerPage::openFileRequested);
    connect(m_toolPanel, &ImageToolPanel::invertRequested,
            this, &ImageViewerPage::invertRequested);
    connect(m_toolPanel, &ImageToolPanel::resetViewRequested,
            this, &ImageViewerPage::resetViewRequested);
    connect(m_toolPanel, &ImageToolPanel::fitToWindowRequested,
            this, &ImageViewerPage::fitToWindowRequested);

    // Viewport grid signals
    connect(m_viewportGrid, &ViewportGrid::activeViewportChanged,
            this, &ImageViewerPage::activeViewportChanged);
    connect(m_viewportGrid, &ViewportGrid::viewportMousePressed,
            this, &ImageViewerPage::viewportMousePressed);
    connect(m_viewportGrid, &ViewportGrid::viewportMouseMoved,
            this, &ImageViewerPage::viewportMouseMoved);
    connect(m_viewportGrid, &ViewportGrid::viewportMouseReleased,
            this, &ImageViewerPage::viewportMouseReleased);
    connect(m_viewportGrid, &ViewportGrid::viewportMouseWheel,
            this, &ImageViewerPage::viewportMouseWheel);
    connect(m_viewportGrid, &ViewportGrid::viewportDoubleClicked,
            this, &ImageViewerPage::viewportDoubleClicked);

    // Thumbnail panel signals
    connect(m_thumbnailPanel, &SeriesThumbnailPanel::thumbnailClicked,
            this, &ImageViewerPage::thumbnailClicked);
    connect(m_thumbnailPanel, &SeriesThumbnailPanel::thumbnailDoubleClicked,
            this, &ImageViewerPage::thumbnailDoubleClicked);

    // Sync layout selection between tool panel and viewport grid
    connect(m_toolPanel, &ImageToolPanel::layoutSelected,
            m_viewportGrid, &ViewportGrid::setViewportLayout);
}

ViewportGrid* ImageViewerPage::getViewportGrid() const {
    return m_viewportGrid;
}

ImageToolPanel* ImageViewerPage::getToolPanel() const {
    return m_toolPanel;
}

SeriesThumbnailPanel* ImageViewerPage::getThumbnailPanel() const {
    return m_thumbnailPanel;
}

QVTKOpenGLNativeWidget* ImageViewerPage::getVtkWidget(int viewportIndex) {
    return m_viewportGrid->getVtkWidget(viewportIndex);
}

std::array<QVTKOpenGLNativeWidget*, 4> ImageViewerPage::getAllVtkWidgets() {
    return m_viewportGrid->getAllVtkWidgets();
}

void ImageViewerPage::setThumbnailPanelVisible(bool visible) {
    m_thumbnailPanel->setVisible(visible);
}

bool ImageViewerPage::isThumbnailPanelVisible() const {
    return m_thumbnailPanel->isVisible();
}

void ImageViewerPage::dragEnterEvent(QDragEnterEvent* event) {
    if (event->mimeData()->hasUrls()) {
        // Check if any URLs are files
        for (const QUrl& url : event->mimeData()->urls()) {
            if (url.isLocalFile()) {
                event->acceptProposedAction();
                return;
            }
        }
    }
    event->ignore();
}

void ImageViewerPage::dragMoveEvent(QDragMoveEvent* event) {
    if (event->mimeData()->hasUrls()) {
        event->acceptProposedAction();
    }
}

void ImageViewerPage::dropEvent(QDropEvent* event) {
    QStringList filePaths;

    for (const QUrl& url : event->mimeData()->urls()) {
        if (url.isLocalFile()) {
            QString path = url.toLocalFile();
            QFileInfo fileInfo(path);

            if (fileInfo.exists() && fileInfo.isFile()) {
                filePaths.append(path);
            }
        }
    }

    if (!filePaths.isEmpty()) {
        emit filesDropped(filePaths);
        event->acceptProposedAction();
    } else {
        event->ignore();
    }
}

void ImageViewerPage::keyPressEvent(QKeyEvent* event) {
    // Keyboard shortcuts
    switch (event->key()) {
        case Qt::Key_Z:
            m_toolPanel->setSelectedTool(ToolType::ZOOM);
            emit toolSelected(ToolType::ZOOM);
            break;
        case Qt::Key_P:
            m_toolPanel->setSelectedTool(ToolType::PAN);
            emit toolSelected(ToolType::PAN);
            break;
        case Qt::Key_W:
            m_toolPanel->setSelectedTool(ToolType::WINDOW_LEVEL);
            emit toolSelected(ToolType::WINDOW_LEVEL);
            break;
        case Qt::Key_R:
            m_toolPanel->setSelectedTool(ToolType::RULER);
            emit toolSelected(ToolType::RULER);
            break;
        case Qt::Key_A:
            m_toolPanel->setSelectedTool(ToolType::ANGLE);
            emit toolSelected(ToolType::ANGLE);
            break;
        case Qt::Key_I:
            emit invertRequested();
            break;
        case Qt::Key_F:
            emit fitToWindowRequested();
            break;
        case Qt::Key_1:
            m_toolPanel->setSelectedLayout(ViewportLayout::SINGLE);
            emit layoutSelected(ViewportLayout::SINGLE);
            break;
        case Qt::Key_2:
            m_toolPanel->setSelectedLayout(ViewportLayout::ONE_BY_TWO);
            emit layoutSelected(ViewportLayout::ONE_BY_TWO);
            break;
        case Qt::Key_4:
            m_toolPanel->setSelectedLayout(ViewportLayout::TWO_BY_TWO);
            emit layoutSelected(ViewportLayout::TWO_BY_TWO);
            break;
        case Qt::Key_O:
            if (event->modifiers() & Qt::ControlModifier) {
                emit openFileRequested();
            }
            break;
        default:
            QWidget::keyPressEvent(event);
            break;
    }
}
