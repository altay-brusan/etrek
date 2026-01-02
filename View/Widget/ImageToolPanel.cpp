#include "ImageToolPanel.h"
#include "ui_ImageToolPanel.h"

#include <QToolButton>
#include <QButtonGroup>
#include <QVBoxLayout>
#include <QFrame>
#include <QLabel>

using namespace Etrek::ImageViewer;

ImageToolPanel::ImageToolPanel(QWidget* parent)
    : ImageToolPanel(ImageToolPanelConfig::fullFeatures(), parent)
{
}

ImageToolPanel::ImageToolPanel(const ImageToolPanelConfig& config, QWidget* parent)
    : QWidget(parent)
    , ui(new Ui::ImageToolPanel)
    , m_config(config)
    , m_toolButtonGroup(new QButtonGroup(this))
    , m_layoutButtonGroup(new QButtonGroup(this))
{
    ui->setupUi(this);
    initialize();
}

void ImageToolPanel::initialize()
{
    // Set up button groups
    m_toolButtonGroup->setExclusive(true);
    m_layoutButtonGroup->setExclusive(true);

    // Create buttons based on config
    setupToolButtons();
    setupLayoutButtons();
    setupActionButtons();

    // Connect signals
    connect(m_toolButtonGroup, &QButtonGroup::idClicked,
            this, &ImageToolPanel::onToolButtonClicked);
    connect(m_layoutButtonGroup, &QButtonGroup::idClicked,
            this, &ImageToolPanel::onLayoutButtonClicked);

    // Set default selections
    setSelectedTool(ToolType::WINDOW_LEVEL);
    if (m_config.showLayoutSection) {
        setSelectedLayout(ViewportLayout::SINGLE);
    }
}

ImageToolPanel::~ImageToolPanel() {
    delete ui;
}

void ImageToolPanel::setupToolButtons() {
    // Helper lambda to add a tool button if config allows
    auto addToolButton = [this](bool show, const QString& icon, const QString& tooltip,
                                 const QString& text, ToolType type) {
        if (!show) return;

        auto* btn = createToolButton(icon, tooltip);
        btn->setText(text);
        m_toolButtonGroup->addButton(btn, static_cast<int>(type));
        m_toolButtons[type] = btn;
        ui->toolsLayout->addWidget(btn);
    };

    // Create tool buttons based on config
    addToolButton(m_config.showZoom, ":/Images/Asset/Icon/zoom.png", "Zoom (Z)", "Z", ToolType::ZOOM);
    addToolButton(m_config.showPan, ":/Images/Asset/Icon/pan.png", "Pan (P)", "P", ToolType::PAN);
    addToolButton(m_config.showWindowLevel, ":/Images/Asset/Icon/windowlevel.png", "Window/Level (W)", "W/L", ToolType::WINDOW_LEVEL);
    addToolButton(m_config.showRuler, ":/Images/Asset/Icon/ruler.png", "Ruler (R)", "R", ToolType::RULER);
    addToolButton(m_config.showAngle, ":/Images/Asset/Icon/angle.png", "Angle (A)", "A", ToolType::ANGLE);

    // Reset button is special - it's a momentary action, not a toggle tool
    if (m_config.showReset) {
        auto* resetBtn = createToolButton(":/Images/Asset/Icon/reset.png", "Reset View");
        resetBtn->setText("RST");
        resetBtn->setCheckable(false);  // Momentary action, not toggle
        m_toolButtons[ToolType::RESET] = resetBtn;
        ui->toolsLayout->addWidget(resetBtn);

        // Connect reset button directly to resetViewRequested signal
        connect(resetBtn, &QToolButton::clicked, this, &ImageToolPanel::resetViewRequested);
    }
}

void ImageToolPanel::setupLayoutButtons() {
    // Hide entire layout section if not configured
    if (!m_config.showLayoutSection) {
        ui->separator1->hide();
        ui->layoutLabel->hide();
        // Don't create any layout buttons
        return;
    }

    auto* layout1x1 = createToolButton(":/Images/Asset/Icon/layout1x1.png", "1x1 Layout");
    auto* layout1x2 = createToolButton(":/Images/Asset/Icon/layout1x2.png", "1x2 Layout");
    auto* layout2x2 = createToolButton(":/Images/Asset/Icon/layout2x2.png", "2x2 Layout");

    // Set text fallback
    layout1x1->setText("1x1");
    layout1x2->setText("1x2");
    layout2x2->setText("2x2");

    // Add to button group
    m_layoutButtonGroup->addButton(layout1x1, static_cast<int>(ViewportLayout::SINGLE));
    m_layoutButtonGroup->addButton(layout1x2, static_cast<int>(ViewportLayout::ONE_BY_TWO));
    m_layoutButtonGroup->addButton(layout2x2, static_cast<int>(ViewportLayout::TWO_BY_TWO));

    // Store references
    m_layoutButtons[ViewportLayout::SINGLE] = layout1x1;
    m_layoutButtons[ViewportLayout::ONE_BY_TWO] = layout1x2;
    m_layoutButtons[ViewportLayout::TWO_BY_TWO] = layout2x2;

    // Add to layout
    ui->layoutsLayout->addWidget(layout1x1);
    ui->layoutsLayout->addWidget(layout1x2);
    ui->layoutsLayout->addWidget(layout2x2);
}

void ImageToolPanel::setupActionButtons() {
    bool hasAnyAction = m_config.showOpenFile || m_config.showInvert || m_config.showFit;

    // Hide actions section if no action buttons are configured
    if (!hasAnyAction) {
        ui->separator2->hide();
        ui->actionsLabel->hide();
        return;
    }

    // Open file button
    if (m_config.showOpenFile) {
        auto* openBtn = createToolButton(":/Images/Asset/Icon/open.png", "Open File (Ctrl+O)");
        openBtn->setText("Open");
        openBtn->setCheckable(false);
        connect(openBtn, &QToolButton::clicked, this, &ImageToolPanel::openFileRequested);
        ui->actionsLayout->addWidget(openBtn);
    }

    // Invert button
    if (m_config.showInvert) {
        auto* invertBtn = createToolButton(":/Images/Asset/Icon/invert.png", "Invert Image (I)");
        invertBtn->setText("Inv");
        invertBtn->setCheckable(false);
        connect(invertBtn, &QToolButton::clicked, this, &ImageToolPanel::invertRequested);
        ui->actionsLayout->addWidget(invertBtn);
    }

    // Fit to window button
    if (m_config.showFit) {
        auto* fitBtn = createToolButton(":/Images/Asset/Icon/fit.png", "Fit to Window (F)");
        fitBtn->setText("Fit");
        fitBtn->setCheckable(false);
        connect(fitBtn, &QToolButton::clicked, this, &ImageToolPanel::fitToWindowRequested);
        ui->actionsLayout->addWidget(fitBtn);
    }
}

QToolButton* ImageToolPanel::createToolButton(const QString& iconPath, const QString& tooltip) {
    auto* button = new QToolButton(this);
    button->setCheckable(true);
    button->setAutoExclusive(false);  // Managed by button group
    button->setToolTip(tooltip);
    button->setMinimumSize(40, 40);
    button->setMaximumSize(40, 40);
    button->setIconSize(QSize(24, 24));

    // Try to load icon
    QIcon icon(iconPath);
    if (!icon.isNull()) {
        button->setIcon(icon);
    }

    // Style
    button->setStyleSheet(
        "QToolButton {"
        "   background-color: rgb(60, 60, 60);"
        "   border: 1px solid rgb(80, 80, 80);"
        "   border-radius: 4px;"
        "   color: rgb(200, 200, 200);"
        "   font-size: 10px;"
        "}"
        "QToolButton:hover {"
        "   background-color: rgb(80, 80, 80);"
        "}"
        "QToolButton:checked {"
        "   background-color: rgb(0, 120, 200);"
        "   border: 1px solid rgb(0, 150, 255);"
        "}"
        "QToolButton:pressed {"
        "   background-color: rgb(0, 100, 180);"
        "}"
    );

    return button;
}

ToolType ImageToolPanel::selectedTool() const {
    return m_selectedTool;
}

void ImageToolPanel::setSelectedTool(ToolType tool) {
    m_selectedTool = tool;
    if (m_toolButtons.contains(tool)) {
        m_toolButtons[tool]->setChecked(true);
    }
}

ViewportLayout ImageToolPanel::selectedLayout() const {
    return m_selectedLayout;
}

void ImageToolPanel::setSelectedLayout(ViewportLayout layout) {
    m_selectedLayout = layout;
    if (m_layoutButtons.contains(layout)) {
        m_layoutButtons[layout]->setChecked(true);
    }
}

void ImageToolPanel::setToolEnabled(ToolType tool, bool enabled) {
    if (m_toolButtons.contains(tool)) {
        m_toolButtons[tool]->setEnabled(enabled);
    }
}

void ImageToolPanel::onToolButtonClicked(int id) {
    ToolType tool = static_cast<ToolType>(id);
    if (tool != m_selectedTool) {
        m_selectedTool = tool;
        emit toolSelected(tool);
    }
}

void ImageToolPanel::onLayoutButtonClicked(int id) {
    ViewportLayout layout = static_cast<ViewportLayout>(id);
    if (layout != m_selectedLayout) {
        m_selectedLayout = layout;
        emit layoutSelected(layout);
    }
}
