#include "ViewportGrid.h"
#include "ui_ViewportGrid.h"
#include "ImageViewport.h"

#include <QGridLayout>
#include <QVTKOpenGLNativeWidget.h>

using namespace Etrek::ImageViewer;

ViewportGrid::ViewportGrid(QWidget* parent)
    : QWidget(parent)
    , ui(new Ui::ViewportGrid)
    , m_gridLayout(nullptr)
{
    ui->setupUi(this);

    // Create grid layout
    m_gridLayout = new QGridLayout(this);
    m_gridLayout->setContentsMargins(0, 0, 0, 0);
    m_gridLayout->setSpacing(2);
    setLayout(m_gridLayout);

    // Initialize viewports array
    m_viewports.fill(nullptr);

    // Create all viewports
    createViewports();

    // Set initial layout
    updateGridLayout();
}

ViewportGrid::~ViewportGrid() {
    delete ui;
}

void ViewportGrid::createViewports() {
    for (int i = 0; i < MAX_VIEWPORTS; ++i) {
        m_viewports[i] = new ImageViewport(this);
        m_viewports[i]->setIndex(i);
        connectViewportSignals(m_viewports[i]);
    }

    // Set first viewport as active
    m_viewports[0]->setActive(true);
}

void ViewportGrid::connectViewportSignals(ImageViewport* viewport) {
    connect(viewport, &ImageViewport::activated,
            this, &ViewportGrid::onViewportActivated);

    connect(viewport, &ImageViewport::mousePressed,
            this, &ViewportGrid::viewportMousePressed);

    connect(viewport, &ImageViewport::mouseMoved,
            this, &ViewportGrid::viewportMouseMoved);

    connect(viewport, &ImageViewport::mouseReleased,
            this, &ViewportGrid::viewportMouseReleased);

    connect(viewport, &ImageViewport::mouseWheelScrolled,
            this, &ViewportGrid::viewportMouseWheel);

    connect(viewport, &ImageViewport::mouseDoubleClicked,
            this, &ViewportGrid::viewportDoubleClicked);
}

void ViewportGrid::setViewportLayout(ViewportLayout layout) {
    if (m_currentLayout == layout) {
        return;
    }

    m_currentLayout = layout;
    updateGridLayout();

    // Ensure active viewport is visible
    int visibleCount = visibleViewportCount();
    if (m_activeIndex >= visibleCount) {
        setActiveViewport(0);
    }

    emit layoutChanged(layout);
}

ViewportLayout ViewportGrid::currentLayout() const {
    return m_currentLayout;
}

void ViewportGrid::updateGridLayout() {
    // Remove all widgets from layout
    while (m_gridLayout->count() > 0) {
        QLayoutItem* item = m_gridLayout->takeAt(0);
        if (item->widget()) {
            item->widget()->hide();
        }
        delete item;
    }

    int rows, cols;
    layoutDimensions(m_currentLayout, rows, cols);

    int viewportIndex = 0;
    for (int r = 0; r < rows; ++r) {
        for (int c = 0; c < cols; ++c) {
            if (viewportIndex < MAX_VIEWPORTS && m_viewports[viewportIndex]) {
                m_gridLayout->addWidget(m_viewports[viewportIndex], r, c);
                m_viewports[viewportIndex]->show();
            }
            ++viewportIndex;
        }
    }

    // Hide unused viewports
    for (int i = viewportIndex; i < MAX_VIEWPORTS; ++i) {
        if (m_viewports[i]) {
            m_viewports[i]->hide();
        }
    }

    // Set equal row/column stretch
    for (int r = 0; r < rows; ++r) {
        m_gridLayout->setRowStretch(r, 1);
    }
    for (int c = 0; c < cols; ++c) {
        m_gridLayout->setColumnStretch(c, 1);
    }
}

ImageViewport* ViewportGrid::getViewport(int index) {
    if (index >= 0 && index < MAX_VIEWPORTS) {
        return m_viewports[index];
    }
    return nullptr;
}

const ImageViewport* ViewportGrid::getViewport(int index) const {
    if (index >= 0 && index < MAX_VIEWPORTS) {
        return m_viewports[index];
    }
    return nullptr;
}

QVTKOpenGLNativeWidget* ViewportGrid::getVtkWidget(int index) {
    if (auto* viewport = getViewport(index)) {
        return viewport->vtkWidget();
    }
    return nullptr;
}

std::array<QVTKOpenGLNativeWidget*, ViewportGrid::MAX_VIEWPORTS> ViewportGrid::getAllVtkWidgets() {
    std::array<QVTKOpenGLNativeWidget*, ViewportGrid::MAX_VIEWPORTS> widgets;
    for (int i = 0; i < MAX_VIEWPORTS; ++i) {
        widgets[i] = m_viewports[i] ? m_viewports[i]->vtkWidget() : nullptr;
    }
    return widgets;
}

int ViewportGrid::activeViewportIndex() const {
    return m_activeIndex;
}

void ViewportGrid::setActiveViewport(int index) {
    if (index < 0 || index >= visibleViewportCount()) {
        return;
    }

    if (m_activeIndex != index) {
        // Deactivate old
        if (m_activeIndex >= 0 && m_activeIndex < MAX_VIEWPORTS && m_viewports[m_activeIndex]) {
            m_viewports[m_activeIndex]->setActive(false);
        }

        // Activate new
        m_activeIndex = index;
        if (m_viewports[m_activeIndex]) {
            m_viewports[m_activeIndex]->setActive(true);
        }

        emit activeViewportChanged(m_activeIndex);
    }
}

int ViewportGrid::visibleViewportCount() const {
    return viewportCount(m_currentLayout);
}

void ViewportGrid::onViewportActivated(int index) {
    setActiveViewport(index);
}

void ViewportGrid::resizeEvent(QResizeEvent* event) {
    QWidget::resizeEvent(event);
    // Grid layout handles resizing automatically
}
