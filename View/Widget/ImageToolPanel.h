#ifndef IMAGETOOLPANEL_H
#define IMAGETOOLPANEL_H

#include <QWidget>
#include <QMap>

#include "ImageViewerTypes.h"
#include "ViewportLayout.h"

class QToolButton;
class QButtonGroup;

namespace Ui {
class ImageToolPanel;
}

/**
 * @struct ImageToolPanelConfig
 * @brief Configuration for ImageToolPanel button visibility.
 *
 * Use this to customize which buttons and sections are shown
 * in the ImageToolPanel for different contexts (e.g., ImageViewerPage vs ExamPage).
 */
struct ImageToolPanelConfig {
    // Tool buttons
    bool showZoom = true;
    bool showPan = true;
    bool showWindowLevel = true;
    bool showRuler = true;
    bool showAngle = true;
    bool showReset = true;

    // Layout section (entire section including label and separator)
    bool showLayoutSection = true;

    // Action buttons
    bool showOpenFile = true;
    bool showInvert = true;
    bool showFit = true;

    /**
     * @brief Create config with all features enabled (for ImageViewerPage).
     */
    static ImageToolPanelConfig fullFeatures() {
        return ImageToolPanelConfig{};  // All defaults are true
    }

    /**
     * @brief Create config for ExamPage (no layout, no open file).
     */
    static ImageToolPanelConfig examPageFeatures() {
        ImageToolPanelConfig config;
        config.showLayoutSection = false;
        config.showOpenFile = false;
        return config;
    }
};

/**
 * @class ImageToolPanel
 * @brief Left toolbar panel with imaging tool buttons and layout selection.
 *
 * Contains:
 * - Tool selection buttons (zoom, pan, window/level, ruler, angle, reset)
 * - Layout selection buttons (1x1, 1x2, 2x2)
 * - Additional action buttons (open file, invert, etc.)
 *
 * Use ImageToolPanelConfig to customize which buttons are visible.
 */
class ImageToolPanel : public QWidget
{
    Q_OBJECT

public:
    /**
     * @brief Construct with default config (all features).
     */
    explicit ImageToolPanel(QWidget* parent = nullptr);

    /**
     * @brief Construct with custom configuration.
     */
    explicit ImageToolPanel(const ImageToolPanelConfig& config, QWidget* parent = nullptr);

    ~ImageToolPanel() override;

    /**
     * @brief Get the currently selected tool.
     */
    Etrek::ImageViewer::ToolType selectedTool() const;

    /**
     * @brief Set the selected tool.
     */
    void setSelectedTool(Etrek::ImageViewer::ToolType tool);

    /**
     * @brief Get the currently selected layout.
     */
    Etrek::ImageViewer::ViewportLayout selectedLayout() const;

    /**
     * @brief Set the selected layout.
     */
    void setSelectedLayout(Etrek::ImageViewer::ViewportLayout layout);

    /**
     * @brief Enable/disable specific tool buttons.
     */
    void setToolEnabled(Etrek::ImageViewer::ToolType tool, bool enabled);

signals:
    /**
     * @brief Emitted when a tool is selected.
     */
    void toolSelected(Etrek::ImageViewer::ToolType tool);

    /**
     * @brief Emitted when layout is changed.
     */
    void layoutSelected(Etrek::ImageViewer::ViewportLayout layout);

    /**
     * @brief Emitted when Open File button is clicked.
     */
    void openFileRequested();

    /**
     * @brief Emitted when Invert button is clicked.
     */
    void invertRequested();

    /**
     * @brief Emitted when Reset View button is clicked.
     */
    void resetViewRequested();

    /**
     * @brief Emitted when Fit to Window button is clicked.
     */
    void fitToWindowRequested();

private slots:
    void onToolButtonClicked(int id);
    void onLayoutButtonClicked(int id);

private:
    void setupToolButtons();
    void setupLayoutButtons();
    void setupActionButtons();
    QToolButton* createToolButton(const QString& iconPath, const QString& tooltip);

    void initialize();

    Ui::ImageToolPanel* ui;
    ImageToolPanelConfig m_config;
    QButtonGroup* m_toolButtonGroup;
    QButtonGroup* m_layoutButtonGroup;
    QMap<Etrek::ImageViewer::ToolType, QToolButton*> m_toolButtons;
    QMap<Etrek::ImageViewer::ViewportLayout, QToolButton*> m_layoutButtons;
    Etrek::ImageViewer::ToolType m_selectedTool = Etrek::ImageViewer::ToolType::WINDOW_LEVEL;
    Etrek::ImageViewer::ViewportLayout m_selectedLayout = Etrek::ImageViewer::ViewportLayout::SINGLE;
};

#endif // IMAGETOOLPANEL_H
